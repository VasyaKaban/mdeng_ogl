#include "Buffer.h"
#include <stdexcept>
#include "../../Context/Context.h"
#include "../Image/Image.h"

namespace OpenGL
{
    Buffer::Buffer(Context* _parent, const Render::BufferInfo& info)
        : parent(_parent)
    {
        GLHandle _handle = OGL_NULL_HANDLE;
        parent->GetLoader().CreateBuffers(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create buffer");

        auto full_flags = BufferFlagsToNative(info.flags) | GL_CLIENT_STORAGE_BIT;
        parent->GetLoader().NamedBufferStorage(_handle, info.size, nullptr, full_flags);

        handle = _handle;
        size = info.size;
        flags = full_flags;
    }

    Buffer::~Buffer()
    {
        parent->GetLoader().DeleteBuffers(1, &handle);
    }

    std::byte* Buffer::Map(const Render::MappedRange& rng)
    {
        if(handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Usage of null buffer handle");

        std::byte* map_ptr = reinterpret_cast<std::byte*>(parent->GetLoader().MapNamedBufferRange(
            handle,
            rng.offset,
            rng.size,
            (flags | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_FLUSH_EXPLICIT_BIT) &
                ~(GL_CLIENT_STORAGE_BIT)));
        if(!map_ptr)
            throw std::runtime_error("Failed to map buffer");

        return map_ptr;
    }

    void Buffer::Unmap() noexcept
    {
        parent->GetLoader().UnmapNamedBuffer(handle);
    }

    void Buffer::FlushMappedRange(std::span<const Render::MappedRange> ranges)
    {
        for(const auto& rng: ranges)
            parent->GetLoader().FlushMappedNamedBufferRange(handle, rng.offset, rng.size);
    }

    std::uint64_t Buffer::GetSize() const noexcept
    {
        return size;
    }

    void Buffer::CopyToBuffer(const Render::CommandBuffer* cmd,
                              const Render::Buffer* dst,
                              std::span<const Render::BufferCopyRegion> regions) noexcept
    {
        for(const auto& reg: regions)
        {
            parent->GetLoader().CopyNamedBufferSubData(handle,
                                                       static_cast<const Buffer*>(dst)->GetHandle(),
                                                       reg.src_offset,
                                                       reg.dst_offset,
                                                       reg.size);
        }
    }

    void Buffer::CopyToImage(const Render::CommandBuffer* cmd,
                             const Render::Image* dst,
                             std::span<const Render::BufferImageCopyRegion> regions)
    {
        const Image* dst_image = static_cast<const Image*>(dst);

        const auto& image_info = dst_image->GetInfo();
        bool is_compressed = IsFormatCompressed(image_info.format);
        GLenum inner_type = dst_image->GetInnerType();
        GLHandle image_handle = dst_image->GetHandle();
        std::uint16_t format_texel_alignment = GetFormatTexelAlignment(image_info.format);
        GLenum format = dst_image->GetInnerFormat();

        parent->GetLoader().BindBuffer(GL_PIXEL_UNPACK_BUFFER, handle);
        parent->GetLoader().PixelStorei(GL_UNPACK_ALIGNMENT, format_texel_alignment);

        for(const auto& reg: regions)
        {
            parent->GetLoader().PixelStorei(GL_UNPACK_ROW_LENGTH, reg.buffer_row_length);
            parent->GetLoader().PixelStorei(GL_UNPACK_IMAGE_HEIGHT, reg.buffer_image_height);

            auto region_size = GetFormatRegionSize(image_info.format, reg);

            GLenum data_format = ImageCopyDataFormatToNative(reg.data_format);
            GLenum data_type = ImageCopyDataTypeToNative(reg.data_type);

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
                            data_format,
                            data_type,
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
                            data_format,
                            data_type,
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
                            data_format,
                            data_type,
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
                            data_format,
                            data_type,
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
                            data_format,
                            data_type,
                            reinterpret_cast<const void*>(reg.buffer_offset));
                    }
                    break;
            }
        }
    }

    void Buffer::Update(const Render::CommandBuffer* cmd,
                        std::int64_t offset,
                        std::span<const std::byte*> data) noexcept
    {
        parent->GetLoader().NamedBufferSubData(handle, offset, data.size(), data.data());
    }

    GLHandle Buffer::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Buffer::GetContext() const noexcept
    {
        return parent;
    }
};