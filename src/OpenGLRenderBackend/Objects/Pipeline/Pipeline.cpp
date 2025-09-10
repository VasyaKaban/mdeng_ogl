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

    void Pipeline::Bind(const Render::CommandBuffer* cmd)
    {
        parent->GetLoader().UseProgram(handle);
        if(graphics_state)
            graphics_state->Set(*this);
    }

    void Pipeline::BindVertexBuffer(const Render::CommandBuffer* cmd,
                                    const Render::Buffer* buffer,
                                    std::uint32_t binding,
                                    std::int64_t offset)
    {
        parent->GetLoader().VertexArrayVertexBuffer(
            graphics_state->vertex_input_state.vao,
            binding,
            static_cast<const Buffer*>(buffer)->GetHandle(),
            offset,
            graphics_state->vertex_input_state.binding_strides_map[binding]);
    }

    void Pipeline::BindIndexBuffer(const Render::CommandBuffer* cmd,
                                   const Render::Buffer* buffer,
                                   Render::IndexType type,
                                   std::uintptr_t offset)
    {
        parent->GetLoader().VertexArrayElementBuffer(
            graphics_state->vertex_input_state.vao,
            static_cast<const Buffer*>(buffer)->GetHandle());

        graphics_state->draw_state.index_type = IndexTypeToNative(type);
        switch(type)
        {
            case Render::IndexType::u8:
                graphics_state->draw_state.index_size = 1;
                break;
            case Render::IndexType::u16:
                graphics_state->draw_state.index_size = 2;
                break;
            case Render::IndexType::u32:
                graphics_state->draw_state.index_size = 4;
                break;
        }

        graphics_state->draw_state.index_buffer_offset = offset;
    }

    void Pipeline::BindUniformBuffer(const Render::CommandBuffer* cmd,
                                     const Render::Buffer* buffer,
                                     const Render::BufferBindDesc& bind_desc) noexcept
    {
        parent->GetLoader().BindBufferRange(GL_UNIFORM_BUFFER,
                                            bind_desc.index,
                                            static_cast<const Buffer*>(buffer)->GetHandle(),
                                            bind_desc.offset,
                                            bind_desc.size);
    }

    void Pipeline::BindShaderStorageBuffer(const Render::CommandBuffer* cmd,
                                           const Render::Buffer* buffer,
                                           const Render::BufferBindDesc& bind_desc) noexcept
    {
        parent->GetLoader().BindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                            bind_desc.index,
                                            static_cast<const Buffer*>(buffer)->GetHandle(),
                                            bind_desc.offset,
                                            bind_desc.size);
    }

    void Pipeline::BindImageView(const Render::CommandBuffer* cmd,
                                 const Render::ImageView* view,
                                 std::uint32_t index) noexcept
    {
        parent->GetLoader().BindTextureUnit(index,
                                            static_cast<const ImageView*>(view)->GetHandle());
    }

    void Pipeline::BindSampler(const Render::CommandBuffer* cmd,
                               const Render::Sampler* sampler,
                               std::uint32_t index) noexcept
    {
        parent->GetLoader().BindSampler(index, static_cast<const Sampler*>(sampler)->GetHandle());
    }

    void Pipeline::Draw(const Render::CommandBuffer* cmd,
                        std::uint32_t vertex_count,
                        std::uint32_t instance_count,
                        std::uint32_t first_vertex,
                        std::uint32_t first_instance)
    {
        parent->GetLoader().DrawArraysInstancedBaseInstance(
            graphics_state->input_assembly_state.topology,
            first_vertex,
            vertex_count,
            instance_count,
            first_instance);
    }

    void Pipeline::DrawIndexed(const Render::CommandBuffer* cmd,
                               std::uint32_t index_count,
                               std::uint32_t instance_count,
                               std::uint32_t first_index,
                               std::int32_t vertex_offset,
                               std::uint32_t first_instance)
    {
        //indices = index_buffer_offset + sizoef(indexType) * first_index;

        parent->GetLoader().DrawElementsInstancedBaseVertexBaseInstance(
            graphics_state->input_assembly_state.topology,
            index_count,
            graphics_state->draw_state.index_type,
            reinterpret_cast<const void*>(graphics_state->draw_state.index_buffer_offset +
                                          graphics_state->draw_state.index_size * first_index),
            instance_count,
            vertex_offset,
            first_instance);
    }

    void Pipeline::SetViewport(const Render::CommandBuffer* cmd,
                               std::uint32_t first_viewport,
                               std::span<const Render::Viewport> viewports)
    {
        parent->GetLoader().ViewportArrayv(first_viewport, viewports.size(), &viewports.data()->x);
    }

    void Pipeline::SetScissor(const Render::CommandBuffer* cmd,
                              std::uint32_t first_scissor,
                              std::span<const Render::Rect2D> scissors)
    {
        parent->GetLoader().Enable(GL_SCISSOR_TEST);
        parent->GetLoader().ScissorArrayv(first_scissor,
                                          scissors.size(),
                                          &scissors.data()->offset.x);
    }

    void Pipeline::SetUniform(const Render::CommandBuffer* cmd,
                              const Render::UniformDesc& desc,
                              std::span<const std::byte> data)
    {
#define VECTOR_CASE(EXTENT, SIZE) \
    case EXTENT: \
        switch(desc.type) \
        { \
            case Render::UniformType::Float: \
            { \
                parent->GetLoader().Uniform##SIZE##fv( \
                    desc.location, \
                    data.size() / (sizeof(GLfloat) * SIZE), \
                    reinterpret_cast<const GLfloat*>(data.data())); \
            } \
            break; \
            case Render::UniformType::Int: \
            { \
                parent->GetLoader().Uniform##SIZE##iv( \
                    desc.location, \
                    data.size() / (sizeof(GLint) * SIZE), \
                    reinterpret_cast<const GLint*>(data.data())); \
            } \
            break; \
            case Render::UniformType::UInt: \
            { \
                parent->GetLoader().Uniform##SIZE##uiv( \
                    desc.location, \
                    data.size() / (sizeof(GLuint) * SIZE), \
                    reinterpret_cast<const GLuint*>(data.data())); \
            } \
            break; \
            case Render::UniformType::Double: \
            { \
                parent->GetLoader().Uniform##SIZE##dv( \
                    desc.location, \
                    data.size() / (sizeof(GLdouble) * SIZE), \
                    reinterpret_cast<const GLdouble*>(data.data())); \
            } \
            break; \
        } \
        break;

