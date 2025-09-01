#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Pipeline.h"
#include "GraphicsPipelineState.h"

namespace OpenGL
{
    class Pipeline : public Render::Pipeline, hrs::non_copyable, hrs::non_movable
    {
    public:
        Pipeline(Context* _parent, const Render::GraphicsPipelineInfo& info);
        Pipeline(Context* _parent, const Render::ComputePipelineInfo& info);

        virtual ~Pipeline() override;

        virtual void Bind(const Render::CommandBuffer* cmd) override;

        virtual void BindVertexBuffer(const Render::CommandBuffer* cmd,
                                      const Render::Buffer* buffer,
                                      std::uint32_t binding,
                                      std::int64_t offset) override;
        virtual void BindIndexBuffer(const Render::CommandBuffer* cmd,
                                     const Render::Buffer* buffer,
                                     Render::IndexType type,
                                     std::uintptr_t offset) override;

        virtual void BindUniformBuffer(const Render::CommandBuffer* cmd,
                                       const Render::Buffer* buffer,
                                       const Render::BufferBindDesc& bind_desc) noexcept override;
        virtual void
        BindShaderStorageBuffer(const Render::CommandBuffer* cmd,
                                const Render::Buffer* buffer,
                                const Render::BufferBindDesc& bind_desc) noexcept override;

        virtual void BindImageView(const Render::CommandBuffer* cmd,
                                   const Render::ImageView* view,
                                   std::uint32_t index) noexcept override;

        virtual void BindSampler(const Render::CommandBuffer* cmd,
                                 const Render::Sampler* sampler,
                                 std::uint32_t index) noexcept override;

        virtual void Draw(const Render::CommandBuffer* cmd,
                          std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex,
                          std::uint32_t first_instance) override;

        virtual void DrawIndexed(const Render::CommandBuffer* cmd,
                                 std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset,
                                 std::uint32_t first_instance) override;

        virtual void SetViewport(const Render::CommandBuffer* cmd,
                                 std::uint32_t first_viewport,
                                 std::span<const Render::Viewport> viewports) override;
        virtual void SetScissor(const Render::CommandBuffer* cmd,
                                std::uint32_t first_scissor,
                                std::span<const Render::Rect2D> scissors) override;

        virtual void SetUniform(const Render::CommandBuffer* cmd,
                                const Render::UniformDesc& desc,
                                std::span<const std::byte> data) override;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;

        GraphicsPipelineState* graphics_state;
    };
};