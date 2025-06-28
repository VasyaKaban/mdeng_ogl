#include "GraphicsPipelineState.h"
#include "Pipeline.h"
#include "../../Context/Context.h"
#include <stdexcept>
#include <format>
#include "../../../hrs/scoped_call.hpp"

//VertexInput
GraphicsPipelineVertexInputState::GraphicsPipelineVertexInputState(
    Pipeline* parent,
    const GraphicsPipelineVertexInputStateInfo& info)
{
    GLHandle _vao = GL_NULL_HANDLE;
    parent->GetContext()->GetLoader().CreateVertexArrays(1, &_vao);
    if(_vao == GL_NULL_HANDLE)
        throw std::runtime_error("Failed to create vertex array");

    BindingStridesMap _binding_strides_map;

    for(const auto& binding: info.vertex_input_bindings)
    {
        parent->GetContext()->GetLoader().VertexArrayBindingDivisor(
            _vao,
            binding.binding,
            static_cast<GLuint>(binding.input_rate));

        _binding_strides_map.insert({binding.binding, binding.stride});
    }

    for(auto& attr: info.vertex_input_attributes)
    {
        parent->GetContext()->GetLoader().EnableVertexArrayAttrib(_vao, attr.location);
        parent->GetContext()->GetLoader().VertexArrayAttribFormat(_vao,
                                                                  attr.location,
                                                                  static_cast<GLint>(attr.size),
                                                                  static_cast<GLenum>(attr.type),
                                                                  GL_FALSE,
                                                                  attr.offset);

        parent->GetContext()->GetLoader().VertexArrayAttribBinding(_vao,
                                                                   attr.location,
                                                                   attr.binding);
    }

    vao = _vao;
    binding_strides_map = std::move(_binding_strides_map);
}

GraphicsPipelineVertexInputState::GraphicsPipelineVertexInputState() noexcept
    : vao(GL_NULL_HANDLE)
{}

void GraphicsPipelineVertexInputState::Set(Pipeline& parent) noexcept
{
    parent.GetContext()->GetLoader().BindVertexArray(vao);
}

void GraphicsPipelineVertexInputState::Destroy(Pipeline& parent) noexcept
{
    if(vao == GL_NULL_HANDLE)
        return;

    parent.GetContext()->GetLoader().DeleteVertexArrays(1, &vao);
    vao = GL_NULL_HANDLE;
    binding_strides_map.clear();
}
//~VertexInput

//InputAssembly

GraphicsPipelineInputAssemblyState::GraphicsPipelineInputAssemblyState(
    const GraphicsPipelineInputAssemblyStateInfo& info)
    : topology(info.topology),
      primitive_restart_enabled(info.primitive_restart_enabled)
{}

GraphicsPipelineInputAssemblyState::GraphicsPipelineInputAssemblyState() noexcept
    : topology(PrimitiveTopology::Points),
      primitive_restart_enabled(false)
{}

void GraphicsPipelineInputAssemblyState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();
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

//~InputAssembly

//Blend

GraphicsPipelineBlendState::GraphicsPipelineBlendState(const GraphicsPipelineBlendStateInfo& info)
    : blend_enabled(info.blend_enabled),
      blend_function_info(info.blend_function_info.begin(), info.blend_function_info.end()),
      logic_op_enabled(info.logic_op_enabled),
      logic_op(info.logic_op),
      blend_color(
          {info.blend_color[0], info.blend_color[1], info.blend_color[2], info.blend_color[3]}),
      blend_eq_info(info.blend_eq_info.begin(), info.blend_eq_info.end())
{}

GraphicsPipelineBlendState::GraphicsPipelineBlendState() noexcept
    : blend_enabled(false),
      logic_op_enabled(false),
      logic_op(BlendLogicOp::Noop),
      blend_color({0.0f, 0.0f, 0.0f, 0.0f})
{}

