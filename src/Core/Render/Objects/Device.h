#pragma once

#include <optional>
#include "../Render.h"

namespace Render
{
    class Device
    {
    public:
        virtual ~Device() {};

        virtual Queue* GetQueue(const QueueInfo& info) = 0;

        virtual void WaitIdle() noexcept = 0;

        virtual std::optional<std::uint32_t> AcquireNextSwapchainImage(
            Semaphore*
                signal_semaphore) = 0; //OGL -> noop; false -> should recreate window/swapchain

        virtual std::span<Image*> GetSwapchainImages() = 0;
        virtual Framebuffer* CreateFramebufferFromSwapchainImage(std::uint32_t index,
                                                                 RenderPass* renderpass) = 0;

        virtual bool ReleaseSwapchainImage(
            const PresentInfo&
                info) = 0; //SDL_SwapWindow(); false -> should recreate window/swapchain

        virtual void RecreateSwapchain(const SwapchainInfo& info) = 0;

        virtual void SetDebugMessenger(const DebugMessengerInfo& info) = 0;

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

        virtual PhysicalDevice* GetParent() const noexcept = 0;
    };
};