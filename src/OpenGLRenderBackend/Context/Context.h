#pragma once

#include "hrs/non_creatable.hpp"
#include "../Render.h"
#include "Core/Window/OpenGL/OpenGLBackend.h"
#include "Core/Render/Context.h"
#include "../Objects/Framebuffer/Framebuffer.h"

class GraphicWindow;

namespace OpenGL
{
    class Context : public Render::Context, hrs::non_copyable, hrs::non_movable
    {
    public:
        Context(OpenGLBackend* _parent);
        virtual ~Context() override;

        virtual Render::Queue* GetQueue(Render::QueueSpecialization spec) override;

        virtual void WaitIdle() noexcept override;

        virtual void
        AcquireNextSwapchainImage(Render::Semaphore* signal_semaphore) override; //OGL -> noop
        virtual Render::Framebuffer* GetCurrentDefaultFramebuffer() noexcept override;
        virtual void ReleaseSwapchainImage(Render::PresentInfo& info) override; //SDL_SwapWindow();

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
        GladGLContext loader; //Split into CommandBufferLoader and ImmediateLoader

        Framebuffer default_framebuffer;
    };
};