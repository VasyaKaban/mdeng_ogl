#pragma once

#include <unordered_map>
#include "hrs/non_creatable.hpp"
#include "../../Render.h"

namespace OpenGL
{
    class GraphicsPipelineVertexInputState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineVertexInputState(const Pipeline& parent,
                                         const Render::GraphicsPipelineVertexInputStateInfo& info);
        GraphicsPipelineVertexInputState() = default;
        ~GraphicsPipelineVertexInputState() = default;
        GraphicsPipelineVertexInputState(GraphicsPipelineVertexInputState&&) = default;
        GraphicsPipelineVertexInputState& operator=(GraphicsPipelineVertexInputState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        using BindingStridesMap = std::unordered_map<GLuint, GLsizei>; //binding -> stride
        GLHandle vao;
        BindingStridesMap binding_strides_map;
    };

    class GraphicsPipelineInputAssemblyState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineInputAssemblyState(
            const Render::GraphicsPipelineInputAssemblyStateInfo& info);
        GraphicsPipelineInputAssemblyState() = default;
        ~GraphicsPipelineInputAssemblyState() = default;
        GraphicsPipelineInputAssemblyState(GraphicsPipelineInputAssemblyState&&) = default;
        GraphicsPipelineInputAssemblyState&
        operator=(GraphicsPipelineInputAssemblyState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        GLenum topology; //used directly by Context!
        bool primitive_restart_enabled;
    };

    struct BlendFunctionInfoNative
    {
        std::uint32_t buffer_index;
        GLenum src_rgb;
        GLenum src_alpha;
        GLenum dst_rgb;
        GLenum dst_alpha;
    };

    struct BlendEquationInfoNative
    {
        std::uint32_t buffer_index;
        GLenum eq_rgb;
        GLenum eq_alpha;
    };

    class GraphicsPipelineBlendState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineBlendState(const Render::GraphicsPipelineBlendStateInfo& info);
        GraphicsPipelineBlendState() = default;
        ~GraphicsPipelineBlendState() = default;
        GraphicsPipelineBlendState(GraphicsPipelineBlendState&&) = default;
        GraphicsPipelineBlendState& operator=(GraphicsPipelineBlendState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool blend_enabled;
        std::vector<BlendFunctionInfoNative> blend_function_info;
        bool logic_op_enabled;
        GLenum logic_op;
        std::array<GLfloat, 4> blend_color;
        std::vector<BlendEquationInfoNative> blend_eq_info;
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

    StencilStateOpNative stencil_state_op_to_native(const Render::StencilStateOp& op) noexcept;

    class GraphicsPipelineDepthStencilState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineDepthStencilState(
            const Render::GraphicsPipelineDepthStencilStateInfo& info);
        GraphicsPipelineDepthStencilState() = default;
        ~GraphicsPipelineDepthStencilState() = default;
        GraphicsPipelineDepthStencilState(GraphicsPipelineDepthStencilState&&) = default;
        GraphicsPipelineDepthStencilState& operator=(GraphicsPipelineDepthStencilState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool depth_test_enabled;
        GLenum depth_compare_op;
        bool write_enabled;
        bool stencil_test_enabled;
        StencilStateOpNative stencil_front_op;
        StencilStateOpNative stencil_back_op;
    };

    class GraphicsPipelineMultisampleState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineMultisampleState(const Render::GraphicsPipelineMultisampleStateInfo& info);
        GraphicsPipelineMultisampleState() = default;
        ~GraphicsPipelineMultisampleState() = default;
        GraphicsPipelineMultisampleState(GraphicsPipelineMultisampleState&&) = default;
        GraphicsPipelineMultisampleState& operator=(GraphicsPipelineMultisampleState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool multisample_enabled;
        Render::SampleCount sample_count;
        std::vector<GLbitfield> sample_mask; //NULL, 1[1 - 32], 2[64]
        bool sample_shading_enabled;
        GLfloat min_sample_shading;
        bool alpha_to_coverage_enabled;
        bool alpha_to_one_enabled;
    };

    class GraphicsPipelineRasterizationState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineRasterizationState(
            const Render::GraphicsPipelineRasterizationStateInfo& info);
        GraphicsPipelineRasterizationState() = default;
        ~GraphicsPipelineRasterizationState() = default;
        GraphicsPipelineRasterizationState(GraphicsPipelineRasterizationState&&) = default;
        GraphicsPipelineRasterizationState&
        operator=(GraphicsPipelineRasterizationState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool depth_clamp_enabled;
        bool rasterizer_discard_enabled;
        GLenum polygon_mode;
        GLenum cull_mode;
        GLenum front_face;
        GLfloat line_width;
    };

    class GraphicsPipelineViewportState : hrs::non_copyable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineViewportState(const Render::GraphicsPipelineViewportStateInfo& info);
        GraphicsPipelineViewportState() = default;
        ~GraphicsPipelineViewportState() = default;
        GraphicsPipelineViewportState(GraphicsPipelineViewportState&&) = default;
        GraphicsPipelineViewportState& operator=(GraphicsPipelineViewportState&&) = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        bool viewport_enabled;
        std::vector<Render::Viewport> viewports;
        std::vector<Render::Rect2D> scissors;
    };

    struct GraphicsPipelineDrawState : hrs::non_copyable
    {
        friend class Pipeline;

        int index_size;
        GLenum index_type;
        std::uintptr_t index_buffer_offset;
    };

    class GraphicsPipelineState : hrs::non_copyable, hrs::non_movable
    {
        friend class Pipeline;
    public:
        GraphicsPipelineState(Pipeline& parent, const Render::GraphicsPipelineStateInfo& info);
        ~GraphicsPipelineState() = default;

        void Set(Pipeline& parent) noexcept;
        void Destroy(Pipeline& parent) noexcept;
    private:
        GraphicsPipelineVertexInputState vertex_input_state;
        GraphicsPipelineInputAssemblyState input_assembly_state;
        GraphicsPipelineBlendState blend_state;
        GraphicsPipelineDepthStencilState depth_stencil_state;
        GraphicsPipelineMultisampleState multisample_state;
        GraphicsPipelineRasterizationState rasterization_state;
        GraphicsPipelineViewportState viewport_state;

        GraphicsPipelineDrawState draw_state;
    };
};