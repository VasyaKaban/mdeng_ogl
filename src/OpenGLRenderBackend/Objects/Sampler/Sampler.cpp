#include "Sampler.h"
#include "../../Context/Context.h"
#include <stdexcept>

namespace OpenGL
{
    Sampler::Sampler(Context* _parent, const Render::SamplerInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().GenSamplers(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create sampler");

        GLint mag_filter = FilterToNative(info.mag_filter);
        GLint min_filter;
        if(info.min_filter == Render::Filter::Linear)
        {
            if(info.mipmap_mode == Render::Filter::Linear)
                min_filter = GL_LINEAR_MIPMAP_LINEAR;
            else
                min_filter = GL_LINEAR_MIPMAP_NEAREST;
        }
        else
        {
            if(info.mipmap_mode == Render::Filter::Linear)
                min_filter = GL_NEAREST_MIPMAP_LINEAR;
            else
                min_filter = GL_NEAREST_MIPMAP_NEAREST;
        }

        GLint texture_wrap_s = AddressModeToNative(info.address_mode_u);
        GLint texture_wrap_t = AddressModeToNative(info.address_mode_v);
        GLint texture_wrap_r = AddressModeToNative(info.address_mode_w);

        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, mag_filter);
        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, min_filter);
        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_WRAP_S, texture_wrap_s);
        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_WRAP_T, texture_wrap_t);
        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_WRAP_R, texture_wrap_r);
        parent->GetLoader().SamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, info.mip_lod_bias);
        if(info.anisotropy_enable)
            parent->GetLoader().SamplerParameterf(_handle,
                                                  GL_TEXTURE_MAX_ANISOTROPY,
                                                  info.max_anisotropy);
        if(info.compare_enable)
        {
            parent->GetLoader().SamplerParameteri(_handle,
                                                  GL_TEXTURE_COMPARE_MODE,
                                                  GL_COMPARE_REF_TO_TEXTURE);

            GLint compare_op = ComapreOpToNative(info.compare_op);
            parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, compare_op);
        }

        parent->GetLoader().SamplerParameterf(_handle, GL_TEXTURE_MIN_LOD, info.min_lod);
        parent->GetLoader().SamplerParameterf(_handle, GL_TEXTURE_MAX_LOD, info.max_lod);

        if(info.address_mode_u == Render::AddressMode::ClampToBorder ||
           info.address_mode_v == Render::AddressMode::ClampToBorder ||
           info.address_mode_w == Render::AddressMode::ClampToBorder)
        {
            if(std::holds_alternative<Render::ClearColorFloatValue>(info.border_color.value))
            {
                const auto& color = std::get<Render::ClearColorFloatValue>(info.border_color.value);
                parent->GetLoader().SamplerParameterfv(_handle,
                                                       GL_TEXTURE_BORDER_COLOR,
                                                       color.data());
            }
            else if(std::holds_alternative<Render::ClearColorIntValue>(info.border_color.value))
            {
                const auto& color = std::get<Render::ClearColorIntValue>(info.border_color.value);
                parent->GetLoader().SamplerParameterIiv(_handle,
                                                        GL_TEXTURE_BORDER_COLOR,
                                                        color.data());
            }
            else // if(std::holds_alternative<ClearColorUIntValue>(info.border_color.value))
            {
                const auto& color = std::get<Render::ClearColorUIntValue>(info.border_color.value);
                parent->GetLoader().SamplerParameterIuiv(_handle,
                                                         GL_TEXTURE_BORDER_COLOR,
                                                         color.data());
            }
        }

        handle = _handle;
    }

    Sampler::~Sampler()
    {
        parent->GetLoader().DeleteSamplers(1, &handle);
    }

    GLHandle Sampler::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Sampler::GetContext() const noexcept
    {
        return parent;
    }
};