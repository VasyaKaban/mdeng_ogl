#pragma once

#include <cstdint>
#include <array>
#include <variant>
#include <span>
#include <string_view>
#include <functional>
#include <string>
#include "Core/Window/RenderBackend.h"

#pragma message("MOVE FORWARD POOL TO CORE!!! -> maybe get rid of hrs at all??? + allocators...")
#pragma message("MAKE COMMON NAMESPACES FOR CORE -> CORE or inner like Render???")
#pragma message( \
    "SPLIT GAME INTO DLL + ADD ENGINE EXE + ADD INTO PLUGINS INTERFACE AND RENDER INTERFACE START, STOP AND CREATE GLOBAL FUNCTIONS + RESOLVE!!!")
#pragma message("std::list and std::map TO CHAIN-LIKE STRUCTURES FOR EVENTS!!!")
#pragma message("LINK HRS AS STATIC WITH CORE ONLY!!!")

namespace Render
{
    class Context;
    class Object;
    class Buffer;
    class CommandBuffer;
    class CommandPool;
    class DescriptorPool;
    class DescriptorSet;
    class DescriptorSetLayout;
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
        std::uint64_t src_offset;
        std::uint64_t dst_offset;
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
        std::uint64_t offset;
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

    enum BufferUsageFlagBits
    {
        BufferUsageTransferSource = 1 << 0, //VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00'00'00'01,
        BufferUsageTransferDestination = 1 << 1, //VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00'00'00'02,
        //VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00'00'00'04,
        //VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00'00'00'08,
        BufferUsageUniformBuffer = 1 << 2, //VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00'00'00'10,
        BufferUsageStorageBuffer = 1 << 3, //VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00'00'00'20,
        BufferUsageIndexBuffer = 1 << 4, //VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00'00'00'40,
        BufferUsageVertexBuffer = 1 << 5, //VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00'00'00'80,
        //VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00'00'01'00,
    };

    using BufferUsageFlags = std::underlying_type_t<BufferUsageFlagBits>;

    enum class AllocateMemoryPolicy
    {
        AnyBits,
        OnlyBits
    };

    enum MemoryHeapFlagBits
    {
        DeviceLocalHeap = 1 << 0
    };

    using MemoryHeapFlags = std::underlying_type_t<MemoryHeapFlagBits>;

    enum MemoryTypePropertyFlagBits
    {
        DeviceLocal = 1 << 0, //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00'00'00'01, + 0
        HostMappingReadable = 1 << 1, //VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT + GL_MAP_READ,
        HostMappingWritable = 1 << 2, //VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT + GL_MAP_WRITE,
        HostMappingPersistent = 1 << 3, //VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT + GL_MAP_PERSISTENT,
        HostCoherent =
            1 << 4, //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00'00'00'04, + GL_MAPPING_COHERENT
        HostCached =
            1 << 5, //VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00'00'00'08, + GL_CLIENT_STORAGE
        //VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
    };

    using MemoryTypePropertyFlags = std::underlying_type_t<MemoryTypePropertyFlagBits>;

    struct MemoryType
    {
        MemoryHeapFlags memory_heap_flags;
        MemoryTypePropertyFlags memory_type_flags;
    };

    struct BufferInfo
    {
        std::uint64_t size;
        BufferUsageFlags usage;
    };

    struct MappedRange
    {
        std::uint64_t offset;
        std::uint64_t size;
    };

    enum CommandPoolFlagBits
    {
        TransientCommandPoolBit = 1 << 0,
        ResetableCommandPoolBit = 1 << 1
    };

    using CommandPoolFlags = std::underlying_type_t<CommandPoolFlagBits>;

    struct CommandPoolInfo
    {
        CommandPoolFlags flags;
        std::uint32_t queue_family_index;
    };

    enum CommandBufferUsageFlagBits
    {
        CommandBufferUsageOneTimeSubmitBit = 1 << 0
    };

    using CommandBufferUsageFlags = std::underlying_type_t<CommandBufferUsageFlagBits>;

    struct CommandBufferBeginInfo
    {
        CommandBufferUsageFlags flags;
    };

    enum CommandBufferResetFlagBits
    {
        CommandBufferResetReleaseResourcesBit = 1 << 0
    };

