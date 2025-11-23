#include "Pipeline.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "hrs/scoped_call.hpp"
#include "../Shader/Shader.h"
#include "../Buffer/Buffer.h"
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

        state = new GraphicsPipelineState(*this, info);

        cleanup.drop();

        handle = _handle;
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
        state = nullptr;
    }

    Pipeline::~Pipeline()
    {
        if(state)
        {
            state->Destroy(*this);
            delete state;
        }

        parent->GetLoader().DeleteProgram(handle);
    }

    GLHandle Pipeline::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* Pipeline::GetContext() const noexcept
    {
        return parent;
    }

    void Pipeline::Bind(CommandBuffer& cmd)
    {
        parent->GetLoader().UseProgram(handle);

        if(state)
            state->Set(cmd, *this);
    }

    void Pipeline::BindVertexBuffer(CommandBuffer& cmd,
                                    Buffer& buffer,
                                    std::uint32_t binding,
                                    std::int64_t offset)
    {
        parent->GetLoader().VertexArrayVertexBuffer(state->GetVertexInputStateHandle(),
                                                    binding,
                                                    buffer.GetHandle(),
                                                    offset,
                                                    state->GetVertexInputStateStride(binding));
    }

    void Pipeline::BindIndexBuffer(CommandBuffer& cmd,
                                   Buffer& buffer,
                                   Render::IndexType type,
                                   std::uintptr_t offset)
    {
        parent->GetLoader().VertexArrayElementBuffer(state->GetVertexInputStateHandle(),
                                                     buffer.GetHandle());

        state->SetIndexBufferState(type, offset);
    }

    void Pipeline::Draw(CommandBuffer& cmd,
                        std::uint32_t vertex_count,
                        std::uint32_t instance_count,
                        std::uint32_t first_vertex,
                        std::uint32_t first_instance)
    {
        parent->GetLoader().DrawArraysInstancedBaseInstance(state->GetInputAssemblyStateTopology(),
                                                            first_vertex,
                                                            vertex_count,
                                                            instance_count,
                                                            first_instance);
    }

    void Pipeline::DrawIndexed(CommandBuffer& cmd,
                               std::uint32_t index_count,
                               std::uint32_t instance_count,
                               std::uint32_t first_index,
                               std::int32_t vertex_offset,
                               std::uint32_t first_instance)
    {
        parent->GetLoader().DrawElementsInstancedBaseVertexBaseInstance(
            state->GetInputAssemblyStateTopology(),
            index_count,
            state->GetIndexBufferStateType(),
            reinterpret_cast<const void*>(state->GetIndexBufferStateOffset(first_index)),
            instance_count,
            vertex_offset,
            first_instance);
    }

    void Pipeline::DrawIndirect(CommandBuffer& cmd,
                                std::uint64_t offset,
                                std::uint32_t draw_count,
                                std::uint32_t stride)
    {
        parent->GetLoader().MultiDrawArraysIndirect(state->GetInputAssemblyStateTopology(),
                                                    reinterpret_cast<const void*>(offset),
                                                    draw_count,
                                                    stride);
    }

    void Pipeline::DrawIndexedIndirect(CommandBuffer& cmd,
                                       std::uint64_t offset,
                                       std::uint32_t draw_count,
                                       std::uint32_t stride)
    {
        parent->GetLoader().MultiDrawElementsIndirect(state->GetInputAssemblyStateTopology(),
                                                      state->GetIndexBufferStateType(),
                                                      reinterpret_cast<const void*>(offset),
                                                      draw_count,
                                                      stride);
    }
};