#pragma once

#include <unordered_map>
#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"

namespace OpenGL
{
    class GraphicsPipelineVertexInputState : Core::NonCopyable
    {
    public:
        GraphicsPipelineVertexInputState(const Pipeline& parent,
                                         const Render::GraphicsPipelineVertexInputStateInfo& info);
        GraphicsPipelineVertexInputState() = default;
        ~GraphicsPipelineVertexInputState() = default;
        GraphicsPipelineVertexInputState(GraphicsPipelineVertexInputState&&) = default;
        GraphicsPipelineVertexInputState& operator=(GraphicsPipelineVertexInputState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;

        GLHandle GetHandle() const noexcept;
        GLsizei GetStride(GLuint binding) const;
    private:
        using BindingStridesMap = std::unordered_map<GLuint, GLsizei>; //binding -> stride
        GLHandle vao;
        BindingStridesMap binding_strides_map;
    };

    class GraphicsPipelineInputAssemblyState : Core::NonCopyable
    {
    public:
        GraphicsPipelineInputAssemblyState(
            const Render::GraphicsPipelineInputAssemblyStateInfo& info);
        GraphicsPipelineInputAssemblyState() = default;
        ~GraphicsPipelineInputAssemblyState() = default;
        GraphicsPipelineInputAssemblyState(GraphicsPipelineInputAssemblyState&&) = default;
        GraphicsPipelineInputAssemblyState&
        operator=(GraphicsPipelineInputAssemblyState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;

        GLenum GetTopology() const noexcept;
    private:
        GLenum topology; //used directly by Context!
        bool primitive_restart_enabled;
    };

    class GraphicsPipelineTessellationState : Core::NonCopyable
    {
    public:
        GraphicsPipelineTessellationState(const Render::GraphicsPipelineTessellationStateInfo& info,
                                          bool _enabled);
        GraphicsPipelineTessellationState() = default;
        ~GraphicsPipelineTessellationState() = default;
        GraphicsPipelineTessellationState(GraphicsPipelineTessellationState&&) = default;
        GraphicsPipelineTessellationState& operator=(GraphicsPipelineTessellationState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool enabled;
        std::uint32_t patch_control_points;
    };

    struct ColorBlendAttachmentStateNative
    {
        bool blend_enabled;
        GLenum src_rgb;
        GLenum eq_rgb;
        GLenum dst_rgb;
        GLenum src_alpha;
        GLenum eq_alpha;
        GLenum dst_alpha;
        Render::ColorComponentFlags color_write_mask;
    };

    class GraphicsPipelineColorBlendState : Core::NonCopyable
    {
    public:
        GraphicsPipelineColorBlendState(const Render::GraphicsPipelineColorBlendStateInfo& info);
        GraphicsPipelineColorBlendState() = default;
        ~GraphicsPipelineColorBlendState() = default;
        GraphicsPipelineColorBlendState(GraphicsPipelineColorBlendState&&) = default;
        GraphicsPipelineColorBlendState& operator=(GraphicsPipelineColorBlendState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool logic_op_enabled;
        GLenum logic_op;
        std::vector<ColorBlendAttachmentStateNative> attachments;
        std::array<GLfloat, 4> blend_color;
    };

    struct StencilStateOpNative
    {
        GLenum fail_op;
        GLenum pass_op;
        GLenum depth_fail_op;
        GLenum compare_op;
        std::int32_t reference;
        std::uint32_t compare_mask;
        std::uint32_t write_mask;
    };

    class GraphicsPipelineDepthStencilState : Core::NonCopyable
    {
    public:
        GraphicsPipelineDepthStencilState(
            const Render::GraphicsPipelineDepthStencilStateInfo& info);
        GraphicsPipelineDepthStencilState() = default;
        ~GraphicsPipelineDepthStencilState() = default;
        GraphicsPipelineDepthStencilState(GraphicsPipelineDepthStencilState&&) = default;
        GraphicsPipelineDepthStencilState& operator=(GraphicsPipelineDepthStencilState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool depth_test_enabled;
        bool depth_write_enabled;
        GLenum depth_compare_op;
        bool stencil_test_enabled;
        StencilStateOpNative stencil_front_op;
        StencilStateOpNative stencil_back_op;

        bool depth_bounds_test_enabled;
        float min_depth_bounds;
        float max_depth_bounds;
    };

