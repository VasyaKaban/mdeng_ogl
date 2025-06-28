#pragma once

#include <span>
#include <unordered_map>
#include <array>
#include "../../Common.h"
#include "../../../hrs/non_creatable.hpp"

class Pipeline;

//VertexInput
enum class InputRate
{
    VertexRate = 0,
    InstanceRate = 1
};

struct VertexInputBindingDescription
{
    GLuint binding;
    GLsizei stride;
    InputRate input_rate;
};

enum class VertexInputAttributeSize
{
    Scalar = 1,
    Vec2 = 2,
    Vec3 = 3,
    Vec4 = 4,
    BGRA = GL_BGRA
};

enum class VertexInputAttributeType
{
    Byte = GL_BYTE,
    Short = GL_SHORT,
    Int = GL_INT,
    Fixed = GL_FIXED,
    Float = GL_FLOAT,
    HalfFloat = GL_HALF_FLOAT,
    Double = GL_DOUBLE,
    UnsignedByte = GL_UNSIGNED_BYTE,
    UnsignedShort = GL_UNSIGNED_SHORT,
    UnsignedInt = GL_UNSIGNED_INT,
    Int_2_10_10_10_REV = GL_INT_2_10_10_10_REV,
    UnsignedInt_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
    UnsignedInt_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV
};

struct VertexInputAttributeDescription
{
    GLuint location;
    GLuint binding;
    VertexInputAttributeSize size;
    VertexInputAttributeType type;
    GLuint offset;
};

struct GraphicsPipelineVertexInputStateInfo
{
    std::span<const VertexInputBindingDescription> vertex_input_bindings;
    std::span<const VertexInputAttributeDescription> vertex_input_attributes;
};

class GraphicsPipelineVertexInputState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineVertexInputState(Pipeline* parent,
                                     const GraphicsPipelineVertexInputStateInfo& info);
    GraphicsPipelineVertexInputState() noexcept;

    ~GraphicsPipelineVertexInputState() = default;
    GraphicsPipelineVertexInputState(GraphicsPipelineVertexInputState&& state) = default;
    GraphicsPipelineVertexInputState& operator=(GraphicsPipelineVertexInputState&& state) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    using BindingStridesMap = std::unordered_map<GLuint, GLsizei>; //binding -> stride
    GLHandle vao;
    BindingStridesMap binding_strides_map;
};
//~VertexInput

//InputAssembly
enum class PrimitiveTopology
{
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    LinesAdjacency = GL_LINES_ADJACENCY,
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    TriangleStrIpAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    Patches = GL_PATCHES,

    LineLoop = GL_LINE_LOOP //unused in VK
};

struct GraphicsPipelineInputAssemblyStateInfo
{
    PrimitiveTopology topology;
    bool primitive_restart_enabled;
};

class GraphicsPipelineInputAssemblyState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineInputAssemblyState(const GraphicsPipelineInputAssemblyStateInfo& info);
    GraphicsPipelineInputAssemblyState() noexcept;
    ~GraphicsPipelineInputAssemblyState() = default;
    GraphicsPipelineInputAssemblyState(GraphicsPipelineInputAssemblyState&&) = default;
    GraphicsPipelineInputAssemblyState& operator=(GraphicsPipelineInputAssemblyState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    PrimitiveTopology topology; //used directly by Context!
    bool primitive_restart_enabled;
};
//~InputAssembly

//Blend
enum class BlendFactor
{
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcColor = GL_SRC_COLOR,
    OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    DstColor = GL_DST_COLOR,
    OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    DstAlpha = GL_DST_ALPHA,
    OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
    ConstantColor = GL_CONSTANT_COLOR,
    OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
    ConstantAlpha = GL_CONSTANT_ALPHA,
    OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
    SrvAlphaSaturate = GL_SRC_ALPHA_SATURATE,
    Src1Color = GL_SRC1_COLOR,
    OneMinusSrc1Color = GL_ONE_MINUS_SRC1_COLOR,
    Src1Alpha = GL_SRC1_ALPHA,
    OneMinusSrc1Alpha = GL_ONE_MINUS_SRC1_ALPHA
};

