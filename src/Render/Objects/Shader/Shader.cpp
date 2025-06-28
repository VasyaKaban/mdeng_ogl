#include "Shader.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "../../../hrs/scoped_call.hpp"

Shader::Shader() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE)
{}

Shader::Shader(Context* _parent, const ShaderInfo& info)
    : parent(_parent)
{
    GLHandle _handle = parent->GetLoader().CreateShader(static_cast<GLenum>(info.stage));
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create shader");

    hrs::scoped_call cleanup(
        [&_handle, _parent]()
        {
            _parent->GetLoader().DeleteShader(_handle);
        });

    parent->GetLoader().ShaderSource(_handle, 1, &info.code, &info.code_size);
    parent->GetLoader().CompileShader(_handle);

    GLint status;
    parent->GetLoader().GetShaderiv(_handle, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        GLint log_length = 0;
        parent->GetLoader().GetShaderiv(_handle, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(log_length, '\0');
        parent->GetLoader().GetShaderInfoLog(_handle, log_length, nullptr, log.data());

        throw std::runtime_error(std::move(log));
    }

    cleanup.drop();

    handle = _handle;
}

Shader::~Shader()
{
    destroy();
}

Shader::Shader(Shader&& shader) noexcept
    : parent(shader.parent),
      handle(std::exchange(shader.handle, GL_NULL_HANDLE))
{}

Shader& Shader::operator=(Shader&& shader) noexcept
{
    destroy();

    parent = shader.parent;
    handle = std::exchange(shader.handle, GL_NULL_HANDLE);

    return *this;
}

bool Shader::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

void Shader::destroy() noexcept
{
    if(IsCreated())
        parent->GetLoader().DeleteShader(handle);
}
