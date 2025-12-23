#include "ImageView.h"
#include "../Image/Image.h"
#include "../Device/Device.h"
#include <stdexcept>

namespace OpenGL
{
    ImageView::ImageView(Device* _parent, const Render::ImageViewInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().GenTextures(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create image view");

        GLenum _inner_type = ImageViewTypeToNative(
            info.view_type,
            static_cast<Image*>(info.image)->GetSampleCount() != Render::SampleCount_1);

        GLenum _inner_format = FormatToNative(info.format).value();

        parent->GetLoader().TextureView(_handle,
                                        _inner_type,
                                        static_cast<const Image*>(info.image)->GetHandle(),
                                        _inner_format,
                                        info.subresource_range.min_mip_level,
                                        info.subresource_range.mip_level_count,
                                        info.subresource_range.min_layer,
                                        info.subresource_range.layer_count);

        GLint swizzle_mask[4] = {ComponentSwizzleToNative(info.components.r, GL_RED),
                                 ComponentSwizzleToNative(info.components.g, GL_GREEN),
                                 ComponentSwizzleToNative(info.components.b, GL_BLUE),
                                 ComponentSwizzleToNative(info.components.a, GL_ALPHA)};

        parent->GetLoader().TextureParameterIiv(_handle, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);

        if(!(info.subresource_range.aspect & Render::ImageAspectFlagBits::AspectColorBit))
        {
            GLint aspect =
                (info.subresource_range.aspect & Render::ImageAspectFlagBits::AspectDepthBit ?
                     GL_DEPTH_COMPONENT :
                     GL_STENCIL_INDEX);

            parent->GetLoader().TextureParameteri(_handle, GL_DEPTH_STENCIL_TEXTURE_MODE, aspect);
        }

        handle = _handle;
    };

    ImageView::~ImageView()
    {
        parent->GetLoader().DeleteTextures(1, &handle);
    }

    Render::Device* ImageView::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle ImageView::GetHandle() const noexcept
    {
        return handle;
    }
};