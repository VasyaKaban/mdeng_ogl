#include "Device.h"
#include "../Instance/Instance.h"
#include "../PhysicalDevice/PhysicalDevice.h"
#include "../Surface/Surface.h"
#include "../Semaphore/Semaphore.h"
#include "../Queue/Queue.h"
#include "../Buffer/Buffer.h"
#include "../CommandPool/CommandPool.h"
#include "../DescriptorPool/DescriptorPool.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../Fence/Fence.h"
#include "../Framebuffer/Framebuffer.h"
#include "../Image/Image.h"
#include "../ImageView/ImageView.h"
#include "../Pipeline/Pipeline.h"
#include "../RenderPass/RenderPass.h"
#include "../Sampler/Sampler.h"
#include "../Shader/Shader.h"
#include "../Swapchain/Swapchain.h"

namespace OpenGL
{
    Device::Device(PhysicalDevice* _parent, const Render::DeviceInfo& info)
        : parent(_parent),
          surface(static_cast<Surface*>(info.surface)),
          swapchain(new Swapchain(this, surface, info.swapchain_info)),
          enabled_features(info.enabled_features),
          default_queue(new Queue(this))
    {
        Surface* impl_surface = static_cast<Surface*>(info.surface);

        const SurfaceConnectInfo connection_info = {
            .physical_device = parent,
            .config_index = info.swapchain_info.surface_config_index,
            .robust_buffer_access_enabled = info.enabled_features.robust_buffer_access,
        };

        impl_surface->Connect(connection_info); //we need to make current context

        swapchain->Recreate(info.swapchain_info);

        int glad_ver = gladLoadGLContext(&loader, parent->GetProcAddressResolver());
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        loader.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //use only seamless cubemaps
        loader.ClipControl(/*GL_UPPER_LEFT*/ GL_LOWER_LEFT,
                           GL_ZERO_TO_ONE); //make viewport like in D3D or VK(only depth)
        loader.Enable(GL_PROGRAM_POINT_SIZE); //activate GLSL gl_PointSize
        loader.ProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
        loader.Enable(GL_FRAMEBUFFER_SRGB);
        loader.Enable(GL_LINE_SMOOTH);

        Instance* impl_instance = static_cast<Instance*>(parent->GetParent());
        if(impl_instance->GetEnabledFeatures().validation_layer ||
           impl_instance->GetEnabledFeatures().debug_messenger)
        {
            loader.Enable(GL_DEBUG_OUTPUT);
#ifndef NDEBUG
            loader.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
            loader.DebugMessageControl(GL_DONT_CARE,
                                       GL_DONT_CARE,
                                       GL_DONT_CARE,
                                       0,
                                       nullptr,
                                       GL_TRUE); //drop all filters

            SetDebugMessenger(impl_instance->GetDebugMessengerInfo());
        }
    }

    Device::~Device()
    {
        WaitIdle();

        delete swapchain;
        delete default_queue;

        parent->DeleteDeviceNotify();
    }

    Render::Queue* Device::GetQueue(const Render::QueueInfo& info)
    {
        if(!(info.family_index == 0 && info.index == 0))
            throw std::runtime_error("Bad queue family index or queue index");

        //just return 'empty' queue -> we don't have a 'Queue' concept in opengl! Only implicit queue exists
        return default_queue;
    }

    void Device::WaitIdle() noexcept
    {
        loader.Finish();
    }

    Render::Swapchain* Device::GetSwapchain() const noexcept
    {
        return swapchain;
    }

    Render::Buffer* Device::CreateBuffer(const Render::BufferInfo& info,
                                         std::span<const std::uint32_t> desired_memory_type_indices)
    {
        return new Buffer(this, info, desired_memory_type_indices);
    }

