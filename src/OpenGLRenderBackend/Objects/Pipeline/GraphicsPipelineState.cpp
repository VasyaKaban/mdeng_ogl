#include "GraphicsPipelineState.h"
#include "Pipeline.h"
#include "../../Context/Context.h"
#include "../Shader/Shader.h"
#include "../Buffer/Buffer.h"
#include <stdexcept>
#include <format>
#include "hrs/scoped_call.hpp"

namespace OpenGL
{
    static const GladGLContext& get_loader(const Pipeline& pipeline) noexcept
    {
        return static_cast<const Context*>(pipeline.GetContext())->GetLoader();
    }

    GraphicsPipelineVertexInputState::GraphicsPipelineVertexInputState(
        const Pipeline& parent,
        const Render::GraphicsPipelineVertexInputStateInfo& info)
    {
        const auto& loader = get_loader(parent);

        GLHandle _vao = OGL_NULL_HANDLE;
        BindingStridesMap _binding_strides_map;

        loader.CreateVertexArrays(1, &_vao);
        if(_vao == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to create vertex array");

        for(const auto& binding: info.vertex_input_bindings)
        {
            loader.VertexArrayBindingDivisor(_vao,
                                             binding.binding,
                                             InputRateToNative(binding.input_rate));

            _binding_strides_map.insert({binding.binding, binding.stride});
        }

        for(auto& attr: info.vertex_input_attributes)
        {
            const auto& vertex_input_type_size_pair = DecodeVertexInputTypeSizePair(attr.format);
            loader.EnableVertexArrayAttrib(_vao, attr.location);
            loader.VertexArrayAttribFormat(_vao,
                                           attr.location,
                                           vertex_input_type_size_pair.size,
                                           vertex_input_type_size_pair.type,
                                           vertex_input_type_size_pair.normalized,
                                           attr.offset);

            loader.VertexArrayAttribBinding(_vao, attr.location, attr.binding);
        }

        vao = _vao;
        binding_strides_map = std::move(_binding_strides_map);
    }

    void GraphicsPipelineVertexInputState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        get_loader(parent).BindVertexArray(vao);
    }

    void GraphicsPipelineVertexInputState::Destroy(Pipeline& parent) noexcept
    {
        get_loader(parent).DeleteVertexArrays(1, &vao);

        vao = OGL_NULL_HANDLE;
        binding_strides_map.clear();
    }

    GLHandle GraphicsPipelineVertexInputState::GetHandle() const noexcept
    {
        return vao;
    }

    GLsizei GraphicsPipelineVertexInputState::GetStride(GLuint binding) const
    {
        return binding_strides_map.find(binding)->second;
    }

    GraphicsPipelineInputAssemblyState::GraphicsPipelineInputAssemblyState(
        const Render::GraphicsPipelineInputAssemblyStateInfo& info)
        : topology(PrimitiveTopologyToNative(info.topology)),
          primitive_restart_enabled(info.primitive_restart_enabled)
    {}

