#pragma once

#include <memory>
#include "Render.h"

#include "Objects/Buffer.h"
#include "Objects/CommandPool.h"
#include "Objects/Fence.h"
#include "Objects/Framebuffer.h"
#include "Objects/Image.h"
#include "Objects/ImageView.h"
#include "Objects/Pipeline.h"
#include "Objects/RenderPass.h"
#include "Objects/Sampler.h"
#include "Objects/Semaphore.h"
#include "Objects/Shader.h"

class RenderBackend;

namespace Render
{
    class Context
    {
    public:
        virtual ~Context() {};

        virtual ContextProperties GetProperties() const = 0;

        virtual Queue* GetQueue(QueueSpecializationFlags spec) = 0;

        virtual void WaitIdle() noexcept = 0;

        virtual void AcquireNextSwapchainImage(Semaphore* signal_semaphore) = 0; //OGL -> noop
        virtual Framebuffer* GetCurrentDefaultFramebuffer() noexcept = 0;
        virtual void ReleaseSwapchainImage(const PresentInfo& info) = 0; //SDL_SwapWindow();

        virtual void SetDebugMessenger(const DebugMessengerInfo& info) = 0;

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

        //CreateUnique
        std::unique_ptr<Buffer> CreateBufferUnique(const BufferInfo& info);
        std::unique_ptr<CommandPool> CreateCommandPoolUnique(const CommandPoolInfo& info);
        std::unique_ptr<Fence> CreateFenceUnique();
        std::unique_ptr<Framebuffer> CreateFramebufferUnique(const FramebufferInfo& info);
        std::unique_ptr<Image> CreateImageUnique(const ImageInfo& info);
        std::unique_ptr<ImageView> CreateImageViewUnique(const ImageViewInfo& info);
        std::unique_ptr<Pipeline> CreatePipelineUnique(const GraphicsPipelineInfo& info);
        std::unique_ptr<Pipeline> CreatePipelineUnique(const ComputePipelineInfo& info);
        std::unique_ptr<RenderPass> CreateRenderPassUnique(const RenderPassInfo& info);
        std::unique_ptr<Sampler> CreateSamplerUnique(const SamplerInfo& info);
        std::unique_ptr<Semaphore> CreateSemaphoreUnique();
        std::unique_ptr<Shader> CreateShaderUnique(const ShaderInfo& info);
    };
};