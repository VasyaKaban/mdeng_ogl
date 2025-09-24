#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Pipeline : public Object
    {
    public:
        virtual ~Pipeline() {};

        virtual void Bind(const CommandBuffer* cmd) = 0;

        virtual void BindVertexBuffer(const CommandBuffer* cmd,
                                      const Buffer* buffer,
                                      std::uint32_t binding,
                                      std::int64_t offset) = 0;
        virtual void BindIndexBuffer(const CommandBuffer* cmd,
                                     const Buffer* buffer,
                                     IndexType type,
                                     std::uintptr_t offset) = 0;

        virtual void BindUniformBuffer(const CommandBuffer* cmd,
                                       const Buffer* buffer,
                                       const BufferBindDesc& bind_desc) noexcept = 0;
        virtual void BindShaderStorageBuffer(const CommandBuffer* cmd,
                                             const Buffer* buffer,
                                             const BufferBindDesc& bind_desc) noexcept = 0;

        virtual void BindImageView(const CommandBuffer* cmd,
                                   const ImageView* view,
                                   std::uint32_t index) noexcept = 0;

        virtual void BindSampler(const CommandBuffer* cmd,
                                 const Sampler* sampler,
                                 std::uint32_t index) noexcept = 0;

        virtual void Draw(const CommandBuffer* cmd,
                          std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex) = 0;

        virtual void DrawIndexed(const CommandBuffer* cmd,
                                 std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset) = 0;

        //Dynamic state
        virtual void SetViewport(const CommandBuffer* cmd,
                                 std::uint32_t first_viewport,
                                 std::span<const Viewport> viewports) = 0;
        virtual void SetScissor(const CommandBuffer* cmd,
                                std::uint32_t first_scissor,
                                std::span<const Rect2D> scissors) = 0;

        virtual void SetUniform(const CommandBuffer* cmd,
                                const UniformDesc& desc,
                                std::span<const std::byte> data) = 0;

        virtual Context* GetContext() const noexcept = 0;
    };
};