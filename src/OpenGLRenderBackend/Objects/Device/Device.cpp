#include "Device.h"
#include <format>
#include <iostream>
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

namespace OpenGL
{
    static void GLAPIENTRY debug_messenger_callback(GLenum source,
                                                    GLenum type,
                                                    GLuint id,
                                                    GLenum severity,
                                                    GLsizei length,
                                                    const GLchar* message,
                                                    const void* userParam)
    {
        (*reinterpret_cast<const std::function<Render::DebugMessengerCallback>*>(userParam))(
            NativeDebugMessengerSeverityFlagBitToSpec(severity),
            NativeDebugMessengerTypeFlagBitToSpec(type),
            id,
            {reinterpret_cast<const char*>(message), static_cast<std::size_t>(length)});
    }

    static void default_debug_messenger(Render::DebugMessengerSeverityFlagBits severity,
                                        Render::DebugMessengerTypeFlags types,
                                        std::int64_t id,
                                        std::string_view message)
    {
        std::string_view severity_string = "Unknown";
        switch(severity)
        {
            case Render::DebugMessengerSeverityFlagBits::Error:
                severity_string = "Error";
                break;
            case Render::DebugMessengerSeverityFlagBits::Warning:
                severity_string = "Warning";
                break;
            case Render::DebugMessengerSeverityFlagBits::Info:
                severity_string = "Info";
                break;
            case Render::DebugMessengerSeverityFlagBits::Verbose:
                severity_string = "Verbose";
                break;
        }

        static std::string types_string;
        constexpr std::pair<Render::DebugMessengerTypeFlagBits, std::string_view>
            TYPE_NAMES_MAPPING[] = {
                {Render::DebugMessengerTypeFlagBits::General, "General"},
                {Render::DebugMessengerTypeFlagBits::Validation, "Validation"},
                {Render::DebugMessengerTypeFlagBits::Performance, "Performance"}};

        for(const auto& [type, name]: TYPE_NAMES_MAPPING)
        {
            if(types_string.empty())
                types_string += name;
            else
                types_string += std::format(" | {}", name);
        }

        std::format_to(std::ostream_iterator<char>(std::cout, "\n"),
                       "[Severity: {}][Types: {}] ID: {}; {}",
                       severity_string,
                       types_string.empty() ? "Unknown" : types_string,
                       id,
                       message);

        types_string.clear();
    }

    Device::Device(PhysicalDevice* _parent, const Render::DeviceInfo& info)
        : parent(_parent),
          surface(static_cast<Surface*>(info.surface)),
          enabled_features(info.enabled_features),
          default_queue(new Queue(this))
    {
        int glad_ver = gladLoadGLContext(&loader, parent->GetProcAddressResolver());
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        Surface* impl_surface = static_cast<Surface*>(info.surface);

        const SurfaceConnectInfo connection_info = {
            .physical_device = parent,
            .config_index = info.swapchain_info.surface_config_index,
            .validation_layer_enabled = info.enabled_features.validation_layer,
            .robust_buffer_access_enabled = info.enabled_features.robust_buffer_access,
        };
        impl_surface->Connect(connection_info);
        impl_surface->SetSwapInterval(info.swapchain_info.present_mode);

        loader.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //use only seamless cubemaps
        loader.ClipControl(/*GL_UPPER_LEFT*/ GL_LOWER_LEFT,
                           GL_ZERO_TO_ONE); //make viewport like in D3D or VK(only depth)
        loader.Enable(GL_PROGRAM_POINT_SIZE); //activate GLSL gl_PointSize
        loader.ProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
        loader.Enable(GL_FRAMEBUFFER_SRGB);
        loader.Enable(GL_LINE_SMOOTH);

        if(parent->GetProperties().features.debug_messenger)
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

            if(parent->GetProperties().features.validation_layer)
            {
                debug_callback = default_debug_messenger;
                loader.DebugMessageCallback(debug_messenger_callback, &debug_callback);
            }
        }
    }

    Device::~Device()
    {
        WaitIdle();

        delete default_queue;
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

    std::optional<std::uint32_t> Device::AcquireNextSwapchainImage(
        Render::Semaphore*
            signal_semaphore) //OGL -> noop; false -> should recreate window/swapchain
    {
        static_cast<Semaphore*>(signal_semaphore)->Set();

        return surface->GetImageIndex();
    }

    std::span<Render::Image*> Device::GetSwapchainImages()
    {
        return surface->GetImages();
    }

    Render::Framebuffer* Device::CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                                                     Render::RenderPass* renderpass)
    {
        return new Framebuffer(this);
    }

    bool Device::ReleaseSwapchainImage(const Render::PresentInfo& info)
    {
        for(auto& sem: info.wait_semaphores)
            static_cast<Semaphore*>(sem)->Wait();

        surface->SwapWindow();

        return true;
    }

    void Device::RecreateSwapchain(const Render::SwapchainInfo& info)
    {
        surface->SetSwapInterval(info.present_mode);
    }

    void Device::SetDebugMessenger(const Render::DebugMessengerInfo& info)
    {
        if(!parent->GetProperties().features.debug_messenger)
            throw std::runtime_error("Context is not in debug mode");

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

        debug_callback = info.callback;

        loader.DebugMessageCallback(debug_messenger_callback, &debug_callback);
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
};