    class GraphicsPipelineMultisampleState : Core::NonCopyable
    {
    public:
        GraphicsPipelineMultisampleState(const Render::GraphicsPipelineMultisampleStateInfo& info);
        GraphicsPipelineMultisampleState() = default;
        ~GraphicsPipelineMultisampleState() = default;
        GraphicsPipelineMultisampleState(GraphicsPipelineMultisampleState&&) = default;
        GraphicsPipelineMultisampleState& operator=(GraphicsPipelineMultisampleState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        //is sample_count == 1 then we should set other parametrs because they use other Enable operations!
        // GL_MULTISAMPLE must be set if any other boolean flags are also true!
        bool multisample_enabled;
        Render::SampleCount sample_count;
        bool sample_shading_enabled;
        GLfloat min_sample_shading;
        std::vector<GLbitfield> sample_mask; //NULL, 1[1 - 32], 2[64]
        bool alpha_to_coverage_enabled;
        bool alpha_to_one_enabled;
    };

    class GraphicsPipelineRasterizationState : Core::NonCopyable
    {
    public:
        GraphicsPipelineRasterizationState(
            const Render::GraphicsPipelineRasterizationStateInfo& info);
        GraphicsPipelineRasterizationState() = default;
        ~GraphicsPipelineRasterizationState() = default;
        GraphicsPipelineRasterizationState(GraphicsPipelineRasterizationState&&) = default;
        GraphicsPipelineRasterizationState&
        operator=(GraphicsPipelineRasterizationState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool depth_clamp_enabled;
        bool rasterizer_discard_enabled;
        GLenum polygon_mode;
        GLenum cull_mode;
        GLenum front_face;
        GLenum polygon_offset_mode;
        bool depth_bias_enabled;
        float depth_bias_constant_factor;
        float depth_bias_clamp;
        float depth_bias_slope_factor;
        GLfloat line_width;
    };

    class GraphicsPipelineViewportState : Core::NonCopyable
    {
    public:
        GraphicsPipelineViewportState(const Render::GraphicsPipelineViewportStateInfo& info,
                                      bool _dynamic_viewports,
                                      bool _dynamic_scissors);
        GraphicsPipelineViewportState() = default;
        ~GraphicsPipelineViewportState() = default;
        GraphicsPipelineViewportState(GraphicsPipelineViewportState&&) = default;
        GraphicsPipelineViewportState& operator=(GraphicsPipelineViewportState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool dynamic_viewports;
        bool dynamic_scissors;

        std::vector<Render::Viewport> viewports;
        std::vector<Render::Rect2D> scissors;
    };

    struct GraphicsPipelineDrawState : Core::NonCopyable
    {
        int index_size;
        GLenum index_type;
        std::uintptr_t index_buffer_offset;
    };

    class GraphicsPipelineState : Core::NonCopyable
    {
    public:
        GraphicsPipelineState() noexcept;
        GraphicsPipelineState(Pipeline& parent, const Render::GraphicsPipelineInfo& info);
        ~GraphicsPipelineState() = default;
        GraphicsPipelineState(GraphicsPipelineState&&) = default;
        GraphicsPipelineState& operator=(GraphicsPipelineState&&) = default;

        void Set(CommandBuffer& cmd, Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;

        GLHandle GetVertexInputStateHandle() const noexcept;
        GLsizei GetVertexInputStateStride(GLuint binding) const;
        GLenum GetInputAssemblyStateTopology() const noexcept;
        void SetIndexBufferState(Render::IndexType index_type,
                                 std::uintptr_t index_buffer_offset) noexcept;
        GLenum GetIndexBufferStateType() const noexcept;
        std::uintptr_t GetIndexBufferStateOffset(std::uint32_t first_index) const noexcept;
    private:
        GraphicsPipelineVertexInputState vertex_input_state;
        GraphicsPipelineInputAssemblyState input_assembly_state;
        GraphicsPipelineTessellationState tessellation_state;
        GraphicsPipelineColorBlendState color_blend_state;
        GraphicsPipelineDepthStencilState depth_stencil_state;
        GraphicsPipelineMultisampleState multisample_state;
        GraphicsPipelineRasterizationState rasterization_state;
        GraphicsPipelineViewportState viewport_state;

        GraphicsPipelineDrawState draw_state;
    };
};