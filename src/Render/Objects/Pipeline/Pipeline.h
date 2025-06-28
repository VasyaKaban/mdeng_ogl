#pragma once

#include <span>
#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"
#include "GraphicsPipelineState.h"
#include "../../../hrs/math/vector_view.hpp"
#include "../../../hrs/math/matrix_view.hpp"

class Shader;

struct GraphicsPipelineInfo
{
    std::span<const Shader* const> shaders;
    GraphicsPipelineStateInfo state_info;
};

struct ComputePipelineInfo
{
    const Shader* shader;
};

class Context;
class Buffer;

class Pipeline : hrs::non_copyable
{
public:
    Pipeline() noexcept;
    Pipeline(Context* _parent, const GraphicsPipelineInfo& info);
    Pipeline(Context* _parent, const ComputePipelineInfo& info);
    ~Pipeline();
    Pipeline(Pipeline&& pipeline) noexcept;
    Pipeline& operator=(Pipeline&& pipeline) noexcept;

    void Bind();

    void BindVertexBuffer(const Buffer& buffer, GLuint binding, GLintptr offset);
    void BindIndexBuffer(const Buffer& buffer, IndexType type, std::uintptr_t offset);

    void
    Draw(GLsizei vertex_count, GLsizei instance_count, GLuint first_vertex, GLuint first_instance);

    void DrawIndexed(GLsizei index_count,
                     GLsizei instance_count,
                     GLsizei first_index,
                     GLint vertex_offset,
                     GLuint first_instance);

    //Dynamic state
    void SetViewport(GLuint first_viewport, std::span<const Viewport> viewports);
    void SetScissor(GLuint first_scissor, std::span<const Rect2D> scissors);

    //Uniforms
    void SetUniform(GLuint location, std::span<const GLfloat> v) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::vec2> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::vec3> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::vec4> vec) noexcept;

    void SetUniform(GLuint location, std::span<const GLint> v) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec2> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec3> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::ivec4> vec) noexcept;

    void SetUniform(GLuint location, std::span<const GLuint> v) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec2> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec3> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::uvec4> vec) noexcept;

    void SetUniform(GLuint location, std::span<const GLdouble> v) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec2> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec3> vec) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dvec4> vec) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat2x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat2x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat2x4> matrices) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat3x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat3x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat3x4> matrices) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat4x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat4x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::mat4x4> matrices) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat2x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat2x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat2x4> matrices) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat3x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat3x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat3x4> matrices) noexcept;

    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat4x2> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat4x3> matrices) noexcept;
    void SetUniform(GLuint location, std::span<const hrs::math::glsl::dmat4x4> matrices) noexcept;

    bool IsCreated() const noexcept;

    Context* GetContext() noexcept;
    const Context* GetContext() const noexcept;
private:
    void destroy() noexcept;
private:
    Context* parent;
    GLHandle handle;

    GraphicsPipelineState* graphics_state;
};