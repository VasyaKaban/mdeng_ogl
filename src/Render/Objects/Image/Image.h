#pragma once

#include <span>
#include "../../Format.h"
#include "../../../hrs/non_creatable.hpp"

enum class ImageType
{
    Image1D,
    Image2D,
    Image3D
};

struct ImageInfo
{
    ImageType image_type;
    Format format;
    Extent3D extent;
    GLsizei mip_levels;
    GLsizei array_layers;
    SampleCount samples;
};

struct ImageSubresourceLayers
{
    GLuint mip_level;
    GLuint base_layer;
    GLuint layer_count;
};

struct ImageInitRegion
{
    ImageSubresourceLayers subresource_layers;
    Extent3D extent;
    Offset3D offset;
    const std::byte* data;
};

struct ImageInitInfo
{
    std::span<const ImageInitRegion> regions;
    bool generate_mip_maps;
    InitDataFormat data_format;
    InitDataType data_type;
};

class Context;

class Image : hrs::non_copyable
{
    friend class ImageView;
    friend class Framebuffer;
public:
    Image() noexcept;
    Image(Context* _parent, const ImageInfo& info, const ImageInitInfo& init_info);
    ~Image();
    Image(Image&& image) noexcept;
    Image& operator=(Image&& image) noexcept;

    void BindTexture(GLuint index) const noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
};