void GraphicsPipelineBlendState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();

    if(!blend_enabled)
        loader.Disable(GL_BLEND);
    else
    {
        loader.Enable(GL_BLEND);
        for(const auto& func: blend_function_info)
            loader.BlendFuncSeparatei(func.buffer_index,
                                      static_cast<GLenum>(func.src_rgb),
                                      static_cast<GLenum>(func.dst_rgb),
                                      static_cast<GLenum>(func.src_alpha),
                                      static_cast<GLenum>(func.dst_alpha));

        if(!logic_op_enabled)
            loader.Disable(GL_COLOR_LOGIC_OP);
        else
        {
            loader.Enable(GL_COLOR_LOGIC_OP);
            loader.LogicOp(static_cast<GLenum>(logic_op));
        }

        loader.BlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
        for(const auto& eq: blend_eq_info)
            loader.BlendEquationSeparatei(eq.buffer_index,
                                          static_cast<GLenum>(eq.eq_rgb),
                                          static_cast<GLenum>(eq.eq_alpha));
    }
}

void GraphicsPipelineBlendState::Destroy(Pipeline& parent) noexcept
{
    //noop
}

//~Blend

//DepthStencil

GraphicsPipelineDepthStencilState::GraphicsPipelineDepthStencilState(
    const GraphicsPipelineDepthStencilStateInfo& info)
    : depth_test_enabled(info.depth_test_enabled),
      depth_compare_op(info.depth_compare_op),
      write_enabled(info.write_enabled),
      min_depth_bound(info.min_depth_bound),
      max_depth_bound(info.max_depth_bound),
      stencil_test_enabled(info.stencil_test_enabled),
      stencil_front_op(info.stencil_front_op),
      stencil_back_op(info.stencil_back_op)
{}

constexpr static auto DEPTH_STENCIL_STATE_DEFAULT_STENCIL_STATE_OP =
    StencilStateOp{.fail_op = StencilOp::Zero,
                   .pass_op = StencilOp::Zero,
                   .depth_fail_op = StencilOp::Zero,
                   .compare_op = CompareOp::Never,
                   .reference = 0,
                   .compare_mask = 0,
                   .write_mask = 0};

GraphicsPipelineDepthStencilState::GraphicsPipelineDepthStencilState() noexcept
    : depth_test_enabled(false),
      depth_compare_op(CompareOp::Never),
      write_enabled(false),
      min_depth_bound(0.0f),
      max_depth_bound(1.0f),
      stencil_test_enabled(false),
      stencil_front_op(DEPTH_STENCIL_STATE_DEFAULT_STENCIL_STATE_OP),
      stencil_back_op(DEPTH_STENCIL_STATE_DEFAULT_STENCIL_STATE_OP)
{}

static void stencil_state_op_set(Pipeline& parent, GLenum face, const StencilStateOp& state)
{
    auto& loader = parent.GetContext()->GetLoader();
    loader.StencilFuncSeparate(face,
                               static_cast<GLenum>(state.compare_op),
                               state.reference,
                               state.compare_mask);
    loader.StencilMaskSeparate(face, state.write_mask);
    loader.StencilOpSeparate(face,
                             static_cast<GLenum>(state.fail_op),
                             static_cast<GLenum>(state.depth_fail_op),
                             static_cast<GLenum>(state.pass_op));
}

void GraphicsPipelineDepthStencilState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();
    if(!depth_test_enabled)
        loader.Disable(GL_DEPTH_TEST);
    else
    {
        loader.Enable(GL_DEPTH_TEST);
        if(write_enabled)
            loader.DepthMask(GL_TRUE);
        else
            loader.DepthMask(GL_FALSE);

        loader.DepthRange(min_depth_bound, max_depth_bound);
        loader.DepthFunc(static_cast<GLenum>(depth_compare_op));
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

//~DepthStencil

//Multisample

GraphicsPipelineMultisampleState::GraphicsPipelineMultisampleState(
    const GraphicsPipelineMultisampleStateInfo& info)
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
            int mask_count = (sample_count == SampleCount::SampleCount_64 ? 2 : 1);
            if(!(sample_mask.size() >= mask_count))
                throw std::runtime_error(std::format("Sample mask size({}) must be >= {}",
                                                     sample_mask.size(),
                                                     mask_count));
        }
    }
}

GraphicsPipelineMultisampleState::GraphicsPipelineMultisampleState() noexcept
    : multisample_enabled(false),
      sample_count(SampleCount::SampleCount_1),
      sample_shading_enabled(false),
      min_sample_shading(0.0f),
      alpha_to_coverage_enabled(false),
      alpha_to_one_enabled(false)
{}

void GraphicsPipelineMultisampleState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();
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
            int mask_count = (sample_count == SampleCount::SampleCount_64 ? 2 : 1);

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

//~Multisample

//Rasterization

