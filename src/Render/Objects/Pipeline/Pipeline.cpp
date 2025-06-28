#include "Pipeline.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include "../Shader/Shader.h"
#include "../../../hrs/scoped_call.hpp"
#include "../Buffer/Buffer.h"
#include "../../../hrs/math/vector.hpp"
#include "../../../hrs/math/matrix.hpp"

Pipeline::Pipeline() noexcept
    : parent(nullptr),
      handle(GL_NULL_HANDLE),
      graphics_state(nullptr)
{}

Pipeline::Pipeline(Context* _parent, const GraphicsPipelineInfo& info)
    : parent(_parent)
{
    GLHandle _handle = parent->GetLoader().CreateProgram();
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create pipeline");

    hrs::scoped_call cleanup(
        [&_handle, _parent]()
        {
            _parent->GetLoader().DeleteProgram(_handle);
        });

    for(const auto& shader: info.shaders)
        parent->GetLoader().AttachShader(_handle, shader->handle);

    parent->GetLoader().LinkProgram(_handle);

    for(const auto& shader: info.shaders)
        parent->GetLoader().DetachShader(_handle, shader->handle);

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

    GraphicsPipelineState* _graphics_state = new GraphicsPipelineState(this, info.state_info);

    cleanup.drop();

    handle = _handle;
    graphics_state = _graphics_state;
}

