#include "Context.h"

namespace Render
{
    std::unique_ptr<Buffer> Context::CreateBufferUnique(const BufferInfo& info)
    {
        return std::unique_ptr<Buffer>(CreateBuffer(info));
    }

    std::unique_ptr<CommandPool> Context::CreateCommandPoolUnique(const CommandPoolInfo& info)
    {
        return std::unique_ptr<CommandPool>(CreateCommandPool(info));
    }

    std::unique_ptr<Fence> Context::CreateFenceUnique()
    {
        return std::unique_ptr<Fence>(CreateFence());
    }

    std::unique_ptr<Framebuffer> Context::CreateFramebufferUnique(const FramebufferInfo& info)
    {
        return std::unique_ptr<Framebuffer>(CreateFramebuffer(info));
    }

    std::unique_ptr<Image> Context::CreateImageUnique(const ImageInfo& info)
    {
        return std::unique_ptr<Image>(CreateImage(info));
    }

    std::unique_ptr<ImageView> Context::CreateImageViewUnique(const ImageViewInfo& info)
    {
        return std::unique_ptr<ImageView>(CreateImageView(info));
    }

    std::unique_ptr<Pipeline> Context::CreatePipelineUnique(const GraphicsPipelineInfo& info)
    {
        return std::unique_ptr<Pipeline>(CreatePipeline(info));
    }

    std::unique_ptr<Pipeline> Context::CreatePipelineUnique(const ComputePipelineInfo& info)
    {
        return std::unique_ptr<Pipeline>(CreatePipeline(info));
    }

    std::unique_ptr<RenderPass> Context::CreateRenderPassUnique(const RenderPassInfo& info)
    {
        return std::unique_ptr<RenderPass>(CreateRenderPass(info));
    }

    std::unique_ptr<Sampler> Context::CreateSamplerUnique(const SamplerInfo& info)
    {
        return std::unique_ptr<Sampler>(CreateSampler(info));
    }

    std::unique_ptr<Semaphore> Context::CreateSemaphoreUnique()
    {
        return std::unique_ptr<Semaphore>(CreateSemaphore());
    }

    std::unique_ptr<Shader> Context::CreateShaderUnique(const ShaderInfo& info)
    {
        return std::unique_ptr<Shader>(CreateShader(info));
    }

};