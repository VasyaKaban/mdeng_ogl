#include "Image.h"
#include "../../Context/Context.h"
#include <stdexcept>

Image::Image() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE)
{}

Image::Image(Context* _parent, const ImageInfo& info, const ImageInitInfo& init_info)
    : parent(_parent)
{
    GLenum _inner_type;
    switch(info.image_type)
    {
        case ImageType::Image1D:
        {
            if(info.array_layers == 1)
                _inner_type = GL_TEXTURE_1D;
            else
                _inner_type = GL_TEXTURE_1D_ARRAY;
        }
        break;
        case ImageType::Image2D:
        {
            if(info.array_layers == 1)
            {
                if(info.samples == SampleCount::SampleCount_1)
                    _inner_type = GL_TEXTURE_2D;
                else
                    _inner_type = GL_TEXTURE_2D_MULTISAMPLE;
            }
            else
            {
                if(info.samples == SampleCount::SampleCount_1)
                    _inner_type = GL_TEXTURE_2D_ARRAY;
                else
                    _inner_type = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
            }
        }
        break;
        case ImageType::Image3D:
        {
            _inner_type = GL_TEXTURE_3D;
        }
        break;
    }

    GLHandle _handle;
    parent->GetLoader().CreateTextures(_inner_type, 1, &_handle);

    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("failed to create image");

    auto block_size = GetFormatBlockSize(info.format);
    bool is_compressed = IsFormatCompressed(info.format);

    switch(_inner_type)
    {
        case GL_TEXTURE_1D:
            parent->GetLoader().TextureStorage1D(_handle,
                                                 info.mip_levels,
                                                 static_cast<GLenum>(info.format),
                                                 info.extent.width);

            for(const auto& region: init_info.regions)
            {
                if(is_compressed)
                {
                    auto region_size = GetFormatRegionSize(
                        info.format,
                        Extent3D{.width = region.extent.width, .height = 1, .depth = 1});

                    parent->GetLoader().CompressedTextureSubImage1D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.extent.width,
                        static_cast<GLenum>(info.format),
                        region_size,
                        region.data);
                }
                else
                {
                    parent->GetLoader().TextureSubImage1D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.extent.width,
                        static_cast<GLenum>(init_info.data_format),
                        static_cast<GLenum>(init_info.data_type),
                        region.data);
                }
            }
            break;
        case GL_TEXTURE_1D_ARRAY:
            parent->GetLoader().TextureStorage2D(_handle,
                                                 info.mip_levels,
                                                 static_cast<GLenum>(info.format),
                                                 info.extent.width,
                                                 info.array_layers);

            for(const auto& region: init_info.regions)
            {
                if(is_compressed)
                {
                    auto region_size =
                        GetFormatRegionSize(info.format,
                                            Extent3D{.width = region.extent.width,
                                                     .height = static_cast<GLsizei>(
                                                         region.subresource_layers.layer_count),
                                                     .depth = 1});

                    parent->GetLoader().CompressedTextureSubImage2D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.subresource_layers.base_layer,
                        region.extent.width,
                        region.subresource_layers.layer_count,
                        static_cast<GLenum>(info.format),
                        region_size,
                        region.data);
                }
                else
                {
                    parent->GetLoader().TextureSubImage2D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.subresource_layers.base_layer,
                        region.extent.width,
                        region.subresource_layers.layer_count,
                        static_cast<GLenum>(init_info.data_format),
                        static_cast<GLenum>(init_info.data_type),
                        region.data);
                }
            }
            break;
        case GL_TEXTURE_2D:
            parent->GetLoader().TextureStorage2D(_handle,
                                                 info.mip_levels,
                                                 static_cast<GLenum>(info.format),
                                                 info.extent.width,
                                                 info.extent.height);

            for(const auto& region: init_info.regions)
            {
                if(is_compressed)
                {
                    auto region_size = GetFormatRegionSize(info.format,
                                                           Extent3D{.width = region.extent.width,
                                                                    .height = region.extent.height,
                                                                    .depth = 1});

                    parent->GetLoader().CompressedTextureSubImage2D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.extent.width,
                        region.extent.height,
                        static_cast<GLenum>(info.format),
                        region_size,
                        region.data);
                }
                else
                {
                    parent->GetLoader().TextureSubImage2D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.extent.width,
                        region.extent.height,
                        static_cast<GLenum>(init_info.data_format),
                        static_cast<GLenum>(init_info.data_type),
                        region.data);
                }
            }
            break;
        case GL_TEXTURE_2D_ARRAY:
            parent->GetLoader().TextureStorage3D(_handle,
                                                 info.mip_levels,
                                                 static_cast<GLenum>(info.format),
                                                 info.extent.width,
                                                 info.extent.height,
                                                 info.array_layers);

            for(const auto& region: init_info.regions)
            {
                if(is_compressed)
                {
                    auto region_size =
                        GetFormatRegionSize(info.format,
                                            Extent3D{.width = region.extent.width,
                                                     .height = region.extent.height,
                                                     .depth = static_cast<GLsizei>(
                                                         region.subresource_layers.layer_count)});

                    parent->GetLoader().CompressedTextureSubImage3D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.subresource_layers.base_layer,
                        region.extent.width,
                        region.extent.height,
                        region.subresource_layers.layer_count,
                        static_cast<GLenum>(info.format),
                        region_size,
                        region.data);
                }
                else
                {
                    parent->GetLoader().TextureSubImage3D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.subresource_layers.base_layer,
                        region.extent.width,
                        region.extent.height,
                        region.subresource_layers.layer_count,
                        static_cast<GLenum>(init_info.data_format),
                        static_cast<GLenum>(init_info.data_type),
                        region.data);
                }
            }
            break;
        case GL_TEXTURE_2D_MULTISAMPLE:
            parent->GetLoader().TextureStorage2DMultisample(_handle,
                                                            static_cast<GLsizei>(info.samples),
                                                            static_cast<GLenum>(info.format),
                                                            info.extent.width,
                                                            info.extent.height,
                                                            GL_TRUE);

            //noop for data load
            break;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            parent->GetLoader().TextureStorage3DMultisample(_handle,
                                                            static_cast<GLsizei>(info.samples),
                                                            static_cast<GLenum>(info.format),
                                                            info.extent.width,
                                                            info.extent.height,
                                                            info.array_layers,
                                                            GL_TRUE);

            //noop for data load
            break;
        case GL_TEXTURE_3D:
            parent->GetLoader().TextureStorage3D(_handle,
                                                 info.mip_levels,
                                                 static_cast<GLenum>(info.format),
                                                 info.extent.width,
                                                 info.extent.height,
                                                 info.extent.depth);

            for(const auto& region: init_info.regions)
            {
                if(is_compressed)
                {
                    auto region_size = GetFormatRegionSize(info.format,
                                                           Extent3D{.width = region.extent.width,
                                                                    .height = region.extent.height,
                                                                    .depth = region.extent.depth});

                    parent->GetLoader().CompressedTextureSubImage3D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.offset.z,
                        region.extent.width,
                        region.extent.height,
                        region.extent.depth,
                        static_cast<GLenum>(info.format),
                        region_size,
                        region.data);
                }
                else
                {
                    parent->GetLoader().TextureSubImage3D(
                        _handle,
                        region.subresource_layers.mip_level,
                        region.offset.x,
                        region.offset.y,
                        region.offset.z,
                        region.extent.width,
                        region.extent.height,
                        region.extent.depth,
                        static_cast<GLenum>(init_info.data_format),
                        static_cast<GLenum>(init_info.data_type),
                        region.data);
                }
            }
            break;
    }

    if(init_info.generate_mip_maps)
        parent->GetLoader().GenerateTextureMipmap(_handle);

    handle = _handle;
}

Image::~Image()
{
    destroy();
}

Image::Image(Image&& image) noexcept
    : parent(image.parent),
      handle(std::exchange(image.handle, GL_NULL_HANDLE))
{}

Image& Image::operator=(Image&& image) noexcept
{
    destroy();

    parent = image.parent;
    handle = std::exchange(image.handle, GL_NULL_HANDLE);

    return *this;
}

void Image::BindTexture(GLuint index) const noexcept
{
    parent->GetLoader().BindTextureUnit(index, handle);
}

bool Image::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void Image::destroy() noexcept
{
    if(IsCreated())
        parent->GetLoader().DeleteTextures(1, &handle);
}