    using CommandBufferResetFlags = std::underlying_type_t<CommandBufferResetFlagBits>;

    struct CommandBufferResetInfo
    {
        CommandBufferResetFlags flags;
    };

    //We should start recording into command buffers in the way we want to see execution order of commands
    enum class CommandBufferStrategy
    {
        Deffered, //VK -> you can record command buffers from separate threads
        Immediate //OGL -> you can record command buffers only from one thread
    };

    enum class FenceStatus
    {
        Signaled,
        Unsignaled
    };

    struct FramebufferInfo
    {
        RenderPass* render_pass;
        std::span<const ImageView*> color_attachments;
        const ImageView* depth_stencil_attachment;
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

    enum ImageFlagBits
    {
        CubeCompatible = 1 << 0 //VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT = 0x00000010,
    };

    using ImageFlags = std::underlying_type_t<ImageFlagBits>;

    enum ImageUsageFlagBits
    {
        ImageUsageTransferSource = 1 << 0, //VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00'00'00'01,
        ImageUsageTransferDestination = 1 << 1, //VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00'00'00'02,
        ImageUsageSampled = 1 << 2, //VK_IMAGE_USAGE_SAMPLED_BIT = 0x00'00'00'04,
        ImageUsageStorage = 1 << 3, //VK_IMAGE_USAGE_STORAGE_BIT = 0x00'00'00'08,
        ImageUsagecolorAttachment = 1 << 4, //VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00'00'00'10,
        ImageUsageDepthStencilAttachment =
            1 << 5, //VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00'00'00'20,
        //VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00'00'00'40,
        //VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00'00'00'80,
    };

    using ImageUsageFlags = std::underlying_type_t<ImageUsageFlagBits>;

    struct ImageInfo
    {
        //std::span<const MemoryRequest> memory_requests;//only allocate in device-local memory first...
        ImageFlags flags;
        ImageType image_type;
        Format format;
        Extent3D extent;
        std::uint32_t mip_levels;
        std::uint32_t array_layers;
        SampleCount samples;
        ImageUsageFlags usage;
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

    enum class ImageLayout
    {
        Undefined, //VK_IMAGE_LAYOUT_UNDEFINED = 0, -> on image init
        General, //VK_IMAGE_LAYOUT_GENERAL = 1, -> after image init
        ColorAttachmentOptimal, //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
        DepthStencilAttachmentOptimal, //VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
        DepthStencilReadOnlyOptimal, //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
        ShaderReadOnlyOptimal, //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
        TransferSourceOptimal, //VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
        TransferDestinationOptimal, //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
        PresentSource, //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR -> for renderpass + default framebuffer
    };

    enum PipelineStageFlagBits
    {
        TopOfPipePipelineStageBit = 1 << 0, //VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00'00'00'01,
        DrawIndirectPipelineStageBit = 1
                                       << 1, //VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00'00'00'02,
        VeretxInputPipelineStageBit = 1 << 2, //VK_PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00'00'00'04,
        VertexShaderPipelineStageBit = 1
                                       << 3, //VK_PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00'00'00'08,
        TessellationControlShaderPipelineStageBit =
            1 << 4, //VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00'00'00'10,
        TessellationEvaluationShaderPipelineStageBit =
            1 << 5, // VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00'00'00'20,
        GeometryShaderPipelineStageBit =
            1 << 6, // VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00'00'00'40,
        FragmentShaderPipelineStageBit =
            1 << 7, // VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00'00'00'80,
        EarlyFragmentTestsPipelineStageBit =
            1 << 8, // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00'00'01'00,
        LateFragmentTestsPipelineStageBit =
            1 << 9, // VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00'00'02'00,
        ColorAttachmentOutputPipelineStageBit =
            1 << 10, // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00'00'04'00,
        ComputeShaderPipelineStageBit =
            1 << 11, // VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00'00'08'00,
        TransferPipelineStageBit = 1 << 12, // VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00'00'10'00,
        BottomOfPipePipelineStageBit =
            1 << 13, // VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00'00'20'00,
        HostPipelineStageBit = 1 << 14, // VK_PIPELINE_STAGE_HOST_BIT = 0x00'00'40'00,
        AllGraphicsPipelineStageBit = 1
                                      << 15, // VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT = 0x00'00'80'00,
        AllCommandsPipelineStageBit = 1
                                      << 16, // VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00'01'00'00,
    };

