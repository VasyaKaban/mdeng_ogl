#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Device.h"
#include "../../Render.h"

namespace OpenGL
{
    class Device : public Render::Device, hrs::non_copyable, hrs::non_movable
    {
    public:
        Device(PhysicalDevice* _parent, const Render::DeviceInfo& info);
        virtual ~Device() override;

        virtual Render::Queue* GetQueue(const Render::QueueInfo& info) override;

        virtual void WaitIdle() noexcept override;

        virtual std::optional<std::uint32_t>
        AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore)
            override; //OGL -> noop; false -> should recreate window/swapchain

        virtual std::span<Render::Image*> GetSwapchainImages() override;
        virtual Render::Framebuffer*
        CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                            Render::RenderPass* renderpass) override;

        virtual bool ReleaseSwapchainImage(const Render::PresentInfo& info)
            override; //SDL_SwapWindow(); false -> should recreate window/swapchain

        virtual void RecreateSwapchain(const Render::SwapchainInfo& info) override;

        virtual void SetDebugMessenger(const Render::DebugMessengerInfo& info) override;

        virtual Render::Buffer*
        CreateBuffer(const Render::BufferInfo& info,
                     std::span<const std::uint32_t> desired_memory_type_indices) override;

        //tries to allocate buffers in amanner they will lay out in the same inner memory handle
        //1. searches for already allocated memory handle and tries to place them into memory
        //2. if no memory handle found tries to allocate memory handle
        //3. if memory allcoation fails then calls plain CreateBuffer for each buffer
        virtual void CreateBuffersBatch(std::span<const Render::BufferInfo> infos,
                                        std::span<const std::uint32_t> desired_memory_type_indices,
                                        std::span<Render::Buffer*> buffers) override;

        virtual Render::CommandPool*
        CreateCommandPool(const Render::CommandPoolInfo& info) override;
        virtual Render::DescriptorPool*
        CreateDescriptorPool(const Render::DescriptorPoolInfo& info) override;
        virtual Render::DescriptorSetLayout*
        CreateDescriptorSetLayout(const Render::DescriptorSetLayoutInfo& info) override;
        virtual Render::Fence* CreateFence() override;
        virtual Render::Framebuffer*
        CreateFramebuffer(const Render::FramebufferInfo& info) override;
        virtual Render::Image* CreateImage(const Render::ImageInfo& info) override;

        virtual void CreateImagesBatch(std::span<const Render::ImageInfo> infos,
                                       std::span<Render::Image*> images) override;

        virtual Render::ImageView* CreateImageView(const Render::ImageViewInfo& info) override;
        virtual Render::Pipeline* CreatePipeline(const Render::GraphicsPipelineInfo& info) override;
        virtual Render::Pipeline* CreatePipeline(const Render::ComputePipelineInfo& info) override;
        virtual Render::RenderPass* CreateRenderPass(const Render::RenderPassInfo& info) override;
        virtual Render::Sampler* CreateSampler(const Render::SamplerInfo& info) override;
        virtual Render::Semaphore* CreateSemaphore() override;
        virtual Render::Shader* CreateShader(const Render::ShaderInfo& info) override;

        virtual Render::PhysicalDevice* GetParent() const noexcept override;

        const GladGLContext& GetLoader() const noexcept;
    private:
        PhysicalDevice* parent;
        Surface* surface;
        GladGLContext loader;
        Render::PhysicalDeviceFeatures enabled_features;

        Queue* default_queue;

        std::function<Render::DebugMessengerCallback> debug_callback;
    };
};