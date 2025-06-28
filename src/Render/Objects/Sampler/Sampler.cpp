#include "Sampler.h"
#include "../../Context/Context.h"
#include <stdexcept>

Sampler::Sampler() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE)
{}

Sampler::Sampler(Context* _parent, const SamplerInfo& info)
    : parent(_parent)
{
    GLHandle _handle;
    parent->GetLoader().GenSamplers(1, &_handle);
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create sampler");

    GLint mag_filter = static_cast<GLint>(info.mag_filter);
    GLint min_filter;
    if(info.min_filter == Filter::Linear)
    {
        if(info.mipmap_mode == Filter::Linear)
            min_filter = GL_LINEAR_MIPMAP_LINEAR;
        else
            min_filter = GL_LINEAR_MIPMAP_NEAREST;
    }
    else
    {
        if(info.mipmap_mode == Filter::Linear)
            min_filter = GL_NEAREST_MIPMAP_LINEAR;
        else
            min_filter = GL_NEAREST_MIPMAP_NEAREST;
    }

    GLint texture_wrap_s = static_cast<GLint>(info.address_mode_u);
    GLint texture_wrap_t = static_cast<GLint>(info.address_mode_v);
    GLint texture_wrap_r = static_cast<GLint>(info.address_mode_w);

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

        GLint compare_op = static_cast<GLint>(info.compare_op);
        parent->GetLoader().SamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, compare_op);
    }

    parent->GetLoader().SamplerParameterf(_handle, GL_TEXTURE_MIN_LOD, info.min_lod);
    parent->GetLoader().SamplerParameterf(_handle, GL_TEXTURE_MAX_LOD, info.max_lod);

    handle = _handle;
}

Sampler::~Sampler()
{
    destroy();
}

Sampler::Sampler(Sampler&& sampler) noexcept
    : parent(sampler.parent),
      handle(std::exchange(sampler.handle, GL_NULL_HANDLE))
{}

Sampler& Sampler::operator=(Sampler&& sampler) noexcept
{
    destroy();

    parent = sampler.parent;
    handle = std::exchange(sampler.handle, GL_NULL_HANDLE);

    return *this;
}

void Sampler::BindSampler(GLuint index) const noexcept
{
    parent->GetLoader().BindSampler(index, handle);
}

bool Sampler::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void Sampler::destroy() noexcept
{
    if(IsCreated())
        parent->GetLoader().DeleteSamplers(1, &handle);
}