    using PipelineStageFlags = std::underlying_type_t<PipelineStageFlagBits>;

    enum DependencyFlagBits
    {
        ByRegion = 1 << 0
    };

    using DependencyFlags = std::underlying_type_t<DependencyFlagBits>;

    enum AccessFlagBits
    {
        AccessIndirectCommandReadBit = 1
                                       << 0, //VK_ACCESS_INDIRECT_COMMAND_READ_BIT = 0x00'00'00'01,
        AccessIndexReadBit = 1 << 1, //VK_ACCESS_INDEX_READ_BIT = 0x00'00'00'02,
        AccessVertexAttributeReadBit = 1
                                       << 2, //VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT = 0x00'00'00'04,
        AccessUniformReadBit = 1 << 3, //VK_ACCESS_UNIFORM_READ_BIT = 0x00'00'00'08,
        //AccessInputAttachmentReadBit = 1 << 4, //VK_ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00'00'00'10,
        AccessShaderReadBit = 1 << 5, //VK_ACCESS_SHADER_READ_BIT = 0x00'00'00'20,
        AccessShaderWriteBit = 1 << 6, //VK_ACCESS_SHADER_WRITE_BIT = 0x00'00'00'40,
        AccessColorAttachmentReadBit = 1
                                       << 7, //VK_ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00'00'00'80,
        AccessColorAttachmentWriteBit =
            1 << 8, //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00'00'01'00,
        AccessDepthStencilAttachmentReadBit =
            1 << 9, //VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00'00'02'00,
        AccessDepthStencilAttachmentWriteBit =
            1 << 10, //VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00'00'04'00,
        AccessTransferReadBit = 1 << 11, //VK_ACCESS_TRANSFER_READ_BIT = 0x00'00'08'00,
        AccessTransferWiteBit = 1 << 12, //VK_ACCESS_TRANSFER_WRITE_BIT = 0x00'00'10'00,
        AccessHostReadBit = 1 << 13, //VK_ACCESS_HOST_READ_BIT = 0x00'00'20'00,
        AccessHostWriteBit = 1 << 14, //VK_ACCESS_HOST_WRITE_BIT = 0x00'00'40'00,
        AccessMemoryReadBit = 1 << 15, //VK_ACCESS_MEMORY_READ_BIT = 0x00'00'80'00,
        AccessMemoryWriteBit = 1 << 16, //VK_ACCESS_MEMORY_WRITE_BIT = 0x00'01'00'00,
    };

    using AccessFlags = std::underlying_type_t<AccessFlagBits>;

    struct AttachmentDescription
    {
        bool clear_load;
        Format format; //wtf
        SampleCount samples; ///wtf???
        ImageLayout initial_layout;
        ImageLayout final_layout;
    };

    struct RenderPassDependency
    {
        PipelineStageFlags src_stages;
        PipelineStageFlags dst_stages;
        AccessFlags src_access;
        AccessFlags dst_access;
        DependencyFlags dependency;
    };

    struct RenderPassInfo
    {
        std::span<const AttachmentDescription> color_attachment_descriptions;
        const AttachmentDescription* depth_stencil_attachment_description;
        RenderPassDependency early_external_dependency;
        RenderPassDependency late_external_dependency;
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

    enum ShaderSyntaxFlagBits
    {
        GLSL = 1 << 0
    };

    using ShaderSyntaxFlags = std::underlying_type_t<ShaderSyntaxFlagBits>;

    enum ShaderStageFlagBits
    {
        Vertex = 1 << 0, //VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
        TessellationControl = 1 << 1, //VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
        TessellationEvaluation = 1 << 2, //VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
        Geometry = 1 << 3, //VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
        Fragment = 1 << 4, //VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
        Compute = 1 << 5, //VK_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
        AllGraphics = 1 << 6, //VK_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
        AllStages = 1 << 7, //VK_SHADER_STAGE_ALL = 0x7FFFFFFF,
    };

    using ShaderStageFlags = std::underlying_type_t<ShaderStageFlagBits>;

    struct ShaderInfo
    {
        ShaderSyntaxFlagBits syntax;
        ShaderStageFlagBits stage;
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

