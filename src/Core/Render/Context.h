#pragma once

#include <memory>
#include <optional>
#include "Render.h"

class RenderBackend;

namespace Render
{
    class Context
    {
    public:
        virtual ~Context() {};

        virtual const ContextProperties& GetProperties() const = 0;

        virtual const ContextSurfaceCapabilities& GetSurfaceCapabilities() const = 0;

        virtual std::optional<BufferFormatProperties>
        GetBufferFormatProperties(const BufferFormatInfo& info) const = 0;

        virtual std::optional<ImageFormatProperties>
        GetImageFormatProperties(const ImageFormatInfo& info) const = 0;

        virtual Queue* GetQueue(const QueueInfo& info) = 0;

        virtual void WaitIdle() noexcept = 0;

        virtual bool AcquireNextSwapchainImage(
            Semaphore*
                signal_semaphore) = 0; //OGL -> noop; false -> should recreate window/swapchain
        virtual Image*
        GetCurrentSwapchainImageHandle() = 0; //returns swapchain current image; OGL -> return impl-defined address that OpenGL impl will convert to inner calls; VK -> return already created image from swapchain
        virtual Framebuffer* GetCurrentDefaultFramebuffer() noexcept = 0;
        virtual bool ReleaseSwapchainImage(
            const PresentInfo&
                info) = 0; //SDL_SwapWindow(); false -> should recreate window/swapchain

        virtual void SetDebugMessenger(const DebugMessengerInfo& info) = 0;

        virtual Core::RenderBackend* GetBackend() const noexcept = 0;

        virtual Buffer*
        CreateBuffer(const BufferInfo& info,
                     std::span<const std::uint32_t> desired_memory_type_indices) = 0;

        //tries to allocate buffers in amanner they will lay out in the same inner memory handle
        //1. searches for already allocated memory handle and tries to place them into memory
        //2. if no memory handle found tries to allocate memory handle
        //3. if memory allcoation fails then calls plain CreateBuffer for each buffer
        virtual void CreateBuffersBatch(std::span<const BufferInfo> infos,
                                        std::span<const std::uint32_t> desired_memory_type_indices,
                                        std::span<Buffer*> buffers) = 0;

        virtual CommandPool* CreateCommandPool(const CommandPoolInfo& info) = 0;
        virtual DescriptorPool* CreateDescriptorPool(const DescriptorPoolInfo& info) = 0;
        virtual DescriptorSetLayout*
        CreateDescriptorSetLayout(const DescriptorSetLayoutInfo& info) = 0;
        virtual Fence* CreateFence() = 0;
        virtual Framebuffer* CreateFramebuffer(const FramebufferInfo& info) = 0;
        virtual Image* CreateImage(const ImageInfo& info) = 0;

        virtual void CreateImagesBatch(std::span<const ImageInfo> infos,
                                       std::span<Image*> images) = 0;

        virtual ImageView* CreateImageView(const ImageViewInfo& info) = 0;
        virtual Pipeline* CreatePipeline(const GraphicsPipelineInfo& info) = 0;
        virtual Pipeline* CreatePipeline(const ComputePipelineInfo& info) = 0;
        virtual RenderPass* CreateRenderPass(const RenderPassInfo& info) = 0;
        virtual Sampler* CreateSampler(const SamplerInfo& info) = 0;
        virtual Semaphore* CreateSemaphore() = 0;
        virtual Shader* CreateShader(const ShaderInfo& info) = 0;

        //CreateUnique
        std::unique_ptr<Buffer>
        CreateBufferUnique(const BufferInfo& info,
                           std::span<const std::uint32_t> desired_memory_type_indices);
        std::unique_ptr<CommandPool> CreateCommandPoolUnique(const CommandPoolInfo& info);
        std::unique_ptr<DescriptorPool> CreateDescriptorPoolUnique(const DescriptorPoolInfo& info);
        std::unique_ptr<DescriptorSetLayout>
        CreateDescriptorSetLayoutUnique(const DescriptorSetLayoutInfo& info);
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