Pipeline::Pipeline(Context* _parent, const ComputePipelineInfo& info)
    : parent(_parent)
{
    GLHandle _handle = parent->GetLoader().CreateProgram();
    if(_handle == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create pipeline");

    hrs::scoped_call cleanup(
        [&_handle, _parent]()
        {
            _parent->GetLoader().DeleteProgram(_handle);
        });

    parent->GetLoader().AttachShader(_handle, info.shader->handle);

    parent->GetLoader().LinkProgram(_handle);

    parent->GetLoader().DetachShader(_handle, info.shader->handle);

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
    destroy();
}

Pipeline::Pipeline(Pipeline&& pipeline) noexcept
    : parent(pipeline.parent),
      handle(std::exchange(pipeline.handle, GL_NULL_HANDLE)),
      graphics_state(std::exchange(pipeline.graphics_state, nullptr))
{}

Pipeline& Pipeline::operator=(Pipeline&& pipeline) noexcept
{
    destroy();

    parent = pipeline.parent;
    handle = std::exchange(pipeline.handle, GL_NULL_HANDLE);
    graphics_state = std::exchange(pipeline.graphics_state, nullptr);

    return *this;
}

void Pipeline::Bind()
{
    parent->GetLoader().UseProgram(handle);
    if(graphics_state)
        graphics_state->Set(*this);
}

void Pipeline::BindVertexBuffer(const Buffer& buffer, GLuint binding, GLintptr offset)
{
    parent->GetLoader().VertexArrayVertexBuffer(
        graphics_state->vertex_input_state.vao,
        binding,
        buffer.handle,
        offset,
        graphics_state->vertex_input_state.binding_strides_map[binding]);
}

void Pipeline::BindIndexBuffer(const Buffer& buffer, IndexType type, std::uintptr_t offset)
{
    parent->GetLoader().VertexArrayElementBuffer(graphics_state->vertex_input_state.vao,
                                                 buffer.handle);

    graphics_state->draw_state.index_type = type;
    graphics_state->draw_state.index_buffer_offset = offset;
}

void Pipeline::Draw(GLsizei vertex_count,
                    GLsizei instance_count,
                    GLuint first_vertex,
                    GLuint first_instance)
{
    parent->GetLoader().DrawArraysInstancedBaseInstance(
        static_cast<GLenum>(graphics_state->input_assembly_state.topology),
        first_vertex,
        vertex_count,
        instance_count,
        first_instance);
}

void Pipeline::DrawIndexed(GLsizei index_count,
                           GLsizei instance_count,
                           GLsizei first_index,
                           GLint vertex_offset,
                           GLuint first_instance)
{
    //indices = index_buffer_offset + sizoef(indexType) * first_index;

    int index_size;
    switch(graphics_state->draw_state.index_type)
    {
        case IndexType::u8:
            index_size = 1;
            break;
        case IndexType::u16:
            index_size = 2;
            break;
        case IndexType::u32:
            index_size = 4;
            break;
    }

    parent->GetLoader().DrawElementsInstancedBaseVertexBaseInstance(
        static_cast<GLenum>(graphics_state->input_assembly_state.topology),
        index_count,
        static_cast<GLenum>(graphics_state->draw_state.index_type),
        reinterpret_cast<const void*>(graphics_state->draw_state.index_buffer_offset +
                                      index_size * first_index),
        instance_count,
        vertex_offset,
        first_instance);
}

void Pipeline::SetViewport(GLuint first_viewport, std::span<const Viewport> viewports)
{
    parent->GetLoader().ViewportArrayv(first_viewport, viewports.size(), &viewports.data()->x);
}

void Pipeline::SetScissor(GLuint first_scissor, std::span<const Rect2D> scissors)
{
    parent->GetLoader().Enable(GL_SCISSOR_TEST);
    parent->GetLoader().ScissorArrayv(first_scissor, scissors.size(), &scissors.data()->offset.x);
}

void Pipeline::SetUniform(GLuint location, std::span<const GLfloat> v) noexcept
{
    parent->GetLoader().Uniform1fv(location, v.size(), v.data());
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::vec2> vec) noexcept
{
    parent->GetLoader().Uniform2fv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::vec3> vec) noexcept
{
    parent->GetLoader().Uniform3fv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::vec4> vec) noexcept
{
    parent->GetLoader().Uniform4fv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const GLint> v) noexcept
{
    parent->GetLoader().Uniform1iv(location, v.size(), v.data());
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec2> vec) noexcept
{
    parent->GetLoader().Uniform2iv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec3> vec) noexcept
{
    parent->GetLoader().Uniform3iv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec4> vec) noexcept
{
    parent->GetLoader().Uniform4iv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const GLuint> v) noexcept
{
    parent->GetLoader().Uniform1uiv(location, v.size(), v.data());
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec2> vec) noexcept
{
    parent->GetLoader().Uniform2uiv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec3> vec) noexcept
{
    parent->GetLoader().Uniform3uiv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec4> vec) noexcept
{
    parent->GetLoader().Uniform4uiv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const GLdouble> v) noexcept
{
    parent->GetLoader().Uniform1dv(location, v.size(), v.data());
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec2> vec) noexcept
{
    parent->GetLoader().Uniform2dv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec3> vec) noexcept
{
    parent->GetLoader().Uniform3dv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec4> vec) noexcept
{
    parent->GetLoader().Uniform4dv(location, vec.size(), vec.data()->data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat2x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2fv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat2x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2x3fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat2x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2x4fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat3x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3x2fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat3x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3fv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat3x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3x4fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat4x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4x2fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat4x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4x3fv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::mat4x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4fv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat2x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2dv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat2x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2x3dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat2x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix2x4dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat3x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3x2dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat3x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3dv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat3x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix3x4dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat4x2> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4x2dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat4x3> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4x3dv(location,
                                           matrices.size(),
                                           GL_FALSE,
                                           matrices.data()->data[0].data);
}

void Pipeline::SetUniform(GLuint location,
                          std::span<const hrs::math::glsl::dmat4x4> matrices) noexcept
{
    parent->GetLoader().UniformMatrix4dv(location,
                                         matrices.size(),
                                         GL_FALSE,
                                         matrices.data()->data[0].data);
}

bool Pipeline::IsCreated() const noexcept
{
    return handle != GL_NULL_HANDLE;
}

Context* Pipeline::GetContext() noexcept
{
    return parent;
}

const Context* Pipeline::GetContext() const noexcept
{
    return parent;
}

void Pipeline::destroy() noexcept
{
    if(IsCreated())
    {
        if(graphics_state)
        {
            graphics_state->Destroy(*this);
            delete graphics_state;
        }

        parent->GetLoader().DeleteProgram(handle);
    }
}