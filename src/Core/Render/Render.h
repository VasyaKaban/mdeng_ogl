#pragma once

#include <cstdint>
#include <array>
#include <variant>
#include <span>
#include <string_view>
#include <functional>

namespace Render
{
    class Context;
    class Object;
    class Buffer;
    class CommandBuffer;
    class CommandPool;
    class Fence;
    class Framebuffer;
    class Image;
    class ImageView;
    class Pipeline;
    class Queue;
    class RenderPass;
    class Sampler;
    class Semaphore;
    class Shader;

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class SampleCount
    {
        SampleCount_1 = 1,
        SampleCount_2 = 2,
        SampleCount_4 = 4,
        SampleCount_8 = 8,
        SampleCount_16 = 16,
        SampleCount_32 = 32,
        SampleCount_64 = 64
    };

    struct Extent2D
    {
        std::uint32_t width;
        std::uint32_t height;
    };

    struct Extent3D
    {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t depth;
    };

    struct Offset2D
    {
        std::int32_t x;
        std::int32_t y;
    };

    struct Offset3D
    {
        std::int32_t x;
        std::int32_t y;
        std::int32_t z;
    };

    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float min_depth;
        float max_depth;
    };

    struct Rect2D
    {
        Offset2D offset;
        Extent2D extent;
    };

    using ClearColorFloatValue = std::array<float, 4>;
    using ClearColorIntValue = std::array<std::int32_t, 4>;
    using ClearColorUIntValue = std::array<std::uint32_t, 4>;

    struct BufferCopyRegion
    {
        std::int64_t src_offset;
        std::int64_t dst_offset;
        std::uint64_t size;
    };

    struct ImageSubresourceLayers
    {
        std::uint32_t mip_level;
        std::uint32_t base_layer;
        std::uint32_t layer_count;
    };

    struct BufferImageCopyRegion
    {
        std::uint64_t buffer_offset;
        std::uint32_t buffer_row_length;
        std::uint32_t buffer_image_height;
        ImageSubresourceLayers subresource_layers;
        Offset3D offset;
        Extent3D extent;
    };

    struct MemoryBufferCopyRegion
    {
        const std::uint8_t* data;
        std::int64_t offset;
        std::uint64_t size;
    };

    struct MemoryImageCopyRegion
    {
        const std::uint8_t* data;
        std::uint32_t buffer_row_length;
        std::uint32_t buffer_image_height;
        ImageSubresourceLayers subresource_layers;
        Offset3D offset;
        Extent3D extent;
    };

    struct BufferBindDesc
    {
        std::uint32_t index;
        std::int64_t offset;
        std::uint64_t size;
    };

    struct ClearColorValue
    {
        std::variant<ClearColorFloatValue, ClearColorIntValue, ClearColorUIntValue> value;
    };

    struct ClearDepthStencilValue
    {
        float depth;
        std::uint32_t stencil;
    };

    struct RenderPassBeginInfo
    {
        Framebuffer* framebuffer;
        std::span<const ClearColorValue> clear_color_values;
        ClearDepthStencilValue clear_depth_stencil_value;
    };

    enum BufferFlagBits
    {
        MapRead = 1 << 0,
        MapWrite = 1 << 1,
        PersistentMapping = 1 << 2,
        CoherentMapping = 1 << 3,
        DynamicStorage = 1 << 4
    };

    using BufferFlags = std::underlying_type_t<BufferFlagBits>;

    struct BufferInfo
    {
        std::uint64_t size;
        BufferFlags flags;
    };

    struct MappedRange
    {
        std::int64_t offset;
        std::uint64_t size;
    };

    struct CommandPoolInfo
    {
        const Queue* queue;
    };

    enum class FenceStatus
    {
        Signaled,
        Unsignaled
    };

    struct AttachmentRef
    {
        std::variant<const Image*, const ImageView*> attachment;
    };

    struct FramebufferInfo
    {
        std::span<const AttachmentRef> color_attachments;
        const AttachmentRef* depth_stencil_attachment;
    };

    enum class ImageType
    {
        Image1D,
        Image2D,
        Image3D
    };

    enum class Format
    {
        R32G32B32A32_FLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32_FLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R16G16B16A16_FLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_UINT,
        R16G16B16A16_SNORM,
        R16G16B16A16_SINT,
        R32G32_FLOAT,
        R32G32_UINT,
        R32G32_SINT,
        D32_FLOAT_S8X24_UINT,
        R10G10B10A2_UNORM,
        R10G10B10A2_UINT,
        R11G11B10_FLOAT,
        R8G8B8A8_UNORM,
        R8G8B8A8_UNORM_SRGB,
        R8G8B8A8_UINT,
        R8G8B8A8_SNORM,
        R8G8B8A8_SINT,
        R16G16_FLOAT,
        R16G16_UNORM,
        R16G16_UINT,
        R16G16_SNORM,
        R16G16_SINT,
        D32_FLOAT,
        R32_FLOAT,
        R32_UINT,
        R32_SINT,
        D24_UNORM_S8_UINT,
        R8G8_UNORM,
        R8G8_UINT,
        R8G8_SNORM,
        R8G8_SINT,
        R16_FLOAT,
        D16_UNORM,
        R16_UNORM,
        R16_UINT,
        R16_SNORM,
        R16_SINT,
        R8_UNORM,
        R8_UINT,
        R8_SNORM,
        R8_SINT,
        R9G9B9E5_SHAREDEXP,
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        BC6H_UF16,
        BC6H_SF16,
        BC7_UNORM,
        BC7_UNORM_SRGB,
        B4G4R4A4_UNORM
    };

    struct ImageInfo
    {
        ImageType image_type;
        Format format;
        Extent3D extent;
        std::uint32_t mip_levels;
        std::uint32_t array_layers;
        SampleCount samples;
    };

    enum class ImageViewType
    {
        ImageView1D,
        ImageView2D,
        ImageView3D,
        ImageViewCubeMap,
        ImageView1DArray,
        ImageView2DArray,
        ImageViewCubeMapArray,
        ImageView2DMultisample,
        ImageView2DMultisampleArray
    };

    enum class ComponentSwizzle
    {
        SwizzleRed,
        SwizzleGreen,
        SwizzleBlue,
        SwizzleAlpha,
        SwizzleZero,
        SwizzleOne,
        SwizzleIdentity
    };

    struct ComponentMapping
    {
        ComponentSwizzle r;
        ComponentSwizzle g;
        ComponentSwizzle b;
        ComponentSwizzle a;
    };

    struct ImageSubresourceRange
    {
        std::uint32_t min_mip_level;
        std::uint32_t mip_level_count;
        std::uint32_t min_layer;
        std::uint32_t layer_count;
    };

    struct ImageViewInfo
    {
        const Image* image;
        ImageViewType view_type;
        Format format;
        ComponentMapping components;
        ImageSubresourceRange subresource_range;
    };

    struct QueueBeginInfo
    {
        std::span<Semaphore*> wait_seamphores;
    };

    struct QueueFlushInfo
    {
        Fence* signal_fence;
        std::span<Semaphore*> signal_seamphores;
        std::span<CommandBuffer*> command_buffers;
    };

    struct AttachmentDescription
    {
        bool clear_load;
    };

    struct RenderPassInfo
    {
        std::span<const AttachmentDescription> color_attachment_descriptions;
        const AttachmentDescription* depth_stencil_attachment_description;
    };

    enum class Filter
    {
        Nearest,
        Linear
    };

    enum class AddressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    struct BorderColor
    {
        std::variant<ClearColorFloatValue, ClearColorIntValue, ClearColorUIntValue> value;
    };

    struct SamplerInfo
    {
        Filter mag_filter; //GL_TEXTURE_MAG_FILTER, GL_NEAREST, GL_LINEAR
        Filter min_filter; //GL_TEXTURE_MIN_FILTER, GL_NEAREST, GL_LINEAR, GNMN, GNML, GLMN, GLML
        Filter mipmap_mode;
        AddressMode address_mode_u; //GL_TEXTURE_WRAP_S
        AddressMode address_mode_v; //GL_TEXTURE_WRAP_T
        AddressMode address_mode_w; //GL_TEXTURE_WRAP_R
        float mip_lod_bias;
        bool anisotropy_enable;
        float max_anisotropy;
        bool compare_enable;
        CompareOp compare_op; //GL_TEXTURE_COMPARE_MODE -> GL_COMPARE_REF_TO_TEXTURE
        float min_lod; //GL_TEXTURE_MIN_LOD
        float max_lod; //GL_TEXTURE_MAX_LOD
        BorderColor border_color; //GL_TEXTURE_BORDER_COLOR
    };

    enum class ShaderStage
    {
        Vertex,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        Fragment,
        Compute,
    };

    struct ShaderInfo
    {
        ShaderStage stage;
        std::span<const char> code;
    };

    //VertexInput
    enum class InputRate
    {
        VertexRate,
        InstanceRate
    };

    struct VertexInputBindingDescription
    {
        std::uint32_t binding;
        std::uint32_t stride;
        InputRate input_rate;
    };

    struct VertexInputAttributeDescription
    {
        std::uint32_t location;
        std::uint32_t binding;
        Format format;
        std::uint32_t offset;
    };

    struct GraphicsPipelineVertexInputStateInfo
    {
        std::span<const VertexInputBindingDescription> vertex_input_bindings;
        std::span<const VertexInputAttributeDescription> vertex_input_attributes;
    };
    //~VertexInput

    //InputAssembly
    enum class PrimitiveTopology
    {
        Points,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,
        TriangleFan,
        LinesAdjacency,
        LineStripAdjacency,
        TrianglesAdjacency,
        TriangleStrIpAdjacency,
        Patches

        //LineLoop = GL_LINE_LOOP //unused in VK
    };

    struct GraphicsPipelineInputAssemblyStateInfo
    {
        PrimitiveTopology topology;
        bool primitive_restart_enabled;
    };
    //~InputAssembly

    //Blend
    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha
    };

    enum class BlendLogicOp
    {
        Clear,
        Set,
        Copy,
        CopyInverted,
        Noop,
        Invert,
        And,
        NotAnd,
        Or,
        NotOr,
        Xor,
        Equivalent,
        AndReverse,
        AndInverted,
        OrReverse,
        OrInverted
    };

    struct BlendFunctionInfo
    {
        std::uint32_t buffer_index;
        BlendFactor src_rgb;
        BlendFactor src_alpha;
        BlendFactor dst_rgb;
        BlendFactor dst_alpha;
    };

    enum class BlendEquation
    {
        Add,
        Subtract,
        ReverseSubstract,
        Min,
        Max
    };

    struct BlendEquationInfo
    {
        std::uint32_t buffer_index;
        BlendEquation eq_rgb;
        BlendEquation eq_alpha;
    };

    struct GraphicsPipelineBlendStateInfo
    {
        bool blend_enabled;
        std::span<BlendFunctionInfo> blend_function_info;
        bool logic_op_enabled;
        BlendLogicOp logic_op;
        float blend_color[4];
        std::span<BlendEquationInfo> blend_eq_info;
    };
    //~Blend

    //DepthStencil
    enum class StencilOp
    {
        Keep,
        Zero,
        Replace,
        Increment,
        IncrementWrap,
        Decrement,
        DecrementWrap,
        Invert
    };

    struct StencilStateOp
    {
        StencilOp fail_op;
        StencilOp pass_op;
        StencilOp depth_fail_op;
        CompareOp compare_op;
        std::int32_t reference;
        std::uint32_t compare_mask;
        std::uint32_t write_mask;
    };

    struct GraphicsPipelineDepthStencilStateInfo
    {
        bool depth_test_enabled;
        CompareOp depth_compare_op;
        bool write_enabled;
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
        std::span<std::uint32_t> sample_mask; //NULL, 1[1 - 32], 2[64]
        bool sample_shading_enabled;
        float min_sample_shading;
        bool alpha_to_coverage_enabled;
        bool alpha_to_one_enabled;
    };
    //~Multisample

    //Rasterization
    enum class PolygonMode
    {
        Point,
        Line,
        Fill
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class FrontFace
    {
        CounterClockwise,
        Clockwise
    };

    struct GraphicsPipelineRasterizationStateInfo
    {
        bool depth_clamp_enabled;
        bool rasterizer_discard_enabled;
        PolygonMode polygon_mode;
        CullMode cull_mode;
        FrontFace front_face;
        float line_width;
    };
    //Rasterization

    //Viewport
    struct GraphicsPipelineViewportStateInfo
    {
        bool viewport_enabled;
        std::span<Viewport> viewports;
        std::span<Rect2D> scissors;
    };
    //~Viewport

    //Draw
    enum class IndexType
    {
        u8,
        u16,
        u32
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

    struct GraphicsPipelineInfo
    {
        std::span<const Shader* const> shaders;
        GraphicsPipelineStateInfo state_info;
    };

    struct ComputePipelineInfo
    {
        const Shader* shader;
    };

#pragma message( \
    "Change UniformType and UniformExtent to single Format. -> so add R64G64B64A64 formats + RXGXBX formats also!")

    enum class UniformType
    {
        Float,
        Int,
        UInt,
        Double
    };

    enum class UniformExtent
    {
        Scalar,
        Vec2,
        Vec3,
        Vec4,
        Mat2x2,
        Mat2x3,
        Mat2x4,
        Mat3x2,
        Mat3x3,
        Mat3x4,
        Mat4x2,
        Mat4x3,
        Mat4x4,
    };

    enum class QueueSpecialization
    {
        Graphics,
        Compute,
        Transfer
    };

    struct UniformDesc
    {
        UniformType type;
        UniformExtent extent;
        std::uint32_t count;
        std::uint32_t location; //in std::uint32_t
    };

    struct PresentInfo
    {
        std::span<Semaphore*> wait_semaphores;
    };

    struct ContextProperties
    {
        std::string_view vendor_name;
        std::string_view device_name;
        std::span<std::string_view> extensions;
#pragma message("Add limits!!!")
    };

    enum DebugMessengerSeverityFlagBits
    {
        Verbose = 1 << 0, //GL_DEBUG_SEVERITY_NOTIFICATION
        Info = 1 << 1, //GL_DEBUG_SEVERITY_LOW
        Warning = 1 << 2, //GL_DEBUG_SEVERITY_MEDIUM
        Error = 1 << 3 //GL_DEBUG_SEVERITY_HIGH
    };

    using DebugMessengerSeverityFlags = std::underlying_type_t<DebugMessengerSeverityFlagBits>;

    enum DebugMessengerTypeFlagBits
    {
        General = 1 << 0, //GL_DEBUG_TYPE_PORTABILITY, GL_DEBUG_TYPE_OTHER
        Validation =
            1
            << 1, //GL_DEBUG_TYPE_ERROR, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
        Performance = 1 << 2 //GL_DEBUG_TYPE_PERFORMANCE,

        //GL_DEBUG_TYPE_MARKER, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_TYPE_POP_GROUP -> not presented
    };

    using DebugMessengerTypeFlags = std::underlying_type_t<DebugMessengerTypeFlagBits>;

    using DebugMessengerCallback = void(DebugMessengerSeverityFlagBits severity,
                                        DebugMessengerTypeFlags types,
                                        std::int64_t id,
                                        std::string_view message);

    struct DebugMessengerInfo
    {
        DebugMessengerSeverityFlags severities;
        DebugMessengerTypeFlags types;
        std::function<DebugMessengerCallback> callback;
    };

    bool IsFormatCompressed(Format format) noexcept;

    std::uint8_t GetFormatBlockSize(Format format) noexcept;

    std::uint16_t GetFormatBitsPerPixel(Format format) noexcept;

    std::uint32_t GetFormatRegionSize(Format format, const BufferImageCopyRegion& reg) noexcept;

    std::uint32_t GetFormatRegionSize(Format format, const MemoryImageCopyRegion& reg) noexcept;

    std::uint16_t GetFormatTexelAlignment(Format format) noexcept;

    struct FormatComponentsBitSize
    {
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
        std::uint8_t alpha;
    };

    FormatComponentsBitSize GetFormatComponentsBitSize(Format format) noexcept;
};