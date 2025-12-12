#include "CommandBuffer.h"
#include "../../Context/Context.h"
#include "../Buffer/Buffer.h"
#include "../Image/Image.h"
#include "../Pipeline/Pipeline.h"
#include "../ImageView/ImageView.h"
#include "../Sampler/Sampler.h"
#include "../RenderPass/RenderPass.h"
#include "../DescriptorSet/DescriptorSet.h"
#include <cassert>

namespace OpenGL
{
    CommandBuffer::CommandBuffer(Context* _parent, CommandPool* _pool) noexcept
        : parent(_parent),
          pool(_pool),
          bound_pipeline(nullptr)
    {}

    CommandBuffer::~CommandBuffer()
    {
        //noop
    }

    void CommandBuffer::Reset(const Render::CommandBufferResetInfo& reset_info)
    {
        //noop
    }
    void CommandBuffer::Begin(const Render::CommandBufferBeginInfo& begin_info)
    {
        //noop
    }
    void CommandBuffer::End()
    {
        bound_pipeline = nullptr;
    }

    Render::Context* CommandBuffer::GetContext() const noexcept
    {
        return parent;
    }

    //Buffer
    void
    CommandBuffer::CopyBufferToBuffer(Render::Buffer* src,
                                      Render::Buffer* dst,
                                      std::span<const Render::BufferCopyRegion> regions) noexcept
    {
        Buffer* src_buffer = static_cast<Buffer*>(src);
        Buffer* dst_buffer = static_cast<Buffer*>(dst);
        for(const auto& reg: regions)
        {
            parent->GetLoader().CopyNamedBufferSubData(src_buffer->GetHandle(),
                                                       dst_buffer->GetHandle(),
                                                       reg.src_offset,
                                                       reg.dst_offset,
                                                       reg.size);
        }
    }
    void CommandBuffer::CopyBufferToImage(Render::Buffer* src,
                                          Render::Image* dst,
                                          std::span<const Render::BufferImageCopyRegion> regions)
    {
        Buffer* src_buffer = static_cast<Buffer*>(src);
        Image* dst_image = static_cast<Image*>(dst);

        Render::Format image_format = dst_image->GetFormat();
        bool is_compressed = IsFormatCompressed(image_format);
        GLenum inner_type = dst_image->GetInnerType();
        GLHandle image_handle = dst_image->GetHandle();
        std::uint16_t format_texel_alignment = GetFormatTexelAlignment(image_format);
        GLenum format = dst_image->GetInnerFormat();

        const TransferImageTypeFormat& transfer_type_format_pair =
            dst_image->GetTransferImageTypeFormatPair();

        parent->GetLoader().BindBuffer(GL_PIXEL_UNPACK_BUFFER, src_buffer->GetHandle());
        parent->GetLoader().PixelStorei(GL_UNPACK_ALIGNMENT, format_texel_alignment);

        for(const auto& reg: regions)
        {
            parent->GetLoader().PixelStorei(GL_UNPACK_ROW_LENGTH, reg.buffer_row_length);
            parent->GetLoader().PixelStorei(GL_UNPACK_IMAGE_HEIGHT, reg.buffer_image_height);

            auto region_size = GetFormatRegionSize(image_format, reg);

            switch(inner_type)
            {
                case GL_TEXTURE_1D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage1D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.extent.width,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage1D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.extent.width,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_1D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage2D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.subresource_layers.layer_count,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage2D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.subresource_layers.layer_count,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_2D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage2D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.extent.width,
                            reg.extent.height,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage2D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.extent.width,
                            reg.extent.height,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_2D_MULTISAMPLE:
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                    //noop
                    break;
                case GL_TEXTURE_3D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.offset.z,
                            reg.extent.width,
                            reg.extent.height,
                            reg.extent.depth,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.offset.z,
                            reg.extent.width,
                            reg.extent.height,
                            reg.extent.depth,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_CUBE_MAP:
                case GL_TEXTURE_CUBE_MAP_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            format,
                            region_size,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage3D(
                            image_handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
            }
        }
    }

    void CommandBuffer::CopyImageToBuffer(Render::Image* src,
                                          Render::Buffer* dst,
                                          std::span<const Render::BufferImageCopyRegion> regions)
    {
        Image* src_image = static_cast<Image*>(src);
        Buffer* dst_buffer = static_cast<Buffer*>(dst);

        auto inner_type = src_image->GetInnerType();

        //if(inner_type == GL_TEXTURE_2D_MULTISAMPLE || inner_type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
        //    throw std::runtime_error("Multisampled images are not supported");

        Render::Format image_format = src_image->GetFormat();
        const auto& transfer_type_format_pair = src_image->GetTransferImageTypeFormatPair();

        bool is_compressed = IsFormatCompressed(image_format);
        std::uint16_t format_texel_alignment = GetFormatTexelAlignment(image_format);

        parent->GetLoader().BindBuffer(GL_PIXEL_PACK_BUFFER, dst_buffer->GetHandle());
        parent->GetLoader().PixelStorei(GL_PACK_ALIGNMENT, format_texel_alignment);

        for(const auto& reg: regions)
        {
            parent->GetLoader().PixelStorei(GL_PACK_ROW_LENGTH, reg.buffer_row_length);
            parent->GetLoader().PixelStorei(GL_PACK_IMAGE_HEIGHT, reg.buffer_image_height);

            auto region_size = GetFormatRegionSize(image_format, reg);

            switch(inner_type)
            {
                case GL_TEXTURE_1D:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            0,
                            0,
                            reg.extent.width,
                            1,
                            1,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            0,
                            0,
                            reg.extent.width,
                            1,
                            1,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_1D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.subresource_layers.base_layer,
                            0,
                            reg.extent.width,
                            reg.subresource_layers.layer_count,
                            1,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.subresource_layers.base_layer,
                            0,
                            reg.extent.width,
                            reg.subresource_layers.layer_count,
                            1,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_2D:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            0,
                            reg.extent.width,
                            reg.extent.height,
                            1,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            0,
                            reg.extent.width,
                            reg.extent.height,
                            1,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_3D:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.offset.z,
                            reg.extent.width,
                            reg.extent.height,
                            reg.extent.depth,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.offset.z,
                            reg.extent.width,
                            reg.extent.height,
                            reg.extent.depth,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
                case GL_TEXTURE_CUBE_MAP:
                case GL_TEXTURE_CUBE_MAP_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    else
                    {
                        parent->GetLoader().GetTextureSubImage(
                            src_image->GetHandle(),
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            transfer_type_format_pair.format,
                            transfer_type_format_pair.type,
                            region_size,
                            reinterpret_cast<void*>(reg.buffer_offset));
                    }
                    break;
            }
        }
    }

    //Pipeline
    void CommandBuffer::BindPipeline(Render::Pipeline* pipeline)
    {
        bound_pipeline = static_cast<Pipeline*>(pipeline);
        bound_pipeline->Bind(*this);
    }

    void CommandBuffer::BindVertexBuffer(std::uint32_t first_binding,
                                         std::uint32_t binding_count,
                                         Render::Buffer** buffers,
                                         std::int64_t* offsets)
    {
        for(std::uint32_t i = 0; i < binding_count; i++)
            bound_pipeline->BindVertexBuffer(*this,
                                             *static_cast<Buffer*>(buffers[i]),
                                             first_binding + i,
                                             offsets[i]);
    }

    void CommandBuffer::BindIndexBuffer(Render::Buffer* buffer,
                                        Render::IndexType type,
                                        std::uintptr_t offset)
    {
        bound_pipeline->BindIndexBuffer(*this, *static_cast<Buffer*>(buffer), type, offset);
    }

    void CommandBuffer::Draw(std::uint32_t vertex_count,
                             std::uint32_t instance_count,
                             std::uint32_t first_vertex,
                             std::uint32_t first_instance)
    {
        bound_pipeline->Draw(*this, vertex_count, instance_count, first_vertex, first_instance);
    }

    void CommandBuffer::DrawIndexed(std::uint32_t index_count,
                                    std::uint32_t instance_count,
                                    std::uint32_t first_index,
                                    std::int32_t vertex_offset,
                                    std::uint32_t first_instance)
    {
        bound_pipeline->DrawIndexed(*this,
                                    index_count,
                                    instance_count,
                                    first_index,
                                    vertex_offset,
                                    first_instance);
    }

    void CommandBuffer::DrawIndirect(Render::Buffer* buffer,
                                     std::uint64_t offset,
                                     std::uint32_t draw_count,
                                     std::uint32_t stride)
    {
        parent->GetLoader().BindBuffer(GL_DRAW_INDIRECT_BUFFER,
                                       static_cast<Buffer*>(buffer)->GetHandle());

        bound_pipeline->DrawIndirect(*this, offset, draw_count, stride);
    }

    void CommandBuffer::DrawIndexedIndirect(Render::Buffer* buffer,
                                            std::uint64_t offset,
                                            std::uint32_t draw_count,
                                            std::uint32_t stride)
    {
        parent->GetLoader().BindBuffer(GL_DRAW_INDIRECT_BUFFER,
                                       static_cast<Buffer*>(buffer)->GetHandle());

        bound_pipeline->DrawIndexedIndirect(*this, offset, draw_count, stride);
    }

    void
    CommandBuffer::Dispatch(std::uint32_t x_groups, std::uint32_t y_groups, std::uint32_t z_groups)
    {
        parent->GetLoader().DispatchCompute(x_groups, y_groups, z_groups);
    }

    void CommandBuffer::DispatchIndirect(Render::Buffer* buffer, std::uint64_t offset)
    {
        parent->GetLoader().BindBuffer(GL_DISPATCH_INDIRECT_BUFFER,
                                       static_cast<Buffer*>(buffer)->GetHandle());
        parent->GetLoader().DispatchComputeIndirect(offset);
    }

    void CommandBuffer::BindDescriptorSets(std::uint32_t first_set,
                                           std::span<const Render::DescriptorSet*> sets)
    {
        for(std::uint32_t i = 0; i < sets.size(); i++)
            static_cast<const DescriptorSet*>(sets[i])->Bind(*this);
    }

    //Dynamic state
    void CommandBuffer::SetViewport(std::uint32_t first_viewport,
                                    std::span<const Render::Viewport> viewports)
    {
        parent->GetLoader().ViewportArrayv(first_viewport, viewports.size(), &viewports.data()->x);
    }

    void CommandBuffer::SetScissors(std::uint32_t first_scissor,
                                    std::span<const Render::Rect2D> scissors)
    {
        parent->GetLoader().Enable(GL_SCISSOR_TEST);
        parent->GetLoader().ScissorArrayv(first_scissor,
                                          scissors.size(),
                                          &scissors.data()->offset.x);
    }

    void CommandBuffer::SetUniform(Render::ShaderStageFlags stages,
                                   std::uint32_t offset,
                                   std::span<const std::byte> data,
                                   std::span<const Render::UniformDesc> uniform_descs)
    {
        GLHandle handle = bound_pipeline->GetHandle();

#define VECTOR_CASE(EXTENT, SIZE) \
    case EXTENT: \
        switch(desc.type) \
        { \
            case Render::UniformType::Float: \
            { \
                parent->GetLoader().ProgramUniform##SIZE##fv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    reinterpret_cast<const GLfloat*>(data.data() + desc.offset)); \
            } \
            break; \
            case Render::UniformType::Int: \
            { \
                parent->GetLoader().ProgramUniform##SIZE##iv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    reinterpret_cast<const GLint*>(data.data() + desc.offset)); \
            } \
            break; \
            case Render::UniformType::UInt: \
            { \
                parent->GetLoader().ProgramUniform##SIZE##uiv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    reinterpret_cast<const GLuint*>(data.data() + desc.offset)); \
            } \
            break; \
            case Render::UniformType::Double: \
            { \
                parent->GetLoader().ProgramUniform##SIZE##dv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    reinterpret_cast<const GLdouble*>(data.data() + desc.offset)); \
            } \
            break; \
        } \
        break;

//__VA_ARGS__ -> COLS
#define MATRIX_CASE(EXTENT, ROWS, ...) \
    case EXTENT: \
        switch(desc.type) \
        { \
            case Render::UniformType::Float: \
            { \
                parent->GetLoader().ProgramUniformMatrix##ROWS##__VA_OPT__(x##__VA_ARGS__)##fv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    GL_FALSE, \
                    reinterpret_cast<const GLfloat*>(data.data() + desc.offset)); \
            } \
            break; \
            case Render::UniformType::Double: \
            { \
                parent->GetLoader().ProgramUniformMatrix##ROWS##__VA_OPT__(x##__VA_ARGS__)##dv( \
                    handle, \
                    desc.location, \
                    desc.count, \
                    GL_FALSE, \
                    reinterpret_cast<const GLdouble*>(data.data() + desc.offset)); \
            } \
            break; \
            default: \
                assert(false); \
                break; \
        } \
        break;

