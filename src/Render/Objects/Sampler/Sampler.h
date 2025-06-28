#pragma once

#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

enum class Filter
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR
};

enum class AddressMode
{
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
};

struct SamplerInfo
{
    Filter mag_filter; //GL_TEXTURE_MAG_FILTER, GL_NEAREST, GL_LINEAR
    Filter min_filter; //GL_TEXTURE_MIN_FILTER, GL_NEAREST, GL_LINEAR, GNMN, GNML, GLMN, GLML
    Filter mipmap_mode;
    AddressMode address_mode_u; //GL_TEXTURE_WRAP_S
    AddressMode address_mode_v; //GL_TEXTURE_WRAP_T
    AddressMode address_mode_w; //GL_TEXTURE_WRAP_R
    float mip_lod_bias;
    bool anisotropy_enable;
    float max_anisotropy;
    bool compare_enable;
    CompareOp compare_op; //GL_TEXTURE_COMPARE_MODE -> GL_COMPARE_REF_TO_TEXTURE
    float min_lod; //GL_TEXTURE_MIN_LOD
    float max_lod; //GL_TEXTURE_MAX_LOD

#pragma message( \
    "if we use GL_CLAMP_TO_BORDER we can set GL_TEXTURE_BORDER_COLOR but we don't use it now...!")
};

class Context;

class Sampler : hrs::non_copyable
{
public:
    Sampler() noexcept;
    Sampler(Context* _parent, const SamplerInfo& info);
    ~Sampler();
    Sampler(Sampler&& sampler) noexcept;
    Sampler& operator=(Sampler&& sampler) noexcept;

    void BindSampler(GLuint index) const noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
};