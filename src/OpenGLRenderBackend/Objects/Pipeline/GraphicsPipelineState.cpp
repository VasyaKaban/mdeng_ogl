#include "GraphicsPipelineState.h"
#include "Pipeline.h"
#include "../../Context/Context.h"
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

    void GraphicsPipelineVertexInputState::Set(Pipeline& parent) noexcept
    {
        get_loader(parent).BindVertexArray(vao);
    }

    void GraphicsPipelineVertexInputState::Destroy(Pipeline& parent) noexcept
    {
        get_loader(parent).DeleteVertexArrays(1, &vao);

        vao = OGL_NULL_HANDLE;
        binding_strides_map.clear();
    }

    GraphicsPipelineInputAssemblyState::GraphicsPipelineInputAssemblyState(
        const Render::GraphicsPipelineInputAssemblyStateInfo& info)
        : topology(PrimitiveTopologyToNative(info.topology)),
          primitive_restart_enabled(info.primitive_restart_enabled)
    {}

    void GraphicsPipelineInputAssemblyState::Set(Pipeline& parent) noexcept
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

    GraphicsPipelineBlendState::GraphicsPipelineBlendState(
        const Render::GraphicsPipelineBlendStateInfo& info)
        : blend_enabled(info.blend_enabled),
          //blend_function_info(info.blend_function_info.begin(), info.blend_function_info.end()),
          logic_op_enabled(info.logic_op_enabled),
          logic_op(BlendLogicOpToNative(info.logic_op)),
          blend_color(
              {info.blend_color[0], info.blend_color[1], info.blend_color[2], info.blend_color[3]})
    //blend_eq_info(info.blend_eq_info.begin(), info.blend_eq_info.end())
    {
        if(blend_enabled)
        {
            blend_function_info.reserve(info.blend_function_info.size());
            for(const auto& f_info: info.blend_function_info)
            {
                blend_function_info.push_back(
                    BlendFunctionInfoNative{.buffer_index = f_info.buffer_index,
                                            .src_rgb = BlendFactorToNative(f_info.src_rgb),
                                            .src_alpha = BlendFactorToNative(f_info.src_alpha),
                                            .dst_rgb = BlendFactorToNative(f_info.dst_rgb),
                                            .dst_alpha = BlendFactorToNative(f_info.dst_alpha)});
            }

            blend_eq_info.reserve(info.blend_eq_info.size());
            for(const auto& eq_info: info.blend_eq_info)
            {
                blend_eq_info.push_back(
                    BlendEquationInfoNative{.buffer_index = eq_info.buffer_index,
                                            .eq_rgb = BlendEquationToNative(eq_info.eq_rgb),
                                            .eq_alpha = BlendEquationToNative(eq_info.eq_alpha)});
            }
        }
    }

    void GraphicsPipelineBlendState::Set(Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(!blend_enabled)
            loader.Disable(GL_BLEND);
        else
        {
            loader.Enable(GL_BLEND);
            for(const auto& func: blend_function_info)
                loader.BlendFuncSeparatei(func.buffer_index,
                                          func.src_rgb,
                                          func.dst_rgb,
                                          func.src_alpha,
                                          func.dst_alpha);

            if(!logic_op_enabled)
                loader.Disable(GL_COLOR_LOGIC_OP);
            else
            {
                loader.Enable(GL_COLOR_LOGIC_OP);
                loader.LogicOp(logic_op);
            }

            loader.BlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
            for(const auto& eq: blend_eq_info)
                loader.BlendEquationSeparatei(eq.buffer_index, eq.eq_rgb, eq.eq_alpha);
        }
    }

    void GraphicsPipelineBlendState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    StencilStateOpNative stencil_state_op_to_native(const Render::StencilStateOp& op) noexcept
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
          depth_compare_op(ComapreOpToNative(info.depth_compare_op)),
          write_enabled(info.write_enabled),
          stencil_test_enabled(info.stencil_test_enabled),
          stencil_front_op(stencil_state_op_to_native(info.stencil_front_op)),
          stencil_back_op(stencil_state_op_to_native(info.stencil_back_op))
    {}

    static void
    stencil_state_op_set(Pipeline& parent, GLenum face, const StencilStateOpNative& state)
    {
        const auto& loader = get_loader(parent);
        loader.StencilFuncSeparate(face, state.compare_op, state.reference, state.compare_mask);
        loader.StencilMaskSeparate(face, state.write_mask);
        loader.StencilOpSeparate(face, state.fail_op, state.depth_fail_op, state.pass_op);
    }

    void GraphicsPipelineDepthStencilState::Set(Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);
        if(!depth_test_enabled)
            loader.Disable(GL_DEPTH_TEST);
        else
        {
            loader.Enable(GL_DEPTH_TEST);
            if(write_enabled)
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
    }

    void GraphicsPipelineDepthStencilState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineMultisampleState::GraphicsPipelineMultisampleState(
        const Render::GraphicsPipelineMultisampleStateInfo& info)
        : multisample_enabled(info.multisample_enabled),
          sample_count(info.sample_count),
          sample_mask(info.sample_mask.begin(), info.sample_mask.end()),
          sample_shading_enabled(info.sample_shading_enabled),
          min_sample_shading(info.min_sample_shading),
          alpha_to_coverage_enabled(info.alpha_to_coverage_enabled),
          alpha_to_one_enabled(info.alpha_to_one_enabled)
    {
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

    void GraphicsPipelineMultisampleState::Set(Pipeline& parent) noexcept
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
          line_width(info.line_width)
    {}

    void GraphicsPipelineRasterizationState::Set(Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(depth_clamp_enabled)
            loader.Enable(GL_DEPTH_CLAMP);
        else
            loader.Disable(GL_DEPTH_CLAMP);

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
    }

    void GraphicsPipelineRasterizationState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineViewportState::GraphicsPipelineViewportState(
        const Render::GraphicsPipelineViewportStateInfo& info)
        : viewport_enabled(info.viewport_enabled),
          viewports(info.viewports.begin(), info.viewports.end()),
          scissors(info.scissors.begin(), info.scissors.end())
    {
        if(viewport_enabled)
        {
            if(viewports.size() != scissors.size())
                throw std::runtime_error("Count of scissor boxes must be equal to viewport count");
        }
    }

    void GraphicsPipelineViewportState::Set(Pipeline& parent) noexcept
    {
        const auto& loader = get_loader(parent);

        if(!viewport_enabled || viewports.empty())
            return;

        for(std::size_t i = 0; i < viewports.size(); i++)
        {
            loader.ViewportIndexedf(i,
                                    viewports[i].x,
                                    viewports[i].y,
                                    viewports[i].width,
                                    viewports[i].height);
            loader.DepthRangeIndexed(i, viewports[i].min_depth, viewports[i].max_depth);
        }

        loader.Enable(GL_SCISSOR_TEST);
        loader.ScissorArrayv(0, scissors.size(), &scissors[0].offset.x);
    }

    void GraphicsPipelineViewportState::Destroy(Pipeline& parent) noexcept
    {
        //noop
    }

    GraphicsPipelineState::GraphicsPipelineState(Pipeline& parent,
                                                 const Render::GraphicsPipelineStateInfo& info)
    {
        std::optional<GraphicsPipelineVertexInputState> _vertex_input_state;
        std::optional<GraphicsPipelineInputAssemblyState> _input_assembly_state;
        std::optional<GraphicsPipelineBlendState> _blend_state;
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

                if(_blend_state)
                    _blend_state->Destroy(parent);

                if(_input_assembly_state)
                    _input_assembly_state->Destroy(parent);

                if(_vertex_input_state)
                    _vertex_input_state->Destroy(parent);
            });

        _vertex_input_state =
            GraphicsPipelineVertexInputState(parent, info.vertex_input_state_info);
        _input_assembly_state = GraphicsPipelineInputAssemblyState(info.input_assembly_state_info);
        _blend_state = GraphicsPipelineBlendState(info.blend_state_info);
        _depth_stencil_state = GraphicsPipelineDepthStencilState(info.depth_stencil_state_info);
        _multisample_state = GraphicsPipelineMultisampleState(info.multisample_state_info);
        _rasterization_state = GraphicsPipelineRasterizationState(info.rasterization_state_info);
        _viewport_state = GraphicsPipelineViewportState(info.viewport_state_info);

        cleanup.drop();

        vertex_input_state = std::move(*_vertex_input_state);
        input_assembly_state = std::move(*_input_assembly_state);
        blend_state = std::move(*_blend_state);
        depth_stencil_state = std::move(*_depth_stencil_state);
        multisample_state = std::move(*_multisample_state);
        rasterization_state = std::move(*_rasterization_state);
        viewport_state = std::move(*_viewport_state);
    }

    void GraphicsPipelineState::Set(Pipeline& parent) noexcept
    {
        vertex_input_state.Set(parent);
        input_assembly_state.Set(parent);
        blend_state.Set(parent);
        depth_stencil_state.Set(parent);
        multisample_state.Set(parent);
        rasterization_state.Set(parent);
        viewport_state.Set(parent);
    }

    void GraphicsPipelineState::Destroy(Pipeline& parent) noexcept
    {
        vertex_input_state.Destroy(parent);
        input_assembly_state.Destroy(parent);
        blend_state.Destroy(parent);
        depth_stencil_state.Destroy(parent);
        multisample_state.Destroy(parent);
        rasterization_state.Destroy(parent);
        viewport_state.Destroy(parent);
    }
};