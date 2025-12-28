#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/CommandBuffer.h"

namespace OpenGL
{
    //imitation only!
    class CommandBuffer : public Render::CommandBuffer, hrs::non_copyable, hrs::non_movable
    {
    public:
        CommandBuffer(Device* _parent, CommandPool* _pool) noexcept;
        virtual ~CommandBuffer() override;

        virtual void Reset(const Render::CommandBufferResetInfo& reset_info) override;
        virtual void Begin(const Render::CommandBufferBeginInfo& begin_info) override;
        virtual void End() override;

        //Buffer
        virtual void
        CopyBufferToBuffer(Render::Buffer* src,
                           Render::Buffer* dst,
                           std::span<const Render::BufferCopyRegion> regions) noexcept override;
        virtual void
        CopyBufferToImage(Render::Buffer* src,
                          Render::Image* dst,
                          std::span<const Render::BufferImageCopyRegion> regions) override;

        //Image
        virtual void
        CopyImageToBuffer(Render::Image* src,
                          Render::Buffer* dst,
                          std::span<const Render::BufferImageCopyRegion> regions) override;

        //Pipeline
        virtual void BindPipeline(Render::Pipeline* pipeline) override;

        virtual void BindVertexBuffer(std::uint32_t first_binding,
                                      std::uint32_t binding_count,
                                      Render::Buffer** buffers,
                                      std::int64_t* offsets) override;
        virtual void BindIndexBuffer(Render::Buffer* buffer,
                                     Render::IndexType type,
                                     std::uintptr_t offset) override;

        virtual void Draw(std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex,
                          std::uint32_t first_instance) override;

        virtual void DrawIndexed(std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset,
                                 std::uint32_t first_instance) override;

        virtual void DrawIndirect(Render::Buffer* buffer,
                                  std::uint64_t offset,
                                  std::uint32_t draw_count,
                                  std::uint32_t stride) override;

        virtual void DrawIndexedIndirect(Render::Buffer* buffer,
                                         std::uint64_t offset,
                                         std::uint32_t draw_count,
                                         std::uint32_t stride) override;

        virtual void
        Dispatch(std::uint32_t x_groups, std::uint32_t y_groups, std::uint32_t z_groups) override;

        virtual void DispatchIndirect(Render::Buffer* buffer, std::uint64_t offset) override;

        virtual void BindDescriptorSets(std::uint32_t first_set,
                                        std::span<const Render::DescriptorSet*> sets) override;

        //Dynamic state
        virtual void SetViewport(std::uint32_t first_viewport,
                                 std::span<const Render::Viewport> viewports) override;
        virtual void SetScissors(std::uint32_t first_scissor,
                                 std::span<const Render::Rect2D> scissors) override;

        virtual void SetUniforms(Render::ShaderStageFlags stages,
                                 std::uint32_t offset,
                                 std::span<const std::byte> data,
                                 std::span<const Render::UniformDesc> uniform_descs) override;

        //Renderpass
        virtual void BeginRenderPass(Render::RenderPass* renderpass,
                                     const Render::RenderPassBeginInfo& info) override;
        virtual void EndRenderPass() override;

        //Common
        virtual void SetPipelineBarrier(const Render::PipelineBarrier& barrier) override;

        virtual Render::Device* GetParent() const noexcept override;
    private:
        Device* parent;
        CommandPool* pool;

        Pipeline* bound_pipeline;
    };
};