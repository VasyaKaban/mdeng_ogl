#include "Shader.h"
#include "../Device/Device.h"
#include <stdexcept>
#include "Core/Utils/ScopedCall.hpp"

namespace OpenGL
{
    Shader::Shader(Device* _parent, const Render::ShaderInfo& info)
        : parent(_parent),
          stage(info.stage)
    {
        if(info.syntax != Render::ShaderSyntaxFlagBits::GLSL)
            throw std::runtime_error("Bad shader syntax. Only GLSL is suppoerted");

        GLHandle _handle = parent->GetLoader().CreateShader(ShaderStageToNative(info.stage));
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create shader");

        Core::ScopedCall cleanup(
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

        cleanup.Drop();

        handle = _handle;
    }

    Shader::~Shader()
    {
        parent->GetLoader().DeleteShader(handle);
    }

    Render::Device* Shader::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle Shader::GetHandle() const noexcept
    {
        return handle;
    }

    Render::ShaderStageFlagBits Shader::GetStage() const noexcept
    {
        return stage;
    }
};