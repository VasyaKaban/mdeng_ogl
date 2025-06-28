#pragma once

#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

class Context;

enum class ShaderStage
{
    Vertex = GL_VERTEX_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    TessellationControl = GL_TESS_CONTROL_SHADER,
    TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Compute = GL_COMPUTE_SHADER,
};

struct ShaderInfo
{
    ShaderStage stage;
    const GLchar* code;
    GLint code_size;
};

class Shader : hrs::non_copyable
{
    friend class Pipeline;
public:
    Shader() noexcept;
    Shader(Context* _parent, const ShaderInfo& info);
    ~Shader();
    Shader(Shader&& shader) noexcept;
    Shader& operator=(Shader&& shader) noexcept;

    bool IsCreated() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;
};