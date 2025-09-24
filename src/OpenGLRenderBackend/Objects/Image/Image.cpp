#include "Image.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "../CommandBuffer/CommandBuffer.h"
#include "../Buffer/Buffer.h"

namespace OpenGL
{
    Image::Image(Context* _parent, const Render::ImageInfo& info)
        : parent(_parent),
          info(info)
    {
        GLenum _inner_type;
        switch(info.image_type)
        {
            case Render::ImageType::Image1D:
            {
                if(info.array_layers == 1)
                    _inner_type = GL_TEXTURE_1D;
                else
                    _inner_type = GL_TEXTURE_1D_ARRAY;
            }
            break;
            case Render::ImageType::Image2D:
            {
                if(info.array_layers == 1)
                {
                    if(info.samples == Render::SampleCount::SampleCount_1)
                        _inner_type = GL_TEXTURE_2D;
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE;
                }
                else
                {
                    if(info.samples == Render::SampleCount::SampleCount_1)
                        _inner_type = GL_TEXTURE_2D_ARRAY;
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                }
            }
            break;
            case Render::ImageType::Image3D:
            {
                _inner_type = GL_TEXTURE_3D;
            }
            break;
        }

        GLHandle _handle;
        parent->GetLoader().CreateTextures(_inner_type, 1, &_handle);

        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create image");

        GLenum _inner_format = FormatToNative(info.format);
        GLenum _inner_sample_count = SampleCountToNative(info.samples);

        switch(_inner_type)
        {
            case GL_TEXTURE_1D:
                parent->GetLoader().TextureStorage1D(_handle,
                                                     info.mip_levels,
                                                     _inner_format,
                                                     info.extent.width);
                break;
            case GL_TEXTURE_1D_ARRAY:
                parent->GetLoader().TextureStorage2D(_handle,
                                                     info.mip_levels,
                                                     _inner_format,
                                                     info.extent.width,
                                                     info.array_layers);
                break;
            case GL_TEXTURE_2D:
                parent->GetLoader().TextureStorage2D(_handle,
                                                     info.mip_levels,
                                                     _inner_format,
                                                     info.extent.width,
                                                     info.extent.height);
                break;
            case GL_TEXTURE_2D_ARRAY:
                parent->GetLoader().TextureStorage3D(_handle,
                                                     info.mip_levels,
                                                     _inner_format,
                                                     info.extent.width,
                                                     info.extent.height,
                                                     info.array_layers);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE:
                parent->GetLoader().TextureStorage2DMultisample(_handle,
                                                                _inner_sample_count,
                                                                _inner_format,
                                                                info.extent.width,
                                                                info.extent.height,
                                                                GL_TRUE);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                parent->GetLoader().TextureStorage3DMultisample(_handle,
                                                                _inner_sample_count,
                                                                _inner_format,
                                                                info.extent.width,
                                                                info.extent.height,
                                                                info.array_layers,
                                                                GL_TRUE);
                break;
            case GL_TEXTURE_3D:
                parent->GetLoader().TextureStorage3D(_handle,
                                                     info.mip_levels,
                                                     _inner_format,
                                                     info.extent.width,
                                                     info.extent.height,
                                                     info.extent.depth);
                break;
        }

        inner_type = _inner_type;
        inner_format = _inner_format;
        handle = _handle;

        if(!Render::IsFormatCompressed(info.format))
            transfer_type_format_pair = DecodeTransferTypeFormatPair(info.format);
    }

    Image::~Image()
    {
        parent->GetLoader().DeleteTextures(1, &handle);
    }

    const Render::ImageInfo& Image::GetInfo() const noexcept
    {
        return info;
    }