enum class BlendLogicOp
{
    Clear = GL_CLEAR,
    Set = GL_SET,
    Copy = GL_COPY,
    CopyInverted = GL_COPY_INVERTED,
    Noop = GL_NOOP,
    Invert = GL_INVERT,
    And = GL_AND,
    NotAnd = GL_NAND,
    Or = GL_OR,
    NotOr = GL_NOR,
    Xor = GL_XOR,
    Equivalent = GL_EQUIV,
    AndReverse = GL_AND_REVERSE,
    AndInverted = GL_AND_INVERTED,
    OrReverse = GL_OR_REVERSE,
    OrInverted = GL_OR_INVERTED
};

struct BlendFunctionInfo
{
    GLuint buffer_index;
    BlendFactor src_rgb;
    BlendFactor src_alpha;
    BlendFactor dst_rgb;
    BlendFactor dst_alpha;
};

enum class BlendEquation
{
    Add = GL_FUNC_ADD,
    Subtract = GL_FUNC_SUBTRACT,
    ReverseSubstract = GL_FUNC_REVERSE_SUBTRACT,
    Min = GL_MIN,
    Max = GL_MAX
};

struct BlendEquationInfo
{
    GLuint buffer_index;
    BlendEquation eq_rgb;
    BlendEquation eq_alpha;
};

struct GraphicsPipelineBlendStateInfo
{
    bool blend_enabled;
    std::span<BlendFunctionInfo> blend_function_info;
    bool logic_op_enabled;
    BlendLogicOp logic_op;
    GLfloat blend_color[4];
    std::span<BlendEquationInfo> blend_eq_info;
};

class GraphicsPipelineBlendState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineBlendState(const GraphicsPipelineBlendStateInfo& info);
    GraphicsPipelineBlendState() noexcept;
    ~GraphicsPipelineBlendState() = default;
    GraphicsPipelineBlendState(GraphicsPipelineBlendState&&) = default;
    GraphicsPipelineBlendState& operator=(GraphicsPipelineBlendState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    bool blend_enabled;
    std::vector<BlendFunctionInfo> blend_function_info;
    bool logic_op_enabled;
    BlendLogicOp logic_op;
    std::array<GLfloat, 4> blend_color;
    std::vector<BlendEquationInfo> blend_eq_info;
};
//~Blend

//DepthStencil
enum class StencilOp
{
    Keep = GL_KEEP,
    Zero = GL_ZERO,
    Replcae = GL_REPLACE,
    Increment = GL_INCR,
    IncrementWrap = GL_INCR_WRAP,
    Decrement = GL_DECR,
    DecrementWrap = GL_DECR_WRAP,
    Invert = GL_INVERT
};

struct StencilStateOp
{
    StencilOp fail_op;
    StencilOp pass_op;
    StencilOp depth_fail_op;
    CompareOp compare_op;
    GLint reference;
    GLuint compare_mask;
    GLuint write_mask;
};

struct GraphicsPipelineDepthStencilStateInfo
{
    bool depth_test_enabled;
    CompareOp depth_compare_op;
    bool write_enabled;
    GLfloat min_depth_bound;
    GLfloat max_depth_bound;
    bool stencil_test_enabled;
    StencilStateOp stencil_front_op;
    StencilStateOp stencil_back_op;
};

class GraphicsPipelineDepthStencilState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineDepthStencilState(const GraphicsPipelineDepthStencilStateInfo& info);
    GraphicsPipelineDepthStencilState() noexcept;
    ~GraphicsPipelineDepthStencilState() = default;
    GraphicsPipelineDepthStencilState(GraphicsPipelineDepthStencilState&&) = default;
    GraphicsPipelineDepthStencilState& operator=(GraphicsPipelineDepthStencilState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    bool depth_test_enabled;
    CompareOp depth_compare_op;
    bool write_enabled;
    GLfloat min_depth_bound;
    GLfloat max_depth_bound;
    bool stencil_test_enabled;
    StencilStateOp stencil_front_op;
    StencilStateOp stencil_back_op;
};
//~DepthStencil

//Multisample
struct GraphicsPipelineMultisampleStateInfo
{
    bool multisample_enabled;
    SampleCount sample_count;
    std::span<GLbitfield> sample_mask; //NULL, 1[1 - 32], 2[64]
    bool sample_shading_enabled;
    GLfloat min_sample_shading;
    bool alpha_to_coverage_enabled;
    bool alpha_to_one_enabled;
};

