#include "Sampler.h"
#include "../Device/Device.h"
#include <stdexcept>

namespace OpenGL
{
    Sampler::Sampler(Device* _parent, const Render::SamplerInfo& info)
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
            Render::ClearColorValue border_color;
            switch(info.border_color)
            {
                    //0 - transparent
                    //1 - opaque
                case Render::BorderColor::TransparentBlackFloat:
                    border_color = Render::ClearColorValue{std::numeric_limits<float>::min(),
                                                           std::numeric_limits<float>::min(),
                                                           std::numeric_limits<float>::min(),
                                                           std::numeric_limits<float>::min()};
                    parent->GetLoader().SamplerParameterfv(_handle,
                                                           GL_TEXTURE_BORDER_COLOR,
                                                           border_color.float32);
                    break;
                case Render::BorderColor::TransparentBlackInt:
                    border_color =
                        Render::ClearColorValue{std::numeric_limits<std::int32_t>::min(),
                                                std::numeric_limits<std::int32_t>::min(),
                                                std::numeric_limits<std::int32_t>::min(),
                                                std::numeric_limits<std::int32_t>::min()};
                    parent->GetLoader().SamplerParameterIiv(_handle,
                                                            GL_TEXTURE_BORDER_COLOR,
                                                            border_color.int32);
                    break;
                case Render::BorderColor::OpaqueBlackFloat:
                    border_color =
                        Render::ClearColorValue{.float32 = {std::numeric_limits<float>::min(),
                                                            std::numeric_limits<float>::min(),
                                                            std::numeric_limits<float>::min(),
                                                            std::numeric_limits<float>::max()}};
                    parent->GetLoader().SamplerParameterfv(_handle,
                                                           GL_TEXTURE_BORDER_COLOR,
                                                           border_color.float32);
                    break;
                case Render::BorderColor::OpaqueBlackInt:
                    border_color = Render::ClearColorValue{
                        .int32 = {std::numeric_limits<std::int32_t>::min(),
                                  std::numeric_limits<std::int32_t>::min(),
                                  std::numeric_limits<std::int32_t>::min(),
                                  std::numeric_limits<std::int32_t>::max()}};
                    parent->GetLoader().SamplerParameterIiv(_handle,
                                                            GL_TEXTURE_BORDER_COLOR,
                                                            border_color.int32);
                    break;
                case Render::BorderColor::OpaqueWhiteFloat:
                    border_color =
                        Render::ClearColorValue{.float32 = {std::numeric_limits<float>::max(),
                                                            std::numeric_limits<float>::max(),
                                                            std::numeric_limits<float>::max(),
                                                            std::numeric_limits<float>::max()}};
                    parent->GetLoader().SamplerParameterfv(_handle,
                                                           GL_TEXTURE_BORDER_COLOR,
                                                           border_color.float32);
                    break;
                case Render::BorderColor::OpaqueWhiteInt:
                    border_color = Render::ClearColorValue{
                        .int32 = {std::numeric_limits<std::int32_t>::max(),
                                  std::numeric_limits<std::int32_t>::max(),
                                  std::numeric_limits<std::int32_t>::max(),
                                  std::numeric_limits<std::int32_t>::max()}};
                    parent->GetLoader().SamplerParameterIiv(_handle,
                                                            GL_TEXTURE_BORDER_COLOR,
                                                            border_color.int32);
                    break;
                case Render::BorderColor::CustomFloat:
                    parent->GetLoader().SamplerParameterfv(
                        _handle,
                        GL_TEXTURE_BORDER_COLOR,
                        info.custom_border_color_info.color.float32);
                    break;
                case Render::BorderColor::CustomInt:
                    parent->GetLoader().SamplerParameterIuiv(
                        _handle,
                        GL_TEXTURE_BORDER_COLOR,
                        info.custom_border_color_info.color.uint32);
            }
        }

        handle = _handle;
    }

    Sampler::~Sampler()
    {
        parent->GetLoader().DeleteSamplers(1, &handle);
    }

    Render::Device* Sampler::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle Sampler::GetHandle() const noexcept
    {
        return handle;
    }
};