    //Tessellation
    struct GraphicsPipelineTessellationStateInfo
    {
        std::uint32_t patch_control_points;
    };
    //~Tessellation

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

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubstract,
        Min,
        Max
    };

    struct ColorBlendAttachmentState
    {
        bool blend_enabled;
        BlendFactor src_color_blend_factor;
        BlendFactor dst_color_blend_factor;
        BlendOp color_blend_op;
        BlendFactor src_alpha_blend_factor;
        BlendFactor dst_alpha_blend_factor;
        BlendOp alpha_blend_op;
    };

    struct GraphicsPipelineColorBlendStateInfo
    {
        bool logic_op_enabled;
        BlendLogicOp logic_op;
        std::span<const ColorBlendAttachmentState> attachments;
        float blend_color[4];
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
        std::uint32_t compare_mask;
        std::uint32_t write_mask;
        std::int32_t reference;
    };

    struct GraphicsPipelineDepthStencilStateInfo
    {
        bool depth_test_enabled;
        bool depth_write_enabled;
        CompareOp depth_compare_op;
        bool stencil_test_enabled;
        StencilStateOp stencil_front_op;
        StencilStateOp stencil_back_op;
    };
    //~DepthStencil

    //Multisample
    struct GraphicsPipelineMultisampleStateInfo
    {
        SampleCount sample_count; //SampleCount_1 -> msaa disabled
        bool sample_shading_enabled;
        float min_sample_shading;
        std::span<std::uint32_t> sample_mask; //NULL, 1[1 - 32], 2[64]
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
        std::uint32_t count;
        std::span<Viewport> predefined_viewports;
        std::span<Rect2D> predefined_scissors;
    };
    //~Viewport

    //Draw
    enum class IndexType
    {
        U8,
        U16,
        U32
    };
    //~Draw

    struct UniformRange
    {
        ShaderStageFlags stages;
        std::uint32_t words_offset;
        std::uint32_t words_size;
    };

    enum class DescriptorType
    {
        //VK_DESCRIPTOR_TYPE_SAMPLER = 0,
        CombinedImageSampler, //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        //VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        StorageImage, //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        //VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        //VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        UnifromBuffer, //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        StorageBuffer, //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        //VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    };

    struct DescriptorBinding
    {
        std::uint32_t set_binding;
        std::uint32_t linear_binding;
    };

    struct DescriptorSetLayoutLayoutBinding
    {
        DescriptorBinding binding;
        DescriptorType type;
        ShaderStageFlags stages;
    };

    struct DescriptorSetLayoutInfo
    {
        std::span<const DescriptorSetLayoutLayoutBinding> bindings;
    };

    struct DescriptorPoolSize
    {
        DescriptorType type;
        std::uint32_t count;
    };

    struct DescriptorPoolInfo
    {
        std::uint32_t max_sets;
        std::span<const DescriptorPoolSize> sizes;
    };

    struct DescriptorSetAllocateInfo
    {
        const DescriptorSetLayout* layout;
    };

    struct DescriptorImageDesc
    {
        Sampler* sampler;
        ImageView* image_view;
        ImageLayout layout;
    };

    struct DescriptorBufferDesc
    {
        Buffer* buffer;
        std::uint64_t offset;
        std::uint64_t size;
    };

    struct UpdateDescriptorDesc
    {
        DescriptorBinding binding;
        DescriptorType type;
        union
        {
            DescriptorImageDesc image_desc;
            DescriptorBufferDesc buffer_desc;
        } desc;
    };

    enum class DynamicState
    {
        Viewport,
        Scissors
    };

    struct GraphicsPipelineStateInfo
    {
        GraphicsPipelineVertexInputStateInfo vertex_input_state_info;
        GraphicsPipelineInputAssemblyStateInfo input_assembly_state_info;
        GraphicsPipelineTessellationStateInfo tessellation_state_info;
        GraphicsPipelineColorBlendStateInfo color_blend_state_info;
        GraphicsPipelineDepthStencilStateInfo depth_stencil_state_info;
        GraphicsPipelineMultisampleStateInfo multisample_state_info;
        GraphicsPipelineRasterizationStateInfo rasterization_state_info;
        GraphicsPipelineViewportStateInfo viewport_state_info;
        std::span<const UniformRange> uniform_ranges;
        std::span<const DescriptorSetLayout> descriptor_set_layouts;
        std::span<DynamicState> dynamic_states;
    };

