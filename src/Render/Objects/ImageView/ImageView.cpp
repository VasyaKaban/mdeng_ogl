#include "ImageView.h"
#include "../Image/Image.h"
#include "../../Context/Context.h"
#include <stdexcept>

ImageView::ImageView() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE)
{}

ImageView::ImageView(Context* _parent, const ImageViewInfo& info)
    : parent(_parent)
{
    GLHandle _handle;
    parent->GetLoader().GenTextures(1, &_handle);
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to creat image view");

    parent->GetLoader().TextureView(_handle,
                                    static_cast<GLenum>(info.view_type),
                                    info.image->handle,
                                    static_cast<GLenum>(info.format),
                                    info.subresource_range.min_mip_level,
                                    info.subresource_range.mip_level_count,
                                    info.subresource_range.min_layer,
                                    info.subresource_range.layer_count);

    GLint swizzle_mask[4] = {
        (info.components.r == ComponentSwizzle::SwizzleIdentity ?
             GL_RED :
             static_cast<GLint>(info.components.r)),
        (info.components.g == ComponentSwizzle::SwizzleIdentity ?
             GL_GREEN :
             static_cast<GLint>(info.components.g)),
        (info.components.b == ComponentSwizzle::SwizzleIdentity ?
             GL_BLUE :
             static_cast<GLint>(info.components.b)),
        (info.components.a == ComponentSwizzle::SwizzleIdentity ?
             GL_ALPHA :
             static_cast<GLint>(info.components.a)),
    };

    parent->GetLoader().TextureParameterIiv(_handle, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);

    handle = _handle;
}

ImageView::~ImageView()
{
    destroy();
}

ImageView::ImageView(ImageView&& image_view) noexcept
    : parent(image_view.parent),
      handle(std::exchange(image_view.handle, GL_NULL_HANDLE))
{}

ImageView& ImageView::operator=(ImageView&& image_view) noexcept
{
    destroy();

    parent = image_view.parent;
    handle = std::exchange(image_view.handle, GL_NULL_HANDLE);

    return *this;
}

void ImageView::BindTextureView(GLuint index) const noexcept
{
    parent->GetLoader().BindTextureUnit(index, handle);
}

bool ImageView::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void ImageView::destroy() noexcept
{
    if(IsCreated())
        parent->GetLoader().DeleteTextures(1, &handle);
}
