#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class CommandBuffer : public Object
    {
    public:
        virtual ~CommandBuffer()
        {}

        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void End() = 0;

        //all image copy commands rely on general image layout. So, after creating an image we should transfer image layout to general and then transfer, render, etc...
        //Buffer
        virtual void
        CopyBufferToBuffer(Buffer* src, Buffer* dst, std::span<const BufferCopyRegion> regions) = 0;
        virtual void CopyBufferToImage(Buffer* src,
                                       Image* dst,
                                       std::span<const BufferImageCopyRegion> regions) = 0;

        //Image
        virtual void TransferToGeneralImageLayout(Image* src) = 0;

        virtual void CopyImageToBuffer(Image* src,
                                       Buffer* dst,
                                       std::span<const BufferImageCopyRegion> regions) = 0;

        //Pipeline
        virtual void Bind(Pipeline* pipeline) = 0;

        virtual void
        BindVertexBuffer(Buffer* buffer, std::uint32_t binding, std::int64_t offset) = 0;
        virtual void BindIndexBuffer(Buffer* buffer, IndexType type, std::uintptr_t offset) = 0;

        virtual void Draw(std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex) = 0;

        virtual void DrawIndexed(std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset) = 0;

        virtual void
        Dispatch(std::uint32_t x_groups, std::uint32_t y_groups, std::uint32_t z_groups) = 0;

        virtual void UpdateDescriptorSet(
            DescriptorSet* set,
            std::span<const UpdateDescriptorDesc> descs) = 0; //Update descriptors + bind

        //Dynamic state
        virtual void SetViewport(std::uint32_t first_viewport,
                                 std::span<const Viewport> viewports) = 0;
        virtual void SetScissors(std::uint32_t first_scissor, std::span<const Rect2D> scissors) = 0;

        virtual void SetUniform(const UniformDesc& desc, std::span<const std::byte> data) = 0;

        //Renderpass
        virtual void BeginRenderPass(RenderPass* renderpass, const RenderPassBeginInfo& info) = 0;
        virtual void EndRenderPass() = 0;

        //Common
        virtual void SetPipelineBarrier(const PipelineBarrier& barrier) = 0;
    };
};