    struct GraphicsPipelineInfo
    {
        std::span<const Shader* const> shaders;
        GraphicsPipelineStateInfo state_info;
        RenderPass* render_pass;
    };

    struct ComputePipelineInfo
    {
        const Shader* shader;
    };

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

    struct UniformDesc
    {
        UniformType type;
        UniformExtent extent;
        std::uint32_t count;
        std::uint32_t location;
    };

    struct PresentInfo
    {
        std::span<Semaphore*> wait_semaphores;
    };

    struct BufferMemoryBarrier
    {
        AccessFlags src_access;
        AccessFlags dst_access;
        std::uint32_t src_queue_family_index;
        std::uint32_t dst_queue_family_index;
        Buffer* buffer;
        std::uint64_t offset;
        std::uint64_t size;
    };

    struct ImageMemoryBarrier
    {
        AccessFlags src_access;
        AccessFlags dst_access;
        ImageLayout old_layout;
        ImageLayout new_layout;
        std::uint32_t src_queue_family_index;
        std::uint32_t dst_queue_family_index;
        Image* image;
        ImageSubresourceRange subresource_range;
    };

    struct PipelineBarrier
    {
        PipelineStageFlags src_stages;
        PipelineStageFlags dst_stages;
        DependencyFlags dependency;
        std::span<BufferMemoryBarrier> buffer_barriers;
        std::span<ImageMemoryBarrier> image_barriers;
    };

    enum QueueSpecializationFlagBits
    {
        TransferSpec = 1 << 0,
        GraphicsSpec = 1 << 1,
        ComputeSpec = 1 << 2,
        PresentSpec = 1 << 3
    };

    using QueueSpecializationFlags = std::underlying_type_t<QueueSpecializationFlagBits>;

    struct QueueFamilyProperties
    {
        QueueSpecializationFlags specialization;
        std::uint32_t queue_count;
    };

    struct DrawIndirectCommand
    {
        std::uint32_t vertex_count;
        std::uint32_t instance_count;
        std::uint32_t first_vertex;
        std::uint32_t first_instance;
    };

    struct DrawIndexedIndirectCommand
    {
        std::uint32_t index_count;
        std::uint32_t instance_count;
        std::uint32_t first_index;
        std::int32_t vertex_offset;
        std::uint32_t first_instance;
    };

    struct DispatchIndirectCommand
    {
        std::uint32_t x_groups;
        std::uint32_t y_groups;
        std::uint32_t z_groups;
    };

    constexpr std::uint32_t MakeVersion(std::uint16_t major, std::uint16_t minor) noexcept
    {
        return static_cast<std::uint32_t>(major) << 16 | minor;
    }

    constexpr std::uint16_t GetMajorVersion(std::uint32_t version) noexcept
    {
        return version >> 16;
    }

    constexpr std::uint16_t GetMinorVersion(std::uint32_t version) noexcept
    {
        return version & 0xFF'FF;
    }

    struct ContextProperties
    {
        std::string context_name;
        RenderBackendType supported_backend_type;
        std::uint32_t version; //major << 16 | minor
        std::string_view vendor_name;
        std::string_view device_name;
        std::vector<std::string> extensions;
        ShaderSyntaxFlags supported_syntax;
        std::vector<QueueFamilyProperties> queue_family_properties;
        std::vector<MemoryType> memory_types;
        CommandBufferStrategy command_buffer_strategy;
        //If usage included VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment.
        //If usage included VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minStorageBufferOffsetAlignment.
#pragma message("Add limits and features!!!")
    };

    struct QueueFamilyInfo
    {
        std::uint32_t index;
        std::uint32_t queue_count;
        std::span<const float> queue_priorities;
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

    class Resolve;

    struct SelectedContextDesc
    {
        std::uint32_t index;
        std::vector<Render::QueueFamilyInfo> queue_family_infos;
    };

    constexpr inline auto RENDER_RESOLVE_FUNCTION_NAME = "RenderResolve";
    using PFN_RenderResolve = Resolve* (*)();
};