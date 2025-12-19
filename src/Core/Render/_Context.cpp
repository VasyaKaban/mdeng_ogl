#include "Context.h"
#include "Objects/Buffer.h"
#include "Objects/CommandPool.h"
#include "Objects/DescriptorPool.h"
#include "Objects/DescriptorSetLayout.h"
#include "Objects/Fence.h"
#include "Objects/Framebuffer.h"
#include "Objects/Image.h"
#include "Objects/ImageView.h"
#include "Objects/Pipeline.h"
#include "Objects/RenderPass.h"
#include "Objects/Sampler.h"
#include "Objects/Semaphore.h"
#include "Objects/Shader.h"

namespace Render
{
    std::unique_ptr<Buffer>
    Context::CreateBufferUnique(const BufferInfo& info,
                                std::span<const std::uint32_t> desired_memory_type_indices)
    {
        return std::unique_ptr<Buffer>(CreateBuffer(info, desired_memory_type_indices));
    }

    std::unique_ptr<CommandPool> Context::CreateCommandPoolUnique(const CommandPoolInfo& info)
    {
        return std::unique_ptr<CommandPool>(CreateCommandPool(info));
    }

    std::unique_ptr<DescriptorPool>
    Context::CreateDescriptorPoolUnique(const DescriptorPoolInfo& info)
    {
        return std::unique_ptr<DescriptorPool>(CreateDescriptorPool(info));
    }

    std::unique_ptr<DescriptorSetLayout>
    Context::CreateDescriptorSetLayoutUnique(const DescriptorSetLayoutInfo& info)
    {
        return std::unique_ptr<DescriptorSetLayout>(CreateDescriptorSetLayout(info));
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