    void Image::CopyToBuffer(const Render::CommandBuffer* cmd,
                             const Render::Buffer* dst,
                             std::span<const Render::BufferImageCopyRegion> regions)
    {
        if(inner_type == GL_TEXTURE_2D_MULTISAMPLE || inner_type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
            throw std::runtime_error("Multisampled images are not supported");

        bool is_compressed = IsFormatCompressed(info.format);
        std::uint16_t format_texel_alignment = GetFormatTexelAlignment(info.format);

        parent->GetLoader().BindBuffer(GL_PIXEL_PACK_BUFFER,
                                       static_cast<const Buffer*>(dst)->GetHandle());
        parent->GetLoader().PixelStorei(GL_PACK_ALIGNMENT, format_texel_alignment);

        for(const auto& reg: regions)
        {
            parent->GetLoader().PixelStorei(GL_PACK_ROW_LENGTH, reg.buffer_row_length);
            parent->GetLoader().PixelStorei(GL_PACK_IMAGE_HEIGHT, reg.buffer_image_height);

            auto region_size = GetFormatRegionSize(info.format, reg);

            switch(inner_type)
            {
                case GL_TEXTURE_1D:
                    if(is_compressed)
                    {
                        parent->GetLoader().GetCompressedTextureSubImage(
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
                            handle,
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
            }
        }
    }

    void Image::Update(const Render::CommandBuffer* cmd,
                       std::span<const Render::MemoryImageCopyRegion> regions)
    {
        if(inner_type == GL_TEXTURE_2D_MULTISAMPLE || inner_type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
            throw std::runtime_error("Multisampled images are not supported");

        bool is_compressed = IsFormatCompressed(info.format);
        std::uint16_t format_texel_alignment = GetFormatTexelAlignment(info.format);

        parent->GetLoader().BindBuffer(GL_PIXEL_UNPACK_BUFFER, OGL_NULL_HANDLE);
        parent->GetLoader().PixelStorei(GL_UNPACK_ALIGNMENT, format_texel_alignment);

        for(const auto& reg: regions)
        {
            parent->GetLoader().PixelStorei(GL_UNPACK_ROW_LENGTH, reg.buffer_row_length);
            parent->GetLoader().PixelStorei(GL_UNPACK_IMAGE_HEIGHT, reg.buffer_image_height);

            auto region_size = GetFormatRegionSize(info.format, reg);

            switch(inner_type)
            {
                case GL_TEXTURE_1D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage1D(
                            handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.extent.width,
                            inner_format,
                            region_size,
                            reg.data);
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage1D(handle,
                                                              reg.subresource_layers.mip_level,
                                                              reg.offset.x,
                                                              reg.extent.width,
                                                              transfer_type_format_pair.format,
                                                              transfer_type_format_pair.type,
                                                              reg.data);
                    }
                    break;
                case GL_TEXTURE_1D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage2D(
                            handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.subresource_layers.layer_count,
                            inner_format,
                            region_size,
                            reg.data);
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage2D(handle,
                                                              reg.subresource_layers.mip_level,
                                                              reg.offset.x,
                                                              reg.subresource_layers.base_layer,
                                                              reg.extent.width,
                                                              reg.subresource_layers.layer_count,
                                                              transfer_type_format_pair.format,
                                                              transfer_type_format_pair.type,
                                                              reg.data);
                    }
                    break;
                case GL_TEXTURE_2D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage2D(
                            handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.extent.width,
                            reg.extent.height,
                            inner_format,
                            region_size,
                            reg.data);
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage2D(handle,
                                                              reg.subresource_layers.mip_level,
                                                              reg.offset.x,
                                                              reg.offset.y,
                                                              reg.extent.width,
                                                              reg.extent.height,
                                                              transfer_type_format_pair.format,
                                                              transfer_type_format_pair.type,
                                                              reg.data);
                    }
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage3D(
                            handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.subresource_layers.base_layer,
                            reg.extent.width,
                            reg.extent.height,
                            reg.subresource_layers.layer_count,
                            inner_format,
                            region_size,
                            reg.data);
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage3D(handle,
                                                              reg.subresource_layers.mip_level,
                                                              reg.offset.x,
                                                              reg.offset.y,
                                                              reg.subresource_layers.base_layer,
                                                              reg.extent.width,
                                                              reg.extent.height,
                                                              reg.subresource_layers.layer_count,
                                                              transfer_type_format_pair.format,
                                                              transfer_type_format_pair.type,
                                                              reg.data);
                    }
                    break;
                case GL_TEXTURE_3D:
                    if(is_compressed)
                    {
                        parent->GetLoader().CompressedTextureSubImage3D(
                            handle,
                            reg.subresource_layers.mip_level,
                            reg.offset.x,
                            reg.offset.y,
                            reg.offset.z,
                            reg.extent.width,
                            reg.extent.height,
                            reg.extent.depth,
                            inner_format,
                            region_size,
                            reg.data);
                    }
                    else
                    {
                        parent->GetLoader().TextureSubImage3D(handle,
                                                              reg.subresource_layers.mip_level,
                                                              reg.offset.x,
                                                              reg.offset.y,
                                                              reg.offset.z,
                                                              reg.extent.width,
                                                              reg.extent.height,
                                                              reg.extent.depth,
                                                              transfer_type_format_pair.format,
                                                              transfer_type_format_pair.type,
                                                              reg.data);
                    }
                    break;
            }
        }
    }

    GLenum Image::GetInnerType() const noexcept
    {
        return inner_type;
    }

    GLenum Image::GetInnerFormat() const noexcept
    {
        return inner_format;
    }

    const TransferImageTypeFormat& Image::GetTransferImageTypeFormatPair() const noexcept
    {
        return transfer_type_format_pair;
    }

    GLHandle Image::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Image::GetContext() const noexcept
    {
        return parent;
    }
};