    void GraphicsPipelineInputAssemblyState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);
        if(!primitive_restart_enabled)
        {
            loader.Disable(GL_PRIMITIVE_RESTART);
            loader.Disable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        }
        else
        {
            loader.Enable(GL_PRIMITIVE_RESTART);
            loader.Enable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        }
    }

    void GraphicsPipelineInputAssemblyState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GLenum GraphicsPipelineInputAssemblyState::GetTopology() const noexcept
    {
        return topology;
    }

    GraphicsPipelineTessellationState::GraphicsPipelineTessellationState(
        const Render::GraphicsPipelineTessellationStateInfo& info,
        bool _enabled)
        : enabled(_enabled),
          patch_control_points(info.patch_control_points)
    {}

    void GraphicsPipelineTessellationState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(enabled)
            loader.PatchParameteri(GL_PATCH_VERTICES, patch_control_points);
    }

    void GraphicsPipelineTessellationState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineColorBlendState::GraphicsPipelineColorBlendState(
        const Render::GraphicsPipelineColorBlendStateInfo& info)
        : logic_op_enabled(info.logic_op_enabled),
          logic_op(BlendLogicOpToNative(info.logic_op)),
          blend_color(
              {info.blend_color[0], info.blend_color[1], info.blend_color[2], info.blend_color[3]})
    {
        attachments.reserve(info.attachments.size());
        for(const auto& att: info.attachments)
        {
            attachments.push_back(ColorBlendAttachmentStateNative{
                .blend_enabled = att.blend_enabled,
                .src_rgb = BlendFactorToNative(att.src_color_blend_factor),
                .eq_rgb = BlendOpToNative(att.color_blend_op),
                .dst_rgb = BlendFactorToNative(att.dst_color_blend_factor),
                .src_alpha = BlendFactorToNative(att.src_alpha_blend_factor),
                .eq_alpha = BlendOpToNative(att.alpha_blend_op),
                .dst_alpha = BlendFactorToNative(att.dst_alpha_blend_factor),
                .color_write_mask = att.color_write_mask});
        }
    }

    void GraphicsPipelineColorBlendState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(!logic_op_enabled)
            loader.Disable(GL_COLOR_LOGIC_OP);
        else
        {
            loader.Enable(GL_COLOR_LOGIC_OP);
            loader.LogicOp(logic_op);
        }

        loader.BlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);

        for(std::size_t i = 0; i < attachments.size(); i++)
        {
            if(!attachments[i].blend_enabled)
                loader.Disablei(GL_BLEND, i);
            else
            {
                loader.Enablei(GL_BLEND, i);
                loader.BlendFuncSeparatei(i,
                                          attachments[i].src_rgb,
                                          attachments[i].dst_rgb,
                                          attachments[i].src_alpha,
                                          attachments[i].dst_alpha);

                loader.BlendEquationSeparatei(i, attachments[i].eq_rgb, attachments[i].eq_alpha);
                loader.ColorMaski(i,
                                  attachments[i].color_write_mask &
                                      Render::ColorComponentFlagBits::ColorComponentRed,
                                  attachments[i].color_write_mask &
                                      Render::ColorComponentFlagBits::ColorComponentGreen,
                                  attachments[i].color_write_mask &
                                      Render::ColorComponentFlagBits::ColorComponentBlue,
                                  attachments[i].color_write_mask &
                                      Render::ColorComponentFlagBits::ColorComponentAlpha);
            }
        }
    }

    void GraphicsPipelineColorBlendState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    static StencilStateOpNative
    stencil_state_op_to_native(const Render::StencilStateOp& op) noexcept
    {
        return StencilStateOpNative{.fail_op = StencilOpToNative(op.fail_op),
                                    .pass_op = StencilOpToNative(op.pass_op),
                                    .depth_fail_op = StencilOpToNative(op.depth_fail_op),
                                    .compare_op = ComapreOpToNative(op.compare_op),
                                    .reference = op.reference,
                                    .compare_mask = op.compare_mask,
                                    .write_mask = op.write_mask};
    }

    GraphicsPipelineDepthStencilState::GraphicsPipelineDepthStencilState(
        const Render::GraphicsPipelineDepthStencilStateInfo& info)
        : depth_test_enabled(info.depth_test_enabled),
          depth_write_enabled(info.depth_write_enabled),
          depth_compare_op(ComapreOpToNative(info.depth_compare_op)),
          stencil_test_enabled(info.stencil_test_enabled),
          stencil_front_op(stencil_state_op_to_native(info.stencil_front_op)),
          stencil_back_op(stencil_state_op_to_native(info.stencil_back_op)),
          depth_bounds_test_enabled(info.depth_bounds_test_enabled),
          min_depth_bounds(info.min_depth_bounds),
          max_depth_bounds(info.min_depth_bounds)
    {}

    static void
    stencil_state_op_set(Pipeline& parent, GLenum face, const StencilStateOpNative& state)
    {
        const auto& loader = get_loader(parent);
        loader.StencilFuncSeparate(face, state.compare_op, state.reference, state.compare_mask);
        loader.StencilMaskSeparate(face, state.write_mask);
        loader.StencilOpSeparate(face, state.fail_op, state.depth_fail_op, state.pass_op);
    }

    void GraphicsPipelineDepthStencilState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);
        if(!depth_test_enabled)
            loader.Disable(GL_DEPTH_TEST);
        else
        {
            loader.Enable(GL_DEPTH_TEST);
            if(depth_write_enabled)
                loader.DepthMask(GL_TRUE);
            else
                loader.DepthMask(GL_FALSE);

            loader.DepthFunc(depth_compare_op);
        }

        if(!stencil_test_enabled)
            loader.Disable(GL_STENCIL_TEST);
        else
        {
            loader.Enable(GL_STENCIL_TEST);
            stencil_state_op_set(parent, GL_FRONT, stencil_front_op);
            stencil_state_op_set(parent, GL_BACK, stencil_back_op);
        }

        if(parent.GetContext()->GetProperties().features.depth_bounds)
        {
            if(!depth_bounds_test_enabled)
                loader.Disable(GL_DEPTH_BOUNDS_TEST);
            else
            {
                loader.Enable(GL_DEPTH_BOUNDS_TEST);
                loader.DepthBounds(min_depth_bounds, max_depth_bounds);
            }
        }
    }

    void GraphicsPipelineDepthStencilState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineMultisampleState::GraphicsPipelineMultisampleState(
        const Render::GraphicsPipelineMultisampleStateInfo& info)
        : sample_count(info.sample_count),
          sample_shading_enabled(info.sample_shading_enabled),
          min_sample_shading(info.min_sample_shading),
          sample_mask(info.sample_mask.begin(), info.sample_mask.end()),
          alpha_to_coverage_enabled(info.alpha_to_coverage_enabled),
          alpha_to_one_enabled(info.alpha_to_one_enabled)
    {
        multisample_enabled =
            (sample_count != Render::SampleCount::SampleCount_1 || sample_shading_enabled ||
             alpha_to_coverage_enabled || alpha_to_one_enabled);

        if(multisample_enabled)
        {
            if(!sample_mask.empty())
            {
                int mask_count = (sample_count == Render::SampleCount::SampleCount_64 ? 2 : 1);
                if(!(sample_mask.size() >= mask_count))
                    throw std::runtime_error(std::format("Sample mask size({}) must be >= {}",
                                                         sample_mask.size(),
                                                         mask_count));
            }
        }
    }

    void GraphicsPipelineMultisampleState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);
        if(!multisample_enabled)
            loader.Disable(GL_MULTISAMPLE);
        else
        {
            loader.Enable(GL_MULTISAMPLE);

            if(!sample_shading_enabled)
                loader.Disable(GL_SAMPLE_SHADING);
            else
            {
                loader.Enable(GL_SAMPLE_SHADING);
                loader.MinSampleShading(min_sample_shading);
            }

            if(!alpha_to_coverage_enabled)
                loader.Disable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            else
                loader.Enable(GL_SAMPLE_ALPHA_TO_COVERAGE);

            if(!alpha_to_one_enabled)
                loader.Disable(GL_SAMPLE_ALPHA_TO_ONE);
            else
                loader.Enable(GL_SAMPLE_ALPHA_TO_ONE);

            if(!sample_mask.empty())
            {
                loader.Enable(GL_SAMPLE_MASK);
                int mask_count = (sample_count == Render::SampleCount::SampleCount_64 ? 2 : 1);

                for(int i = 0; i < mask_count; i++)
                    loader.SampleMaski(i, sample_mask[i]);
            }
            else
                loader.Disable(GL_SAMPLE_MASK);
        }
    }
    void GraphicsPipelineMultisampleState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineRasterizationState::GraphicsPipelineRasterizationState(
        const Render::GraphicsPipelineRasterizationStateInfo& info)
        : depth_clamp_enabled(info.depth_clamp_enabled),
          rasterizer_discard_enabled(info.rasterizer_discard_enabled),
          polygon_mode(PolygonModeToNative(info.polygon_mode)),
          cull_mode(CullModeToNative(info.cull_mode)),
          front_face(FrontFaceToNative(info.front_face)),
          polygon_offset_mode(DecodePolygonOffsetMode(info.polygon_mode)),
          depth_bias_enabled(info.depth_bias_enabled),
          depth_bias_constant_factor(info.depth_bias_constant_factor),
          depth_bias_clamp(info.depth_bias_clamp),
          depth_bias_slope_factor(info.depth_bias_slope_factor),
          line_width(info.line_width)
    {}

    void GraphicsPipelineRasterizationState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(parent.GetContext()->GetProperties().features.depth_clamp)
        {
            if(depth_clamp_enabled)
                loader.Enable(GL_DEPTH_CLAMP);
            else
                loader.Disable(GL_DEPTH_CLAMP);
        }

        if(rasterizer_discard_enabled)
            loader.Enable(GL_RASTERIZER_DISCARD);
        else
            loader.Disable(GL_RASTERIZER_DISCARD);

        loader.PolygonMode(GL_FRONT_AND_BACK, polygon_mode);

        if(cull_mode == OGL_CULL_MODE_NONE)
            loader.Disable(GL_CULL_FACE);
        else
        {
            loader.Enable(GL_CULL_FACE);
            loader.CullFace(cull_mode);
        }
        loader.FrontFace(front_face);
        loader.LineWidth(line_width);

        if(depth_bias_enabled)
        {
            loader.Enable(polygon_offset_mode);
            if(parent.GetContext()->GetProperties().features.depth_bias_clamp)
                loader.PolygonOffsetClamp(depth_bias_slope_factor,
                                          depth_bias_constant_factor,
                                          depth_bias_clamp);
            else
                loader.PolygonOffset(depth_bias_slope_factor, depth_bias_constant_factor);
        }
        else
        {
            loader.Disable(polygon_offset_mode);
        }
    }

    void GraphicsPipelineRasterizationState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineViewportState::GraphicsPipelineViewportState(
        const Render::GraphicsPipelineViewportStateInfo& info,
        bool _predefined_viewport_enabled,
        bool _predefined_scissors_enabled)
        : predefined_viewport_enabled(_predefined_viewport_enabled),
          predefined_scissors_enabled(_predefined_scissors_enabled),
          count(info.count)
    {
        if(count == 0)
            throw std::runtime_error("Count of viewports must be greater than zero");

        if(predefined_viewport_enabled)
        {
            if(info.predefined_viewports.size() < count)
                throw std::runtime_error(
                    "Count of viewports must be equal to info.count parameter");

            predefined_viewports.assign(info.predefined_viewports.begin(),
                                        info.predefined_viewports.begin() + count);
        }

        if(predefined_scissors_enabled)
        {
            if(info.predefined_scissors.size() < count)
                throw std::runtime_error(
                    "Count of scissor boxes must be equal to info.count parameter");

            predefined_scissors.assign(info.predefined_scissors.begin(),
                                       info.predefined_scissors.begin() + count);
        }
    }

    void GraphicsPipelineViewportState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(predefined_viewport_enabled)
        {
            for(std::size_t i = 0; i < predefined_viewports.size(); i++)
            {
                loader.ViewportIndexedf(i,
                                        predefined_viewports[i].x,
                                        predefined_viewports[i].y,
                                        predefined_viewports[i].width,
                                        predefined_viewports[i].height);
                loader.DepthRangeIndexed(i,
                                         predefined_viewports[i].min_depth,
                                         predefined_viewports[i].max_depth);
            }
        }

        if(predefined_scissors_enabled)
        {
            loader.Enable(GL_SCISSOR_TEST);
            loader.ScissorArrayv(0, predefined_scissors.size(), &predefined_scissors[0].offset.x);
        }
        else
            loader.Disable(GL_SCISSOR_TEST);
    }

    void GraphicsPipelineViewportState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineState::GraphicsPipelineState(Pipeline& parent,
                                                 const Render::GraphicsPipelineInfo& info)
    {
        std::optional<GraphicsPipelineVertexInputState> _vertex_input_state;
        std::optional<GraphicsPipelineInputAssemblyState> _input_assembly_state;
        std::optional<GraphicsPipelineTessellationState> _tessellation_state;
        std::optional<GraphicsPipelineColorBlendState> _color_blend_state;
        std::optional<GraphicsPipelineDepthStencilState> _depth_stencil_state;
        std::optional<GraphicsPipelineMultisampleState> _multisample_state;
        std::optional<GraphicsPipelineRasterizationState> _rasterization_state;
        std::optional<GraphicsPipelineViewportState> _viewport_state;

        hrs::scoped_call cleanup(
            [&]()
            {
                if(_viewport_state)
                    _viewport_state->Destroy(parent);

                if(_rasterization_state)
                    _rasterization_state->Destroy(parent);

                if(_multisample_state)
                    _multisample_state->Destroy(parent);

                if(_depth_stencil_state)
                    _depth_stencil_state->Destroy(parent);

                if(_color_blend_state)
                    _color_blend_state->Destroy(parent);

                if(_tessellation_state)
                    _tessellation_state->Destroy(parent);

                if(_input_assembly_state)
                    _input_assembly_state->Destroy(parent);

                if(_vertex_input_state)
                    _vertex_input_state->Destroy(parent);
            });

        bool have_tessellation_stage =
            std::ranges::find_if(info.shaders,
                                 [](const Render::Shader* const shader)
                                 {
                                     return static_cast<const Shader*>(shader)->GetStage() ==
                                            (Render::ShaderStageFlagBits::TessellationControl ||
                                             Render::ShaderStageFlagBits::TessellationEvaluation);
                                 }) != info.shaders.end();

        bool predefined_viewport_enabled = false;
        bool predefined_scissors_enabled = false;
        for(const auto& state: info.state_info.dynamic_states)
        {
            if(state == Render::DynamicState::Viewport)
                predefined_viewport_enabled = true;
            else if(state == Render::DynamicState::Scissors)
                predefined_scissors_enabled = true;
        }

        _vertex_input_state =
            GraphicsPipelineVertexInputState(parent, info.state_info.vertex_input_state_info);
        _input_assembly_state =
            GraphicsPipelineInputAssemblyState(info.state_info.input_assembly_state_info);
        _tessellation_state =
            GraphicsPipelineTessellationState(info.state_info.tessellation_state_info,
                                              have_tessellation_stage);
        _color_blend_state =
            GraphicsPipelineColorBlendState(info.state_info.color_blend_state_info);
        _depth_stencil_state =
            GraphicsPipelineDepthStencilState(info.state_info.depth_stencil_state_info);
        _multisample_state =
            GraphicsPipelineMultisampleState(info.state_info.multisample_state_info);
        _rasterization_state =
            GraphicsPipelineRasterizationState(info.state_info.rasterization_state_info);
        _viewport_state = GraphicsPipelineViewportState(info.state_info.viewport_state_info,
                                                        predefined_viewport_enabled,
                                                        predefined_scissors_enabled);

        cleanup.drop();

        vertex_input_state = std::move(*_vertex_input_state);
        input_assembly_state = std::move(*_input_assembly_state);
        tessellation_state = std::move(*_tessellation_state);
        color_blend_state = std::move(*_color_blend_state);
        depth_stencil_state = std::move(*_depth_stencil_state);
        multisample_state = std::move(*_multisample_state);
        rasterization_state = std::move(*_rasterization_state);
        viewport_state = std::move(*_viewport_state);
    }

    void GraphicsPipelineState::Set(CommandBuffer& cmd, Pipeline& parent) noexcept
    {
        vertex_input_state.Set(cmd, parent);
        input_assembly_state.Set(cmd, parent);
        tessellation_state.Set(cmd, parent);
        color_blend_state.Set(cmd, parent);
        depth_stencil_state.Set(cmd, parent);
        multisample_state.Set(cmd, parent);
        rasterization_state.Set(cmd, parent);
        viewport_state.Set(cmd, parent);
    }

    void GraphicsPipelineState::Destroy(Pipeline& parent) noexcept
    {
        vertex_input_state.Destroy(parent);
        input_assembly_state.Destroy(parent);
        tessellation_state.Destroy(parent);
        color_blend_state.Destroy(parent);
        depth_stencil_state.Destroy(parent);
        multisample_state.Destroy(parent);
        rasterization_state.Destroy(parent);
        viewport_state.Destroy(parent);
    }

    GLenum GraphicsPipelineState::GetInputAssemblyStateTopology() const noexcept
    {
        return input_assembly_state.GetTopology();
    }

    GLHandle GraphicsPipelineState::GetVertexInputStateHandle() const noexcept
    {
        return vertex_input_state.GetHandle();
    }

    GLsizei GraphicsPipelineState::GetVertexInputStateStride(GLuint binding) const
    {
        return vertex_input_state.GetStride(binding);
    }

    void GraphicsPipelineState::SetIndexBufferState(Render::IndexType index_type,
                                                    std::uintptr_t index_buffer_offset) noexcept
    {
        draw_state.index_type = IndexTypeToNative(index_type);
        switch(index_type)
        {
            case Render::IndexType::U8:
                draw_state.index_size = 1;
                break;
            case Render::IndexType::U16:
                draw_state.index_size = 2;
                break;
            case Render::IndexType::U32:
                draw_state.index_size = 4;
                break;
        }

        draw_state.index_buffer_offset = index_buffer_offset;
    }

    GLenum GraphicsPipelineState::GetIndexBufferStateType() const noexcept
    {
        return draw_state.index_type;
    }

    std::uintptr_t
    GraphicsPipelineState::GetIndexBufferStateOffset(std::uint32_t first_index) const noexcept
    {
        return draw_state.index_buffer_offset + draw_state.index_size * first_index;
    }
};