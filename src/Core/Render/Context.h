#pragma once

#include "Render.h"

class RenderBackend;

namespace Render
{
    class Context
    {
    public:
        virtual ~Context() {};

        virtual Queue* GetQueue(QueueSpecialization spec) = 0;

        virtual void WaitIdle() noexcept = 0;

        virtual void AcquireNextSwapchainImage(Semaphore* signal_semaphore) = 0; //OGL -> noop
        virtual Framebuffer* GetCurrentDefaultFramebuffer() noexcept = 0;
        virtual void ReleaseSwapchainImage(PresentInfo& info) = 0; //SDL_SwapWindow();

        virtual RenderBackend* GetBackend() const noexcept = 0;

        virtual Buffer* CreateBuffer(const BufferInfo& info) = 0;
        virtual CommandPool* CreateCommandPool(const CommandPoolInfo& info) = 0;
        virtual Fence* CreateFence() = 0;
        virtual Framebuffer* CreateFramebuffer(const FramebufferInfo& info) = 0;
        virtual Image* CreateImage(const ImageInfo& info) = 0;
        virtual ImageView* CreateImageView(const ImageViewInfo& info) = 0;
        virtual Pipeline* CreatePipeline(const GraphicsPipelineInfo& info) = 0;
        virtual Pipeline* CreatePipeline(const ComputePipelineInfo& info) = 0;
        virtual RenderPass* CreateRenderPass(const RenderPassInfo& info) = 0;
        virtual Sampler* CreateSampler(const SamplerInfo& info) = 0;
        virtual Semaphore* CreateSemaphore() = 0;
        virtual Shader* CreateShader(const ShaderInfo& info) = 0;
    };
};