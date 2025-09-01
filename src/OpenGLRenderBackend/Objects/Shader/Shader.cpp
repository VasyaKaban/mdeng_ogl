#include "Shader.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "hrs/scoped_call.hpp"

namespace OpenGL
{
    Shader::Shader(Context* _parent, const Render::ShaderInfo& info)
        : parent(_parent)
    {
        GLHandle _handle = parent->GetLoader().CreateShader(static_cast<GLenum>(info.stage));
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create shader");

        hrs::scoped_call cleanup(
            [&_handle, _parent]()
            {
                _parent->GetLoader().DeleteShader(_handle);
            });

        auto code_data = info.code.data();
        GLint code_size = info.code.size();
        parent->GetLoader().ShaderSource(_handle, 1, &code_data, &code_size);
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
        parent->GetLoader().DeleteShader(handle);
    }

    GLHandle Shader::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Shader::GetContext() const noexcept
    {
        return parent;
    }
};