        for(const auto& desc: uniform_descs)
        {
            if(desc.extent == Render::UniformExtent::Scalar ||
               desc.extent == Render::UniformExtent::Vec2 ||
               desc.extent == Render::UniformExtent::Vec3 ||
               desc.extent == Render::UniformExtent::Vec4)
            {
                switch(desc.extent)
                {
                    VECTOR_CASE(Render::UniformExtent::Scalar, 1)
                    VECTOR_CASE(Render::UniformExtent::Vec2, 2)
                    VECTOR_CASE(Render::UniformExtent::Vec3, 3)
                    VECTOR_CASE(Render::UniformExtent::Vec4, 4)
                    default:
                        assert(false);
                        break;
                }
            }
            else
            {
                switch(desc.extent)
                {
                    MATRIX_CASE(Render::UniformExtent::Mat2x2, 2)
                    MATRIX_CASE(Render::UniformExtent::Mat2x3, 2, 3)
                    MATRIX_CASE(Render::UniformExtent::Mat2x4, 2, 4)
                    MATRIX_CASE(Render::UniformExtent::Mat3x2, 3, 2)
                    MATRIX_CASE(Render::UniformExtent::Mat3x3, 3)
                    MATRIX_CASE(Render::UniformExtent::Mat3x4, 3, 4)
                    MATRIX_CASE(Render::UniformExtent::Mat4x2, 4, 2)
                    MATRIX_CASE(Render::UniformExtent::Mat4x3, 4, 3)
                    MATRIX_CASE(Render::UniformExtent::Mat4x4, 4)
                    default:
                        assert(false);
                        break;
                }
            }
        }
    }

    //Renderpass
    void CommandBuffer::BeginRenderPass(Render::RenderPass* renderpass,
                                        const Render::RenderPassBeginInfo& info)
    {
        RenderPass* rpass = static_cast<RenderPass*>(renderpass);
        rpass->Begin(*this, info);
    }

    void CommandBuffer::EndRenderPass()
    {
        //noop
    }

    //Common
    void CommandBuffer::SetPipelineBarrier(const Render::PipelineBarrier& barrier)
    {
        if(barrier.dependency & Render::DependencyFlagBits::ByRegion)
            parent->GetLoader().MemoryBarrierByRegion(PipelineBarrierToNative(barrier));
        else
            parent->GetLoader().MemoryBarrier(PipelineBarrierToNative(barrier));
    }
};