#include "ImageView.h"
#include "../Image/Image.h"
#include "../../Context/Context.h"
#include <stdexcept>

namespace OpenGL
{
    static GLint swizzle_mapping(GLint def, Render::ComponentSwizzle component) noexcept
    {
        if(component == Render::ComponentSwizzle::SwizzleIdentity)
            return def;

        return ComponentSwizzleToNative(component);
    }

    ImageView::ImageView(Context* _parent, const Render::ImageViewInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().GenTextures(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to creat image view");

        GLenum _inner_type = ImageViewTypeToNative(info.view_type);
        GLenum _inner_format = FormatToNative(info.format);

        parent->GetLoader().TextureView(_handle,
                                        _inner_type,
                                        static_cast<const Image*>(info.image)->GetHandle(),
                                        _inner_format,
                                        info.subresource_range.min_mip_level,
                                        info.subresource_range.mip_level_count,
                                        info.subresource_range.min_layer,
                                        info.subresource_range.layer_count);

        GLint swizzle_mask[4] = {
            swizzle_mapping(GL_RED, info.components.r),
            swizzle_mapping(GL_GREEN, info.components.g),
            swizzle_mapping(GL_BLUE, info.components.b),
            swizzle_mapping(GL_ALPHA, info.components.a),
        };

        parent->GetLoader().TextureParameterIiv(_handle, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);

        handle = _handle;
    };

    ImageView::~ImageView()
    {
        parent->GetLoader().DeleteTextures(1, &handle);
    }

    GLHandle ImageView::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* ImageView::GetContext() const noexcept
    {
        return parent;
    }
};