GraphicsPipelineRasterizationState::GraphicsPipelineRasterizationState(
    const GraphicsPipelineRasterizationStateInfo& info)
    : depth_clamp_enabled(info.depth_clamp_enabled),
      rasterizer_discard_enabled(info.rasterizer_discard_enabled),
      polygon_mode(info.polygon_mode),
      cull_mode(info.cull_mode),
      front_face(info.front_face),
      line_width(info.line_width)
{}

GraphicsPipelineRasterizationState::GraphicsPipelineRasterizationState() noexcept
    : depth_clamp_enabled(false),
      rasterizer_discard_enabled(false),
      polygon_mode(PolygonMode::Fill),
      cull_mode(CullMode::None),
      front_face(FrontFace::Clockwise),
      line_width(1.0f)
{}

void GraphicsPipelineRasterizationState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();

    if(depth_clamp_enabled)
        loader.Enable(GL_DEPTH_CLAMP);
    else
        loader.Disable(GL_DEPTH_CLAMP);

    if(rasterizer_discard_enabled)
        loader.Enable(GL_RASTERIZER_DISCARD);
    else
        loader.Disable(GL_RASTERIZER_DISCARD);

    loader.PolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(polygon_mode));

    if(cull_mode == CullMode::None)
        loader.Disable(GL_CULL_FACE);
    else
    {
        loader.Enable(GL_CULL_FACE);
        loader.CullFace(static_cast<GLenum>(cull_mode));
    }
    loader.FrontFace(static_cast<GLenum>(front_face));
    loader.LineWidth(line_width);
}

void GraphicsPipelineRasterizationState::Destroy(Pipeline& parent) noexcept
{
    //noop
}

//Rasterization

//Viewport

GraphicsPipelineViewportState::GraphicsPipelineViewportState(
    const GraphicsPipelineViewportStateInfo& info)
    : viewport_enabled(info.viewport_enabled),
      viewports(info.viewports.begin(), info.viewports.end()),
      scissors(info.scissors.begin(), info.scissors.end())
{
    if(viewport_enabled && scissors_enabled)
    {
        if(viewports.size() != scissors.size())
            throw std::runtime_error("Count of scissor boxes must be equal to viewport count");
    }
}

GraphicsPipelineViewportState::GraphicsPipelineViewportState() noexcept
    : viewport_enabled(false),
      scissors_enabled(false)
{}

void GraphicsPipelineViewportState::Set(Pipeline& parent) noexcept
{
    auto& loader = parent.GetContext()->GetLoader();

    if(!viewport_enabled || viewports.empty())
        return;

    loader.ViewportArrayv(0, viewports.size(), &viewports[0].x);

    loader.Enable(GL_SCISSOR_TEST);
    loader.ScissorArrayv(0, scissors.size(), &scissors[0].offset.x);
}

void GraphicsPipelineViewportState::Destroy(Pipeline& parent) noexcept
{
    //noop
}

//~Viewport

GraphicsPipelineState::GraphicsPipelineState(Pipeline* parent,
                                             const GraphicsPipelineStateInfo& info)
{
    std::optional<GraphicsPipelineVertexInputState> _vertex_input_state;
    std::optional<GraphicsPipelineInputAssemblyState> _input_assembly_state;
    std::optional<GraphicsPipelineBlendState> _blend_state;
    std::optional<GraphicsPipelineDepthStencilState> _depth_stencil_state;
    std::optional<GraphicsPipelineMultisampleState> _multisample_state;
    std::optional<GraphicsPipelineRasterizationState> _rasterization_state;
    std::optional<GraphicsPipelineViewportState> _viewport_state;

    hrs::scoped_call cleanup(
        [&, parent]()
        {
            if(_viewport_state)
                _viewport_state->Destroy(*parent);

            if(_rasterization_state)
                _rasterization_state->Destroy(*parent);

            if(_multisample_state)
                _multisample_state->Destroy(*parent);

            if(_depth_stencil_state)
                _depth_stencil_state->Destroy(*parent);

            if(_blend_state)
                _blend_state->Destroy(*parent);

            if(_input_assembly_state)
                _input_assembly_state->Destroy(*parent);

            if(_vertex_input_state)
                _vertex_input_state->Destroy(*parent);
        });

    _vertex_input_state = GraphicsPipelineVertexInputState(parent, info.vertex_input_state_info);
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
