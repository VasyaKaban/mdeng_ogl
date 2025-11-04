#include "Pipeline.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "hrs/scoped_call.hpp"
#include "../Shader/Shader.h"
#include "../Buffer/Buffer.h"
#include "../ImageView/ImageView.h"
#include "../Sampler/Sampler.h"
#include <cassert>

namespace OpenGL
{
    Pipeline::Pipeline(Context* _parent, const Render::GraphicsPipelineInfo& info)
        : parent(_parent)
    {
        GLHandle _handle = parent->GetLoader().CreateProgram();
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create pipeline");

        hrs::scoped_call cleanup(
            [&_handle, _parent]()
            {
                _parent->GetLoader().DeleteProgram(_handle);
            });

        for(const auto& shader: info.shaders)
            parent->GetLoader().AttachShader(_handle,
                                             static_cast<const Shader*>(shader)->GetHandle());

        parent->GetLoader().LinkProgram(_handle);

        for(const auto& shader: info.shaders)
            parent->GetLoader().DetachShader(_handle,
                                             static_cast<const Shader*>(shader)->GetHandle());

        GLint link_status;
        parent->GetLoader().GetProgramiv(_handle, GL_LINK_STATUS, &link_status);
        if(link_status != GL_TRUE)
        {
            GLint log_length = 0;
            parent->GetLoader().GetProgramiv(_handle, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, '\0');
            parent->GetLoader().GetProgramInfoLog(_handle, log_length, nullptr, log.data());

            throw std::runtime_error(std::move(log));
        }

        GraphicsPipelineState* _graphics_state = new GraphicsPipelineState(*this, info.state_info);

        cleanup.drop();

        handle = _handle;
        graphics_state = _graphics_state;
    }

    Pipeline::Pipeline(Context* _parent, const Render::ComputePipelineInfo& info)
        : parent(_parent)
    {
        GLHandle _handle = parent->GetLoader().CreateProgram();
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create pipeline");

        hrs::scoped_call cleanup(
            [&_handle, _parent]()
            {
                _parent->GetLoader().DeleteProgram(_handle);
            });

        parent->GetLoader().AttachShader(_handle,
                                         static_cast<const Shader*>(info.shader)->GetHandle());

        parent->GetLoader().LinkProgram(_handle);

        parent->GetLoader().DetachShader(_handle,
                                         static_cast<const Shader*>(info.shader)->GetHandle());

        GLint link_status;
        parent->GetLoader().GetProgramiv(_handle, GL_LINK_STATUS, &link_status);
        if(link_status != GL_TRUE)
        {
            GLint log_length = 0;
            parent->GetLoader().GetProgramiv(_handle, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, '\0');
            parent->GetLoader().GetProgramInfoLog(_handle, log_length, nullptr, log.data());

            throw std::runtime_error(std::move(log));
        }

        cleanup.drop();

        handle = _handle;
        graphics_state = nullptr;
    }

    Pipeline::~Pipeline()
    {
        if(graphics_state)
        {
            graphics_state->Destroy(*this);
            delete graphics_state;
        }

        parent->GetLoader().DeleteProgram(handle);
    }

    GLHandle Pipeline::GetHandle() const noexcept
    {
        return handle;
    }

    GraphicsPipelineState* Pipeline::GetGraphicsPipelineState() noexcept
    {
        return graphics_state;
    }

    Render::Context* Pipeline::GetContext() const noexcept
    {
        return parent;
    }
};