#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "GraphicsPipelineState.h"
#include "Core/Render/Objects/Pipeline.h"

namespace OpenGL
{
    class Pipeline : public Render::Pipeline, hrs::non_copyable, hrs::non_movable
    {
    public:
        Pipeline(Context* _parent, const Render::GraphicsPipelineInfo& info);
        Pipeline(Context* _parent, const Render::ComputePipelineInfo& info);

        virtual ~Pipeline() override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;

        void Bind(CommandBuffer& cmd);

        void BindVertexBuffer(CommandBuffer& cmd,
                              Buffer& buffer,
                              std::uint32_t binding,
                              std::int64_t offset);

        void BindIndexBuffer(CommandBuffer& cmd,
                             Buffer& buffer,
                             Render::IndexType type,
                             std::uintptr_t offset);

        void Draw(CommandBuffer& cmd,
                  std::uint32_t vertex_count,
                  std::uint32_t instance_count,
                  std::uint32_t first_vertex,
                  std::uint32_t first_instance);

        void DrawIndexed(CommandBuffer& cmd,
                         std::uint32_t index_count,
                         std::uint32_t instance_count,
                         std::uint32_t first_index,
                         std::int32_t vertex_offset,
                         std::uint32_t first_instance);

        void DrawIndirect(CommandBuffer& cmd,
                          std::uint64_t offset,
                          std::uint32_t draw_count,
                          std::uint32_t stride);

        void DrawIndexedIndirect(CommandBuffer& cmd,
                                 std::uint64_t offset,
                                 std::uint32_t draw_count,
                                 std::uint32_t stride);
    private:
        Context* parent;
        GLHandle handle;

        GraphicsPipelineState* state;
    };
};