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
        CommandBuffer(Context* _parent, CommandPool* _pool) noexcept;
        virtual ~CommandBuffer() override;

        virtual void Reset() override;
        virtual void Begin() override;
        virtual void End() override;

        virtual Render::Context* GetContext() const noexcept override;

        //all image copy commands rely on general image layout. So, after creating an image we should transfer image layout to general and then transfer, render, etc...
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
        virtual void TransferToGeneralImageLayout(Render::Image* src) override;

        virtual void
        CopyImageToBuffer(Render::Image* src,
                          Render::Buffer* dst,
                          std::span<const Render::BufferImageCopyRegion> regions) override;

        //Pipeline
        virtual void Bind(Render::Pipeline* pipeline) override;

        virtual void BindVertexBuffer(Render::Buffer* buffer,
                                      std::uint32_t binding,
                                      std::int64_t offset) override;
        virtual void BindIndexBuffer(Render::Buffer* buffer,
                                     Render::IndexType type,
                                     std::uintptr_t offset) override;

        virtual void Draw(std::uint32_t vertex_count,
                          std::uint32_t instance_count,
                          std::uint32_t first_vertex) override;

        virtual void DrawIndexed(std::uint32_t index_count,
                                 std::uint32_t instance_count,
                                 std::uint32_t first_index,
                                 std::int32_t vertex_offset) override;

        virtual void
        Dispatch(std::uint32_t x_groups, std::uint32_t y_groups, std::uint32_t z_groups) override;

        virtual void BindUniformBuffer(const Render::DescriptorBufferDesc& desc) override;
        virtual void BindShaderStorageBuffer(const Render::DescriptorBufferDesc& desc) override;
        virtual void BindCombinedImageSampler(const Render::DescriptorImageDesc& desc) override;
        virtual void BindStorageImage(const Render::DescriptorImageDesc& desc) override;

        //Dynamic state
        virtual void SetViewport(std::uint32_t first_viewport,
                                 std::span<const Render::Viewport> viewports) override;
        virtual void SetScissors(std::uint32_t first_scissor,
                                 std::span<const Render::Rect2D> scissors) override;

        virtual void SetUniform(const Render::UniformDesc& desc,
                                std::span<const std::byte> data) override;

        //Renderpass
        virtual void BeginRenderPass(Render::RenderPass* renderpass,
                                     const Render::RenderPassBeginInfo& info) override;
        virtual void EndRenderPass() override;

        //Common
        virtual void SetPipelineBarrier(const Render::PipelineBarrier& barrier) override;
    private:
        Context* parent;
        CommandPool* pool;

        Pipeline* bound_pipeline;
    };
};