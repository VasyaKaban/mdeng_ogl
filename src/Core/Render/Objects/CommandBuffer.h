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

        virtual void Reset(const CommandBufferResetInfo& reset_info) = 0;
        virtual void Begin(const CommandBufferBeginInfo& begin_info) = 0;
        virtual void End() = 0;

        //Buffer
        virtual void
        CopyBufferToBuffer(Buffer* src, Buffer* dst, std::span<const BufferCopyRegion> regions) = 0;
        virtual void CopyBufferToImage(Buffer* src,
                                       Image* dst,
                                       std::span<const BufferImageCopyRegion> regions) = 0;

        //Image
        virtual void CopyImageToBuffer(Image* src,
                                       Buffer* dst,
                                       std::span<const BufferImageCopyRegion> regions) = 0;

        //Pipeline
        virtual void BindPipeline(Pipeline* pipeline) = 0;

        virtual void BindVertexBuffer(std::uint32_t first_binding,
                                      std::uint32_t binding_count,
                                      Buffer** buffers,
                                      std::int64_t* offsets) = 0;
        virtual void BindIndexBuffer(Buffer* buffer, IndexType type, std::uintptr_t offset) = 0;

        virtual void Draw(std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex,
                          std::uint32_t first_instance) = 0;

        virtual void DrawIndexed(std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset,
                                 std::uint32_t first_instance) = 0;

        virtual void DrawIndirect(Buffer* buffer,
                                  std::uint64_t offset,
                                  std::uint32_t draw_count,
                                  std::uint32_t stride) = 0;

        virtual void DrawIndexedIndirect(Buffer* buffer,
                                         std::uint64_t offset,
                                         std::uint32_t draw_count,
                                         std::uint32_t stride) = 0;

        virtual void
        Dispatch(std::uint32_t x_groups, std::uint32_t y_groups, std::uint32_t z_groups) = 0;

        virtual void DispatchIndirect(Buffer* buffer, std::uint64_t offset) = 0;

        virtual void BindDescriptorSets(std::uint32_t first_set,
                                        std::span<const DescriptorSet*> sets) = 0;

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