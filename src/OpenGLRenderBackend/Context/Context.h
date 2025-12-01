#pragma once

#include <vector>
#include "hrs/non_creatable.hpp"
#include "../Render.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Render/Context.h"
#include "../Objects/Framebuffer/Framebuffer.h"
#include "../Objects/Queue/Queue.h"

namespace OpenGL
{
    class Context : public Render::Context, hrs::non_copyable, hrs::non_movable
    {
    public:
        Context(Core::OpenGLBackend* _parent);

        virtual ~Context() override;

        virtual const Render::ContextProperties& GetProperties() const override;

        virtual Render::Queue* GetQueue(const Render::QueueInfo& info) override;

        virtual void WaitIdle() noexcept override;

        virtual bool AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore)
            override; //OGL -> noop; false -> should recreate window/swapchain
        virtual Render::Image* GetCurrentSwapchainImageHandle()
            override; //returns swapchain current image; OGL -> return impl-defined address that OpenGL impl will convert to inner calls; VK -> return already created image from swapchain
        virtual Render::Framebuffer* GetCurrentDefaultFramebuffer() noexcept override;
        virtual bool
        ReleaseSwapchainImage(const Render::PresentInfo& info) override; //SDL_SwapWindow();

        virtual void SetDebugMessenger(const Render::DebugMessengerInfo& info) override;

        virtual Core::RenderBackend* GetBackend() const noexcept override;

        virtual Render::Buffer*
        CreateBuffer(const Render::BufferInfo& info,
                     std::span<const std::uint32_t> desired_memory_type_indices) override;

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

        void MakeCurrent() noexcept;

        const GladGLContext& GetLoader() const noexcept;
    private:
        Core::OpenGLBackend* parent;
        GladGLContext loader;

        Framebuffer default_framebuffer;
        Queue default_queue;

        Render::ContextProperties properties;

        std::function<Render::DebugMessengerCallback> debug_callback;
    };
};