#pragma once

#include "../../Format.h"
#include "../../../hrs/non_creatable.hpp"

enum class ImageViewType
{
    ImageView1D = GL_TEXTURE_1D,
    ImageView2D = GL_TEXTURE_2D,
    ImageView3D = GL_TEXTURE_3D,
    ImageViewCubeMap = GL_TEXTURE_CUBE_MAP,
    ImageView1DArray = GL_TEXTURE_1D_ARRAY,
    ImageView2DArray = GL_TEXTURE_2D_ARRAY,
    ImageViewCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
    ImageView2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
    ImageView2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};

enum class ComponentSwizzle
{
    SwizzleRed = GL_RED,
    SwizzleGreen = GL_GREEN,
    SwizzleBlue = GL_BLUE,
    SwizzleAlpha = GL_ALPHA,
    SwizzleZero = GL_ZERO,
    SwizzleOne = GL_ONE,
    SwizzleIdentity
};

struct ComponentMapping
{
    ComponentSwizzle r;
    ComponentSwizzle g;
    ComponentSwizzle b;
    ComponentSwizzle a;
};

struct ImageSubresourceRange
{
    GLuint min_mip_level;
    GLuint mip_level_count;
    GLuint min_layer;
    GLuint layer_count;
};

class Image;

struct ImageViewInfo
{
    const Image* image;
    ImageViewType view_type;
    Format format;
    ComponentMapping components;
    ImageSubresourceRange subresource_range;
};

class Context;

class ImageView : hrs::non_copyable
{
    friend class Framebuffer;
public:
    ImageView() noexcept;
    ImageView(Context* _parent, const ImageViewInfo& info);
    ~ImageView();
    ImageView(ImageView&& image_view) noexcept;
    ImageView& operator=(ImageView&& image_view) noexcept;

    void BindTextureView(GLuint index) const noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
};