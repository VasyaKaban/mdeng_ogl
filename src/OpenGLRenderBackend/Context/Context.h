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
        Context(OpenGLBackend* _parent, const Render::ContextSelector& selector);
        virtual ~Context() override;

        virtual Render::ContextProperties GetProperties() const override;

        virtual Render::Queue* GetQueue(Render::QueueSpecializationFlags spec) override;

        virtual void WaitIdle() noexcept override;

        virtual void
        AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore) override; //OGL -> noop
        virtual Render::Framebuffer* GetCurrentDefaultFramebuffer() noexcept override;
        virtual void BeginDefaultFramebufferImageUsage() override; //OGL -> noop
        virtual void EndDefaultFramebufferImageUsage() override; //OGL -> noop
        virtual void
        ReleaseSwapchainImage(const Render::PresentInfo& info) override; //SDL_SwapWindow();

        virtual void SetDebugMessenger(const Render::DebugMessengerInfo& info) override;

        virtual RenderBackend* GetBackend() const noexcept override;

        virtual Render::Buffer* CreateBuffer(const Render::BufferInfo& info) override;
        virtual Render::CommandPool*
        CreateCommandPool(const Render::CommandPoolInfo& info) override;
        virtual Render::Fence* CreateFence() override;
        virtual Render::Framebuffer*
        CreateFramebuffer(const Render::FramebufferInfo& info) override;
        virtual Render::Image* CreateImage(const Render::ImageInfo& info) override;
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
        OpenGLBackend* parent;
        GladGLContext loader;

        Framebuffer default_framebuffer;
        Queue default_queue;

        Render::ContextProperties properties;
        std::vector<std::string_view> extensions;

        std::function<Render::DebugMessengerCallback> debug_callback;
    };
};