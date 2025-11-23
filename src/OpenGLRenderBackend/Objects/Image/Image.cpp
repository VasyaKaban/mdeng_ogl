#include "Image.h"
#include "../../Context/Context.h"
#include <stdexcept>

namespace OpenGL
{
    Image::Image(Context* _parent, const Render::ImageInfo& info)
        : parent(_parent),
          format(info.format)
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

    GLenum Image::GetInnerType() const noexcept
    {
        return inner_type;
    }

    GLenum Image::GetInnerFormat() const noexcept
    {
        return inner_format;
    }

    Render::Format Image::GetFormat() const noexcept
    {
        return format;
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