//__VA_ARGS__ -> COLS
#define MATRIX_CASE(EXTENT, ROWS, ...) \
    case EXTENT: \
        switch(desc.type) \
        { \
            case Render::UniformType::Float: \
            { \
                parent->GetLoader().UniformMatrix##ROWS##__VA_OPT__(x##__VA_ARGS__)##fv( \
                    desc.location, \
                    data.size() / (sizeof(GLfloat) * ROWS __VA_OPT__(*__VA_ARGS__)), \
                    GL_FALSE, \
                    reinterpret_cast<const GLfloat*>(data.data())); \
            } \
            break; \
            case Render::UniformType::Double: \
            { \
                parent->GetLoader().UniformMatrix##ROWS##__VA_OPT__(x##__VA_ARGS__)##dv( \
                    desc.location, \
                    data.size() / (sizeof(GLdouble) * ROWS __VA_OPT__(*__VA_ARGS__)), \
                    GL_FALSE, \
                    reinterpret_cast<const GLdouble*>(data.data())); \
            } \
            break; \
            default: \
                assert(false); \
                break; \
        } \
        break;

        if(desc.extent == Render::UniformExtent::Scalar ||
           desc.extent == Render::UniformExtent::Vec2 ||
           desc.extent == Render::UniformExtent::Vec3 || desc.extent == Render::UniformExtent::Vec4)
        {
            switch(desc.extent)
            {
                VECTOR_CASE(Render::UniformExtent::Scalar, 1)
                VECTOR_CASE(Render::UniformExtent::Vec2, 2)
                VECTOR_CASE(Render::UniformExtent::Vec3, 3)
                VECTOR_CASE(Render::UniformExtent::Vec4, 4)
                default:
                    assert(false);
                    break;
            }
        }
        else
        {
            switch(desc.extent)
            {
                MATRIX_CASE(Render::UniformExtent::Mat2x2, 2)
                MATRIX_CASE(Render::UniformExtent::Mat2x3, 2, 3)
                MATRIX_CASE(Render::UniformExtent::Mat2x4, 2, 4)
                MATRIX_CASE(Render::UniformExtent::Mat3x2, 3, 2)
                MATRIX_CASE(Render::UniformExtent::Mat3x3, 3)
                MATRIX_CASE(Render::UniformExtent::Mat3x4, 3, 4)
                MATRIX_CASE(Render::UniformExtent::Mat4x2, 4, 2)
                MATRIX_CASE(Render::UniformExtent::Mat4x3, 4, 3)
                MATRIX_CASE(Render::UniformExtent::Mat4x4, 4)
                default:
                    assert(false);
                    break;
            }
        }
    }

    Render::Context* Pipeline::GetContext() const noexcept
    {
        return parent;
    }
};