class GraphicsPipelineMultisampleState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineMultisampleState(const GraphicsPipelineMultisampleStateInfo& info);
    GraphicsPipelineMultisampleState() noexcept;
    ~GraphicsPipelineMultisampleState() = default;
    GraphicsPipelineMultisampleState(GraphicsPipelineMultisampleState&&) = default;
    GraphicsPipelineMultisampleState& operator=(GraphicsPipelineMultisampleState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    bool multisample_enabled;
    SampleCount sample_count;
    std::vector<GLbitfield> sample_mask; //NULL, 1[1 - 32], 2[64]
    bool sample_shading_enabled;
    GLfloat min_sample_shading;
    bool alpha_to_coverage_enabled;
    bool alpha_to_one_enabled;
};
//~Multisample

//Rasterization
enum class PolygonMode
{
    Point = GL_POINT,
    Line = GL_LINE,
    Fill = GL_FILL
};

enum class CullMode
{
    None,
    Front = GL_FRONT,
    Back = GL_BACK,
    FrontAndBack = GL_FRONT_AND_BACK
};

enum class FrontFace
{
    CounterClockwise = GL_CCW,
    Clockwise = GL_CW
};

struct GraphicsPipelineRasterizationStateInfo
{
    bool depth_clamp_enabled;
    bool rasterizer_discard_enabled;
    PolygonMode polygon_mode;
    CullMode cull_mode;
    FrontFace front_face;
    GLfloat line_width;
};

class GraphicsPipelineRasterizationState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineRasterizationState(const GraphicsPipelineRasterizationStateInfo& info);
    GraphicsPipelineRasterizationState() noexcept;
    ~GraphicsPipelineRasterizationState() = default;
    GraphicsPipelineRasterizationState(GraphicsPipelineRasterizationState&&) = default;
    GraphicsPipelineRasterizationState& operator=(GraphicsPipelineRasterizationState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    bool depth_clamp_enabled;
    bool rasterizer_discard_enabled;
    PolygonMode polygon_mode;
    CullMode cull_mode;
    FrontFace front_face;
    GLfloat line_width;
};
//Rasterization

//Viewport
struct GraphicsPipelineViewportStateInfo
{
    bool viewport_enabled;
    std::span<Viewport> viewports;
    std::span<Rect2D> scissors;
};

class GraphicsPipelineViewportState : hrs::non_copyable
{
    friend class Pipeline;
public:
    GraphicsPipelineViewportState(const GraphicsPipelineViewportStateInfo& info);
    GraphicsPipelineViewportState() noexcept;
    ~GraphicsPipelineViewportState() = default;
    GraphicsPipelineViewportState(GraphicsPipelineViewportState&&) = default;
    GraphicsPipelineViewportState& operator=(GraphicsPipelineViewportState&&) = default;

    void Set(Pipeline& parent) noexcept;
    void Destroy(Pipeline& parent) noexcept;
private:
    bool viewport_enabled;
    std::vector<Viewport> viewports;

    bool scissors_enabled;
    std::vector<Rect2D> scissors;
};
//~Viewport

//Draw

enum class IndexType
{
    u8 = GL_UNSIGNED_BYTE,
    u16 = GL_UNSIGNED_SHORT,
    u32 = GL_UNSIGNED_INT
};

struct GraphicsPipelineDrawState : hrs::non_copyable
{
    IndexType index_type;
    std::uintptr_t index_buffer_offset;
};

//~Draw

struct GraphicsPipelineStateInfo
{
    GraphicsPipelineVertexInputStateInfo vertex_input_state_info;
    GraphicsPipelineInputAssemblyStateInfo input_assembly_state_info;
    GraphicsPipelineBlendStateInfo blend_state_info;
    GraphicsPipelineDepthStencilStateInfo depth_stencil_state_info;
    GraphicsPipelineMultisampleStateInfo multisample_state_info;
    GraphicsPipelineRasterizationStateInfo rasterization_state_info;
    GraphicsPipelineViewportStateInfo viewport_state_info;
};

class GraphicsPipelineState : hrs::non_copyable, hrs::non_movable
{
    friend class Pipeline;
public:
    GraphicsPipelineState(Pipeline* parent, const GraphicsPipelineStateInfo& info);
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