    //tries to allocate buffers in amanner they will lay out in the same inner memory handle
    //1. searches for already allocated memory handle and tries to place them into memory
    //2. if no memory handle found tries to allocate memory handle
    //3. if memory allcoation fails then calls plain CreateBuffer for each buffer
    void Device::CreateBuffersBatch(std::span<const Render::BufferInfo> infos,
                                    std::span<const std::uint32_t> desired_memory_type_indices,
                                    std::span<Render::Buffer*> buffers)
    {
        std::size_t i = 0;
        try
        {
            for(; i < infos.size(); i++)
                buffers[i] = CreateBuffer(infos[i], desired_memory_type_indices);
        }
        catch(...)
        {
            for(std::size_t j = 0; j < i; j++)
                delete buffers[j];

            throw;
        }
    }

    Render::CommandPool* Device::CreateCommandPool(const Render::CommandPoolInfo& info)
    {
        return new CommandPool(this, info);
    }

    Render::DescriptorPool* Device::CreateDescriptorPool(const Render::DescriptorPoolInfo& info)
    {
        return new DescriptorPool(this, info);
    }

    Render::DescriptorSetLayout*
    Device::CreateDescriptorSetLayout(const Render::DescriptorSetLayoutInfo& info)
    {
        return new DescriptorSetLayout(this, info);
    }

    Render::Fence* Device::CreateFence()
    {
        return new Fence(this);
    }

    Render::Framebuffer* Device::CreateFramebuffer(const Render::FramebufferInfo& info)
    {
        return new Framebuffer(this, info);
    }

    Render::Image* Device::CreateImage(const Render::ImageInfo& info)
    {
        return new Image(this, info);
    }

    void Device::CreateImagesBatch(std::span<const Render::ImageInfo> infos,
                                   std::span<Render::Image*> images)
    {
        std::size_t i = 0;
        try
        {
            for(; i < infos.size(); i++)
                images[i] = CreateImage(infos[i]);
        }
        catch(...)
        {
            for(std::size_t j = 0; j < i; j++)
                delete images[j];

            throw;
        }
    }

    Render::ImageView* Device::CreateImageView(const Render::ImageViewInfo& info)
    {
        return new ImageView(this, info);
    }

    Render::Pipeline* Device::CreatePipeline(const Render::GraphicsPipelineInfo& info)
    {
        return new Pipeline(this, info);
    }

    Render::Pipeline* Device::CreatePipeline(const Render::ComputePipelineInfo& info)
    {
        return new Pipeline(this, info);
    }

    Render::RenderPass* Device::CreateRenderPass(const Render::RenderPassInfo& info)
    {
        return new RenderPass(this, info);
    }

    Render::Sampler* Device::CreateSampler(const Render::SamplerInfo& info)
    {
        return new Sampler(this, info);
    }

    Render::Semaphore* Device::CreateSemaphore()
    {
        return new Semaphore(this);
    }

    Render::Shader* Device::CreateShader(const Render::ShaderInfo& info)
    {
        return new Shader(this, info);
    }

    Render::PhysicalDevice* Device::GetParent() const noexcept
    {
        return parent;
    }

    const GladGLContext& Device::GetLoader() const noexcept
    {
        return loader;
    }

    void Device::SetDebugMessenger(const Render::DebugMessengerInfo& info)
    {
        surface->MakeCurrent();

        auto filter_types = DebugMessengerTypeFlagsToNativeInverted(info.types);
        auto filter_severities = DebugMessengerSeverityFlagsToNativeInverted(info.severities);

        loader.DebugMessageControl(GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   0,
                                   nullptr,
                                   GL_TRUE); //drop all filters

        for(std::size_t i = 0; i < filter_types.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       filter_types.data[i],
                                       GL_DONT_CARE,
                                       0,
                                       nullptr,
                                       GL_FALSE);

        for(std::size_t i = 0; i < filter_severities.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       GL_DONT_CARE,
                                       filter_severities.data[i],
                                       0,
                                       nullptr,
                                       GL_FALSE);

        loader.DebugMessageCallback(debug_messenger_callback, &info.callback);
    }
};