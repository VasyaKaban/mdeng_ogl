#include "Device.h"
#include "../PhysicalDevice/PhysicalDevice.h"
#include "../Surface/Surface.h"

namespace OpenGL
{
    Device::Device(PhysicalDevice* _parent, const Render::DeviceInfo& info)
        : parent(_parent),
          enabled_features(info.enabled_features)
    {
        int glad_ver = gladLoadGLContext(&loader, parent->GetProcAddressResolver());
        if(glad_ver == 0)
            throw std::runtime_error("Failed to load GLAD");

        Surface* impl_surface = static_cast<Surface*>(info.swapchain_info.surface);

        const SurfaceConnectInfo connection_info = {
            .physical_device = parent,
            .config_index = info.swapchain_info.surface_config_index,
            .validation_layer_enabled = info.enabled_features.validation_layer,
            .robust_buffer_access_enabled = info.enabled_features.robust_buffer_access,
        };
        impl_surface->Connect(connection_info);
    }

#error HERE!!!

    Device::~Device();

    Render::Queue* Device::GetQueue(const Render::QueueInfo& info);

    void Device::WaitIdle() noexcept;

    bool Device::AcquireNextSwapchainImage(
        Render::Semaphore*
            signal_semaphore); //OGL -> noop; false -> should recreate window/swapchain

    std::span<Render::Image*> Device::GetSwapchainImages();
    Render::Framebuffer* Device::CreateFramebufferFromSwapchainImage(std::uint32_t index);

    bool Device::ReleaseSwapchainImage(
        const Render::PresentInfo&
            info); //SDL_SwapWindow(); false -> should recreate window/swapchain

    void Device::RecreateSwapchain(const Render::SwapchainInfo& info);

    void Device::SetDebugMessenger(const Render::DebugMessengerInfo& info);

    Render::Buffer*
    Device::CreateBuffer(const Render::BufferInfo& info,
                         std::span<const std::uint32_t> desired_memory_type_indices);

    //tries to allocate buffers in amanner they will lay out in the same inner memory handle
    //1. searches for already allocated memory handle and tries to place them into memory
    //2. if no memory handle found tries to allocate memory handle
    //3. if memory allcoation fails then calls plain CreateBuffer for each buffer
    void Device::CreateBuffersBatch(std::span<const Render::BufferInfo> infos,
                                    std::span<const std::uint32_t> desired_memory_type_indices,
                                    std::span<Render::Buffer*> buffers);

    Render::CommandPool* Device::CreateCommandPool(const Render::CommandPoolInfo& info);
    Render::DescriptorPool* Device::CreateDescriptorPool(const Render::DescriptorPoolInfo& info);
    Render::DescriptorSetLayout*
    Device::CreateDescriptorSetLayout(const Render::DescriptorSetLayoutInfo& info);
    Render::Fence* Device::CreateFence();
    Render::Framebuffer* Device::CreateFramebuffer(const Render::FramebufferInfo& info);
    Render::Image* Device::CreateImage(const Render::ImageInfo& info);

    void Device::CreateImagesBatch(std::span<const Render::ImageInfo> infos,
                                   std::span<Render::Image*> images);

    Render::ImageView* Device::CreateImageView(const Render::ImageViewInfo& info);
    Render::Pipeline* Device::CreatePipeline(const Render::GraphicsPipelineInfo& info);
    Render::Pipeline* Device::CreatePipeline(const Render::ComputePipelineInfo& info);
    Render::RenderPass* Device::CreateRenderPass(const Render::RenderPassInfo& info);
    Render::Sampler* Device::CreateSampler(const Render::SamplerInfo& info);
    Render::Semaphore* Device::CreateSemaphore();
    Render::Shader* Device::CreateShader(const Render::ShaderInfo& info)

    //PhysicalDevice* parent;
    //GladGLContext loader;
    //Render::PhysicalDeviceFeatures enabled_features;

};