#pragma once

#include <cstdint>
#include <array>
#include <variant>
#include <span>
#include <string_view>
#include <functional>
#include <string>
#include "Core/Window/RenderBackend.h"

namespace Render
{
    class Context;
    class Object;
    class Buffer;
    class BufferView;
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

    constexpr inline std::uint32_t QUEUE_FAMILY_IGNORED = ~0U;

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

    enum SampleCount
    {
        SampleCount_1 = 1,
        SampleCount_2 = 2,
        SampleCount_4 = 4,
        SampleCount_8 = 8,
        SampleCount_16 = 16,
        SampleCount_32 = 32,
        SampleCount_64 = 64
    };

    using SampleCountFlags = std::underlying_type_t<SampleCount>;

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

    struct ClearColorValue
    {
        float float32[4];
        std::int32_t int32[4];
        std::uint32_t uint32[4];
    };

    struct BufferCopyRegion
    {
        std::uint64_t src_offset;
        std::uint64_t dst_offset;
        std::uint64_t size;
    };

    enum ImageAspectFlagBits
    {
        AspectColorBit = 1 << 0,
        AspectDepthBit = 1 << 1,
        AspectStencilBit = 1 << 2
    };

    using ImageAspectFlags = std::underlying_type_t<ImageAspectFlagBits>;

    struct ImageSubresourceLayers
    {
        ImageAspectFlags aspect;
        std::uint32_t mip_level;
        std::uint32_t base_layer;
        std::uint32_t layer_count;
    };

    //no aspect mask -> only copy color images!
    struct BufferImageCopyRegion
    {
        std::uint64_t buffer_offset;
        std::uint32_t buffer_row_length;
        std::uint32_t buffer_image_height;
        ImageSubresourceLayers subresource_layers;
        Offset3D offset;
        Extent3D extent;
    };

    //no aspect mask -> only copy color images!
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

    struct ClearDepthStencilValue
    {
        float depth;
        std::uint32_t stencil;
    };

    struct RenderPassBeginInfo
    {
        Framebuffer* framebuffer;
        Rect2D render_area;
        std::span<const ClearColorValue> clear_color_values;
        ClearDepthStencilValue clear_depth_stencil_value;
    };

    enum BufferUsageFlagBits
    {
        BufferUsageTransferSource = 1 << 0, //VK_BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00'00'00'01,
        BufferUsageTransferDestination = 1 << 1, //VK_BUFFER_USAGE_TRANSFER_DST_BIT = 0x00'00'00'02,
        BufferUsageUniformTexelBuffer =
            1 << 2, //VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00'00'00'04,
        BufferUsageStorageTexelBuffer =
            1 << 3, //VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00'00'00'08,
        BufferUsageUniformBuffer = 1 << 3, //VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00'00'00'10,
        BufferUsageStorageBuffer = 1 << 4, //VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00'00'00'20,
        BufferUsageIndexBuffer = 1 << 5, //VK_BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00'00'00'40,
        BufferUsageVertexBuffer = 1 << 6, //VK_BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00'00'00'80,
        BufferUsageIndirectBuffer = 1 << 7 //VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00'00'01'00,
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
        HostCoherent =
            1 << 3, //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00'00'00'04, + GL_MAPPING_COHERENT
        HostCached =
            1 << 4, //VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00'00'00'08, + GL_CLIENT_STORAGE
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

    enum class BufferMapContentPolicy
    {
        None,
        Discard //-> OGL: GL_MAP_INVALIDATE_RANGE_BIT ; VK: None
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
        std::span<const ImageView*> input_attachments;
        std::span<const ImageView*> color_attachments;
        const ImageView* depth_stencil_attachment;
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t layers;
    };

    enum class ImageType
    {
        Image1D,
        Image2D,
        Image3D,
        CubeMap
    };

    //formnat: LSB -> MSB
    enum class Format
    {
        UNDEFINED,
        R4G4_UNORM_PACK8,
        R4G4B4A4_UNORM_PACK16,
        B4G4R4A4_UNORM_PACK16,
        R5G6B5_UNORM_PACK16,
        B5G6R5_UNORM_PACK16,
        R5G5B5A1_UNORM_PACK16,
        B5G5R5A1_UNORM_PACK16,
        A1R5G5B5_UNORM_PACK16,
        R8_UNORM,
        R8_SNORM,
        R8_USCALED,
        R8_SSCALED,
        R8_UINT,
        R8_SINT,
        R8_UNORM_SRGB,
        R8G8_UNORM,
        R8G8_SNORM,
        R8G8_USCALED,
        R8G8_SSCALED,
        R8G8_UINT,
        R8G8_SINT,
        R8G8_UNORM_SRGB,
        R8G8B8_UNORM,
        R8G8B8_SNORM,
        R8G8B8_USCALED,
        R8G8B8_SSCALED,
        R8G8B8_UINT,
        R8G8B8_SINT,
        R8G8B8_UNORM_SRGB,
        B8G8R8_UNORM,
        B8G8R8_SNORM,
        B8G8R8_USCALED,
        B8G8R8_SSCALED,
        B8G8R8_UINT,
        B8G8R8_SINT,
        B8G8R8_UNORM_SRGB,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_USCALED,
        R8G8B8A8_SSCALED,
        R8G8B8A8_UINT,
        R8G8B8A8_SINT,
        R8G8B8A8_UNORM_SRGB,
        B8G8R8A8_UNORM,
        B8G8R8A8_SNORM,
        B8G8R8A8_USCALED,
        B8G8R8A8_SSCALED,
        B8G8R8A8_UINT,
        B8G8R8A8_SINT,
        B8G8R8A8_UNORM_SRGB,
        A8B8G8R8_UNORM_PACK32,
        A8B8G8R8_SNORM_PACK32,
        A8B8G8R8_USCALED_PACK32,
        A8B8G8R8_SSCALED_PACK32,
        A8B8G8R8_UINT_PACK32,
        A8B8G8R8_SINT_PACK32,
        A8B8G8R8_UNORM_SRGB_PACK32,
        A2R10G10B10_UNORM_PACK32,
        A2R10G10B10_SNORM_PACK32,
        A2R10G10B10_USCALED_PACK32,
        A2R10G10B10_SSCALED_PACK32,
        A2R10G10B10_UINT_PACK32,
        A2R10G10B10_SINT_PACK32,
        A2B10G10R10_UNORM_PACK32,
        A2B10G10R10_SNORM_PACK32,
        A2B10G10R10_USCALED_PACK32,
        A2B10G10R10_SSCALED_PACK32,
        A2B10G10R10_UINT_PACK32,
        A2B10G10R10_SINT_PACK32,
        R16_UNORM,
        R16_SNORM,
        R16_USCALED,
        R16_SSCALED,
        R16_UINT,
        R16_SINT,
        R16_SFLOAT,
        R16G16_UNORM,
        R16G16_SNORM,
        R16G16_USCALED,
        R16G16_SSCALED,
        R16G16_UINT,
        R16G16_SINT,
        R16G16_SFLOAT,
        R16G16B16_UNORM,
        R16G16B16_SNORM,
        R16G16B16_USCALED,
        R16G16B16_SSCALED,
        R16G16B16_UINT,
        R16G16B16_SINT,
        R16G16B16_SFLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
        R16G16B16A16_USCALED,
        R16G16B16A16_SSCALED,
        R16G16B16A16_UINT,
        R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT,
        R32_UINT,
        R32_SINT,
        R32_SFLOAT,
        R32G32_UINT,
        R32G32_SINT,
        R32G32_SFLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R32G32B32_SFLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT,
        R64_UINT,
        R64_SINT,
        R64_SFLOAT,
        R64G64_UINT,
        R64G64_SINT,
        R64G64_SFLOAT,
        R64G64B64_UINT,
        R64G64B64_SINT,
        R64G64B64_SFLOAT,
        R64G64B64A64_UINT,
        R64G64B64A64_SINT,
        R64G64B64A64_SFLOAT,
        B10G11R11_UFLOAT_PACK32,
        E5B9G9R9_UFLOAT_PACK32,
        D16_UNORM,
        X8_D24_UNORM_PACK32,
        D32_SFLOAT,
        S8_UINT,
        D16_UNORM_S8_UINT,
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT,
        BC1_RGB_UNORM_BLOCK,
        BC1_RGB_UNORM_SRGB_BLOCK,
        BC1_RGBA_UNORM_BLOCK,
        BC1_RGBA_UNORM_SRGB_BLOCK,
        BC2_UNORM_BLOCK,
        BC2_UNORM_SRGB_BLOCK,
        BC3_UNORM_BLOCK,
        BC3_UNORM_SRGB_BLOCK,
        BC4_UNORM_BLOCK,
        BC4_SNORM_BLOCK,
        BC5_UNORM_BLOCK,
        BC5_SNORM_BLOCK,
        BC6H_UFLOAT_BLOCK,
        BC6H_SFLOAT_BLOCK,
        BC7_UNORM_BLOCK,
        BC7_UNORM_SRGB_BLOCK,
        ETC2_R8G8B8_UNORM_BLOCK,
        ETC2_R8G8B8_UNORM_SRGB_BLOCK,
        ETC2_R8G8B8A1_UNORM_BLOCK,
        ETC2_R8G8B8A1_UNORM_SRGB_BLOCK,
        ETC2_R8G8B8A8_UNORM_BLOCK,
        ETC2_R8G8B8A8_UNORM_SRGB_BLOCK,
        EAC_R11_UNORM_BLOCK,
        EAC_R11_SNORM_BLOCK,
        EAC_R11G11_UNORM_BLOCK,
        EAC_R11G11_SNORM_BLOCK,

        // Provided by VK_VERSION_1_3 or EXT
        A4R4G4B4_UNORM_PACK16,
        A4B4G4R4_UNORM_PACK16,

        // Provided by VK_VERSION_1_4 or VK_KHR_maintenance5
        A1B5G5R5_UNORM_PACK16,
        A8_UNORM,

        //ASTC
        /*ASTC_4x4_UNORM_BLOCK,
        ASTC_4x4_SRGB_BLOCK,
        ASTC_5x4_UNORM_BLOCK,
        ASTC_5x4_SRGB_BLOCK,
        ASTC_5x5_UNORM_BLOCK,
        ASTC_5x5_SRGB_BLOCK,
        ASTC_6x5_UNORM_BLOCK,
        ASTC_6x5_SRGB_BLOCK,
        ASTC_6x6_UNORM_BLOCK,
        ASTC_6x6_SRGB_BLOCK,
        ASTC_8x5_UNORM_BLOCK,
        ASTC_8x5_SRGB_BLOCK,
        ASTC_8x6_UNORM_BLOCK,
        ASTC_8x6_SRGB_BLOCK,
        ASTC_8x8_UNORM_BLOCK,
        ASTC_8x8_SRGB_BLOCK,
        ASTC_10x5_UNORM_BLOCK,
        ASTC_10x5_SRGB_BLOCK,
        ASTC_10x6_UNORM_BLOCK,
        ASTC_10x6_SRGB_BLOCK,
        ASTC_10x8_UNORM_BLOCK,
        ASTC_10x8_SRGB_BLOCK,
        ASTC_10x10_UNORM_BLOCK,
        ASTC_10x10_SRGB_BLOCK,
        ASTC_12x10_UNORM_BLOCK,
        ASTC_12x10_SRGB_BLOCK,
        ASTC_12x12_UNORM_BLOCK,
        ASTC_12x12_SRGB_BLOCK,*/

        // Provided by VK_VERSION_1_3 + or EXT + ASTC
        /*ASTC_4x4_SFLOAT_BLOCK,
        ASTC_5x4_SFLOAT_BLOCK,
        ASTC_5x5_SFLOAT_BLOCK,
        ASTC_6x5_SFLOAT_BLOCK,
        ASTC_6x6_SFLOAT_BLOCK,
        ASTC_8x5_SFLOAT_BLOCK,
        ASTC_8x6_SFLOAT_BLOCK,
        ASTC_8x8_SFLOAT_BLOCK,
        ASTC_10x5_SFLOAT_BLOCK,
        ASTC_10x6_SFLOAT_BLOCK,
        ASTC_10x8_SFLOAT_BLOCK,
        ASTC_10x10_SFLOAT_BLOCK,
        ASTC_12x10_SFLOAT_BLOCK,
        ASTC_12x12_SFLOAT_BLOCK,*/

        // Provided by VK_IMG_format_pvrtc
        /*PVRTC1_2BPP_UNORM_BLOCK_IMG,
        PVRTC1_4BPP_UNORM_BLOCK_IMG,
        PVRTC2_2BPP_UNORM_BLOCK_IMG,
        PVRTC2_4BPP_UNORM_BLOCK_IMG,
        PVRTC1_2BPP_SRGB_BLOCK_IMG,
        PVRTC1_4BPP_SRGB_BLOCK_IMG,
        PVRTC2_2BPP_SRGB_BLOCK_IMG,
        PVRTC2_4BPP_SRGB_BLOCK_IMG,*/
    };

    enum ImageUsageFlagBits
    {
        ImageUsageTransferSource = 1 << 0, //VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00'00'00'01,
        ImageUsageTransferDestination = 1 << 1, //VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00'00'00'02,
        ImageUsageSampled = 1 << 2, //VK_IMAGE_USAGE_SAMPLED_BIT = 0x00'00'00'04,
        ImageUsageStorage = 1 << 3, //VK_IMAGE_USAGE_STORAGE_BIT = 0x00'00'00'08,
        ImageUsageColorAttachment = 1 << 4, //VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00'00'00'10,
        ImageUsageDepthStencilAttachment =
            1 << 5, //VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00'00'00'20,
        //VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00'00'00'40,
        ImageUsageInputAttachment = 1 << 6 //VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00'00'00'80,
    };

    using ImageUsageFlags = std::underlying_type_t<ImageUsageFlagBits>;

    struct ImageInfo
    {
        //std::span<const MemoryRequest> memory_requests;//only allocate in device-local memory first...
        ImageType image_type;
        Format format;
        Extent3D extent;
        std::uint32_t mip_levels;
        std::uint32_t
            array_layers; //for cube map: 1 layer -> plain cubemap with 6 inner layers, 2 or more layers -> cube map array with array_layers*6 inner layers
        SampleCount samples;
        ImageUsageFlags usage;
    };

    enum FormatFeatureFlagBits
    {
        FormatFeatureSampledImageBit = 1
                                       << 0, //VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT = 0x00'00'00'01,
        FormatFeatureStorageImageBit = 1
                                       << 1, //VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT = 0x00'00'00'02,
        FormatFeatureStorageImageAtomicBit =
            1 << 2, //VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT = 0x00'00'00'04,
        FormatFeatureUniformTexelBufferBit =
            1 << 3, //VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT = 0x00'00'00'08,
        FormatFeatureStorageTexelBufferBit =
            1 << 4, //VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT = 0x00'00'00'10,
        FormatFeatureStorageTexelBufferAtomicAtomicBit =
            1 << 5, //VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT = 0x00'00'00'20,
        FormatFeatureVertexBufferBit = 1
                                       << 6, //VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT = 0x00'00'00'40,
        FormatFeatureColorAttachmentBit =
            1 << 7, //VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT = 0x00'00'00'80,
        FormatFeatureColorAttachmentBlendBit =
            1 << 8, //VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT = 0x00'00'01'00,
        FormatFeatureDepthStencilAttachmentBit =
            1 << 9, //VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00'00'02'00,
        FormatFeatureSampledImageFilterLinearBit =
            1 << 10 //VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x00'00'10'00,

        // Provided by VK_VERSION_1_1
        //VK_FORMAT_FEATURE_TRANSFER_SRC_BIT = 0x00'00'40'00,
        // Provided by VK_VERSION_1_1
        //VK_FORMAT_FEATURE_TRANSFER_DST_BIT = 0x00'00'80'00,
    };

    using FormatFeatureFlags = std::underlying_type_t<FormatFeatureFlagBits>;

    struct BufferFormatInfo
    {
        Format format;
    };

    struct BufferFormatProperties
    {
        FormatFeatureFlags features;
    };

    struct ImageFormatInfo
    {
        Format format;
        ImageType type;
        bool layered;
        bool sampled;
        ImageUsageFlags usage;
    };

    struct ImageFormatProperties
    {
        Extent3D max_extent;
        std::uint32_t max_mip_levels;
        std::uint32_t max_array_layers;
        SampleCountFlags sample_count;
        FormatFeatureFlags features;
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
        ImageAspectFlags aspect;
        std::uint32_t min_mip_level;
        std::uint32_t mip_level_count;
        std::uint32_t min_layer;
        std::uint32_t layer_count;
    };

    struct ImageViewInfo
    {
        //no format -> we do not use mutable format
        const Image* image;
        ImageViewType view_type;
        ComponentMapping components;
        ImageSubresourceRange subresource_range;
    };

    struct BufferViewInfo
    {
        Buffer* buffer;
        Format format;
        std::uint64_t offset;
        std::uint64_t size;
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
        VertexInputPipelineStageBit = 1 << 2, //VK_PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00'00'00'04,
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

    enum class AttachmentLoadOp
    {
        Load, //VK_ATTACHMENT_LOAD_OP_LOAD = 0,
        Clear, //VK_ATTACHMENT_LOAD_OP_CLEAR = 1,
        DontCare //VK_ATTACHMENT_LOAD_OP_DONT_CARE = 2, -> same as Load for OGL
    };

    enum class AttachmentStoreOp
    {
        Store, //VK_ATTACHMENT_STORE_OP_STORE = 0,
        DontCare //VK_ATTACHMENT_STORE_OP_DONT_CARE = 1,
    };

    struct ColorAttachment
    {
        Format format;
        SampleCount samples;
        AttachmentLoadOp load_op;
        AttachmentStoreOp store_op;
        ImageLayout initial_layout;
        ImageLayout final_layout;
    };

    struct DepthStencilAttachment
    {
        Format format;
        SampleCount samples;
        AttachmentLoadOp load_op;
        AttachmentStoreOp store_op;
        AttachmentLoadOp stencil_load_op;
        AttachmentStoreOp stencil_store_op;
        ImageLayout initial_layout;
        ImageLayout final_layout;
    };

    struct InputAttachment
    {
        Format format;
        SampleCount samples;
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
        std::span<const InputAttachment> input_attachments;
        std::span<const ColorAttachment> color_attachments;
        const DepthStencilAttachment* depth_stencil_attachment;
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

    enum class BorderColor
    {
        TransparentBlackFloat, //VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
        TransparentBlackInt, //VK_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
        OpaqueBlackFloat, //VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
        OpaqueBlackInt, //VK_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
        OpaqueWhiteFloat, //VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
        OpaqueWhiteInt, //VK_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
        CustomFloat, //VK_BORDER_COLOR_FLOAT_CUSTOM_EXT = 1'000'287'003,
        CustomInt, //VK_BORDER_COLOR_INT_CUSTOM_EXT = 1'000'287'004,
    };

    struct SamplerCustomBorderColorInfo
    {
        ClearColorValue color;
        Format format;
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
        SamplerCustomBorderColorInfo custom_border_color_info;
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

    enum ColorComponentFlagBits
    {
        ColorComponentRed = 1 << 0,
        ColorComponentGreen = 1 << 1,
        ColorComponentBlue = 1 << 2,
        ColorComponentAlpha = 1 << 3
    };

    using ColorComponentFlags = std::underlying_type_t<ColorComponentFlagBits>;

    struct ColorBlendAttachmentState
    {
        bool blend_enabled;
        BlendFactor src_color_blend_factor;
        BlendFactor dst_color_blend_factor;
        BlendOp color_blend_op;
        BlendFactor src_alpha_blend_factor;
        BlendFactor dst_alpha_blend_factor;
        BlendOp alpha_blend_op;
        ColorComponentFlags color_write_mask;
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
        bool depth_bounds_test_enabled;
        float min_depth_bounds;
        float max_depth_bounds;
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
        bool depth_bias_enabled;
        float depth_bias_constant_factor;
        float depth_bias_clamp;
        float depth_bias_slope_factor;
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
        SampledImage, //VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        StorageImage, //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        UniformTexelBuffer, //VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        StorageTexelBuffer, //VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        UnifromBuffer, //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        StorageBuffer, //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        InputAttachment //VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    };

    struct DescriptorSetLayoutBinding
    {
        std::uint32_t binding;
        DescriptorType type;
        std::uint32_t descriptor_count;
        ShaderStageFlags stages;
    };

    struct DescriptorSetLayoutInfo
    {
        std::span<const DescriptorSetLayoutBinding> bindings;
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
        std::uint32_t binding;
        std::uint32_t array_index;
        std::uint32_t descriptor_count;
        DescriptorType type;
        union
        {
            DescriptorImageDesc* image_desc;
            DescriptorBufferDesc* buffer_desc;
            BufferView** texel_buffer_view;
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
        std::uint32_t offset;
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
        std::span<const BufferMemoryBarrier> buffer_barriers;
        std::span<const ImageMemoryBarrier> image_barriers;
    };

    //In case if we want to get a DMA-transfer queue we can get a transfer wuqu with video encode/decode in vulkan that is obviously not a DMA queue -> so we add a new spec 'UnknownImplementationSpec' that will signal that current queue family also support other spec(this spec do not include sparse binding)
    enum QueueSpecializationFlagBits
    {
        TransferSpec = 1 << 0,
        GraphicsSpec = 1 << 1,
        ComputeSpec = 1 << 2,
        PresentSpec = 1 << 3,
        UnknownImplementationSpec = 1 << 4
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

    struct QueueInfo
    {
        std::uint32_t family_index;
        std::uint32_t index;
    };

    struct ContextLimits
    {
        uint32_t maxImageDimension1D; //GL_MAX_TEXTURE_SIZE
        uint32_t maxImageDimension2D; //GL_MAX_TEXTURE_SIZE
        uint32_t maxImageDimension3D; //GL_MAX_3D_TEXTURE_SIZE
        uint32_t maxImageDimensionCube; //GL_MAX_CUBE_MAP_TEXTURE_SIZE
        uint32_t maxImageArrayLayers; //GL_MAX_ARRAY_TEXTURE_LAYERS
        uint32_t maxTexelBufferElements; //GL_MAX_TEXTURE_BUFFER_SIZE
        uint32_t maxUniformBufferRange; //GL_MAX_UNIFORM_BLOCK_SIZE
        uint32_t maxStorageBufferRange; //GL_MAX_SHADER_STORAGE_BLOCK_SIZE
        uint32_t
            maxPushConstantsSize; //GL_MAX_UNIFORM_LOCATIONS GL_MAX_COMPUTE_UNIFORM_COMPONENTS, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, GL_MAX_VERTEX_UNIFORM_COMPONENTS * 4
        uint32_t maxMemoryAllocationCount; //OGL -> none
        uint32_t maxSamplerAllocationCount; //OGL -> none
        std::uint64_t bufferImageGranularity; //OGL -> none
        //VkDeviceSize sparseAddressSpaceSize;
        uint32_t maxBoundDescriptorSets; //OGL -> none
        uint32_t
            maxPerStageDescriptorSamplers; //GL_MAX_stage_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_IMAGE_UNITS (fragment), GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
        uint32_t
            maxPerStageDescriptorUniformBuffers; //GL_MAX_COMPUTE_UNIFORM_BLOCKS, GL_MAX_FRAGMENT_UNIFORM_BLOCKS, GL_MAX_GEOMETRY_UNIFORM_BLOCKS, GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, GL_MAX_VERTEX_UNIFORM_BLOCKS
        uint32_t
            maxPerStageDescriptorStorageBuffers; //GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
        uint32_t maxPerStageDescriptorSampledImages; //same as maxPerStageDescriptorSamplers
        uint32_t
            maxPerStageDescriptorStorageImages; //GL_MAX_COMPUTE_IMAGE_UNIFORMS, GL_MAX_FRAGMENT_IMAGE_UNIFORMS, GL_MAX_GEOMETRY_IMAGE_UNIFORMS, GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS, GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS, GL_MAX_VERTEX_IMAGE_UNIFORMS, GL_MAX_IMAGE_UNITS
        //uint32_t maxPerStageDescriptorInputAttachments;
        uint32_t maxPerStageResources; //sum from above
        uint32_t maxDescriptorSetSamplers; //same as maxPerStageDescriptorSamplers
        uint32_t maxDescriptorSetUniformBuffers; //same as maxPerStageDescriptorUniformBuffers
        //uint32_t maxDescriptorSetUniformBuffersDynamic;
        uint32_t maxDescriptorSetStorageBuffers; //same as maxPerStageDescriptorStorageBuffers
        //uint32_t maxDescriptorSetStorageBuffersDynamic;
        uint32_t maxDescriptorSetSampledImages; //same as maxPerStageDescriptorSampledImages
        uint32_t maxDescriptorSetStorageImages; //same as maxPerStageDescriptorStorageImages
        //uint32_t maxDescriptorSetInputAttachments;
        uint32_t maxVertexInputAttributes; //GL_MAX_VERTEX_ATTRIBS
        uint32_t maxVertexInputBindings; //GL_MAX_VERTEX_ATTRIB_BINDINGS
        uint32_t maxVertexInputAttributeOffset; //GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
        uint32_t maxVertexInputBindingStride; //GL_MAX_VERTEX_ATTRIB_STRIDE
        uint32_t maxVertexOutputComponents; //GL_MAX_VERTEX_OUTPUT_COMPONENTS
        uint32_t maxTessellationGenerationLevel; //GL_MAX_TESS_GEN_LEVEL
        uint32_t maxTessellationPatchSize; //GL_MAX_PATCH_VERTICES
        uint32_t
            maxTessellationControlPerVertexInputComponents; //GL_MAX_TESS_CONTROL_INPUT_COMPONENTS
        uint32_t
            maxTessellationControlPerVertexOutputComponents; //GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS
        uint32_t maxTessellationControlPerPatchOutputComponents; //GL_MAX_TESS_PATCH_COMPONENTS
        uint32_t
            maxTessellationControlTotalOutputComponents; //GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS
        uint32_t maxTessellationEvaluationInputComponents; //GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS
        uint32_t
            maxTessellationEvaluationOutputComponents; //GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS
        uint32_t maxGeometryShaderInvocations; //GL_MAX_GEOMETRY_SHADER_INVOCATIONS
        uint32_t maxGeometryInputComponents; //GL_MAX_GEOMETRY_INPUT_COMPONENTS
        uint32_t maxGeometryOutputComponents; //GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
        uint32_t maxGeometryOutputVertices; //GL_MAX_GEOMETRY_OUTPUT_VERTICES
        uint32_t maxGeometryTotalOutputComponents; //GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS
        uint32_t maxFragmentInputComponents; //GL_MAX_FRAGMENT_INPUT_COMPONENTS
        uint32_t maxFragmentOutputAttachments; //GL_MAX_DRAW_BUFFERS
        uint32_t maxFragmentDualSrcAttachments; //GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
        uint32_t maxFragmentCombinedOutputResources; //GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES
        uint32_t maxComputeSharedMemorySize; //GL_MAX_COMPUTE_SHARED_MEMORY_SIZE
        uint32_t maxComputeWorkGroupCount[3]; //GL_MAX_COMPUTE_WORK_GROUP_COUNT
        uint32_t maxComputeWorkGroupInvocations; //GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
        uint32_t maxComputeWorkGroupSize[3]; //GL_MAX_COMPUTE_WORK_GROUP_SIZE
        uint32_t subPixelPrecisionBits; //GL_SUBPIXEL_BITS
        //uint32_t subTexelPrecisionBits;
        //uint32_t mipmapPrecisionBits;
        uint32_t maxDrawIndexedIndexValue; //GL_MAX_ELEMENT_INDEX
        uint32_t maxDrawIndirectCount; //OGL: set max
        float maxSamplerLodBias; //GL_MAX_TEXTURE_LOD_BIAS
        float maxSamplerAnisotropy; //GL_MAX_TEXTURE_MAX_ANISOTROPY
        uint32_t maxViewports; //GL_MAX_VIEWPORTS
        uint32_t maxViewportDimensions[2]; //GL_MAX_VIEWPORT_DIMS
        float viewportBoundsRange[2]; //GL_MAX_VIEWPORT_DIMS
        uint32_t viewportSubPixelBits; //GL_VIEWPORT_SUBPIXEL_BITS
        size_t minMemoryMapAlignment; //GL_MIN_MAP_BUFFER_ALIGNMENT
        std::uint64_t minTexelBufferOffsetAlignment; //GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
        std::uint64_t minUniformBufferOffsetAlignment; //GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
        std::uint64_t minStorageBufferOffsetAlignment; //GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
        int32_t minTexelOffset; //GL_MIN_PROGRAM_TEXEL_OFFSET
        uint32_t maxTexelOffset; //GL_MAX_PROGRAM_TEXEL_OFFSET
        int32_t minTexelGatherOffset; //GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET
        uint32_t maxTexelGatherOffset; //GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET
        float minInterpolationOffset; //GL_MIN_FRAGMENT_INTERPOLATION_OFFSET
        float maxInterpolationOffset; //GL_MAX_FRAGMENT_INTERPOLATION_OFFSET
        uint32_t subPixelInterpolationOffsetBits; //GL_FRAGMENT_INTERPOLATION_OFFSET_BITS
        uint32_t maxFramebufferWidth; //GL_MAX_FRAMEBUFFER_WIDTH
        uint32_t maxFramebufferHeight; //GL_MAX_FRAMEBUFFER_HEIGHT
        uint32_t maxFramebufferLayers; //GL_MAX_FRAMEBUFFER_LAYERS
        SampleCountFlags framebufferColorSampleCounts; //GL_MAX_FRAMEBUFFER_SAMPLES
        SampleCountFlags framebufferDepthSampleCounts; //GL_MAX_FRAMEBUFFER_SAMPLES
        SampleCountFlags framebufferStencilSampleCounts; //GL_MAX_FRAMEBUFFER_SAMPLES
        SampleCountFlags framebufferNoAttachmentsSampleCounts; //GL_MAX_FRAMEBUFFER_SAMPLES
        uint32_t maxColorAttachments; //GL_MAX_COLOR_ATTACHMENTS
        SampleCountFlags sampledImageColorSampleCounts; //GL_MAX_COLOR_TEXTURE_SAMPLES
        SampleCountFlags sampledImageIntegerSampleCounts; //GL_MAX_INTEGER_SAMPLES
        SampleCountFlags sampledImageDepthSampleCounts; //GL_MAX_DEPTH_TEXTURE_SAMPLES
        SampleCountFlags sampledImageStencilSampleCounts; //GL_MAX_DEPTH_TEXTURE_SAMPLES
        SampleCountFlags storageImageSampleCounts; //GL_MAX_IMAGE_SAMPLES
        uint32_t maxSampleMaskWords; //GL_MAX_SAMPLE_MASK_WORDS
        //VkBool32 timestampComputeAndGraphics;
        //float timestampPeriod;
        uint32_t maxClipDistances; //GL_MAX_CLIP_DISTANCES
        uint32_t maxCullDistances; //GL_MAX_CULL_DISTANCES
        uint32_t maxCombinedClipAndCullDistances; //GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES
        uint32_t discreteQueuePriorities; //OGL: max
        float pointSizeRange[2]; //GL_POINT_SIZE_RANGE
        float lineWidthRange[2]; //GL_SMOOTH_LINE_WIDTH_RANGE
        float pointSizeGranularity; //GL_POINT_SIZE_GRANULARITY
        float lineWidthGranularity; //GL_SMOOTH_LINE_WIDTH_GRANULARITY
        //VkBool32 strictLines;
        //VkBool32 standardSampleLocations;
        std::uint64_t optimalBufferCopyOffsetAlignment; //OGL: 1
        std::uint64_t optimalBufferCopyRowPitchAlignment; //OGL: 1
        std::uint64_t nonCoherentAtomSize; //OGL: 1
        std::uint32_t maxCustomBorderColorSamplers; //OGL -> none
    };

    struct VkPhysicalDeviceFeatures
    {
        bool
            robustBufferAccess; //OGL: WGL_ARB_create_context_robustness, GL_ARB_robustness, GL_KHR_robustness
        bool fullDrawIndexUint32; //OGL: true
        bool imageCubeArray; //OGL: true
        bool independentBlend; //OGL: true
        bool geometryShader; //OGL: true
        bool tessellationShader; //OGL: true
        bool sampleRateShading; //OGL: true
        bool dualSrcBlend; //OGL: true
        bool logicOp; //OGL: true
        bool multiDrawIndirect; //GL_ARB_indirect_parameters???
        bool drawIndirectFirstInstance; //OGL: true
        bool depthClamp; //GL_EXT_polygon_offset_clamp, GL_ARB_polygon_offset_clamp
        bool depthBiasClamp; //GL_EXT_polygon_offset_clamp, GL_ARB_polygon_offset_clamp
        bool fillModeNonSolid; //OGL: true
        bool depthBounds; //GL_EXT_depth_bounds_test
        bool wideLines; //GL_SMOOTH_LINE_WIDTH_RANGE check range that is not only 1.0
        bool largePoints; //GL_POINT_SIZE_RANGE check range that is not 1.0 only
        bool alphaToOne; //OGL: true
        bool multiViewport; //GL_MAX_VIEWPORTS check that is not 1
        bool
            samplerAnisotropy; //GL_ARB_texture_filter_anisotropic, GL_EXT_texture_filter_anisotropic
        //bool textureCompressionETC2;
        //bool textureCompressionASTC_LDR;
        bool textureCompressionBC; //GL_EXT_texture_compression_s3tc, GL_EXT_texture_sRGB
        //bool occlusionQueryPrecise;
        //bool pipelineStatisticsQuery;
        bool vertexPipelineStoresAndAtomics; //OGL: true -> 4.3
        bool fragmentStoresAndAtomics; //OGL: true -> 4.3
        bool shaderTessellationAndGeometryPointSize; //OGL: 4.0
        bool shaderImageGatherExtended; //OGL: 4.0
        bool shaderStorageImageExtendedFormats; //OGL: true
        bool shaderStorageImageMultisample; //OGL 4.2
        bool shaderStorageImageReadWithoutFormat; //OGL: true since 4.2???
        bool shaderStorageImageWriteWithoutFormat; //OGL: true since 4.2???
        bool shaderUniformBufferArrayDynamicIndexing; //OGL: 4.0
        bool shaderSampledImageArrayDynamicIndexing; //OGL: 4.0
        bool shaderStorageBufferArrayDynamicIndexing; //OGL: 4.3
        bool shaderStorageImageArrayDynamicIndexing; //OGL: 4.3
        bool shaderClipDistance; //OGL: 4.0 but not all stages supported in 3.3
        bool shaderCullDistance; //OGL: 4.5 or GL_ARB_cull_distance(do not check this)
        bool shaderFloat64; //OGL: 4.1
        bool shaderInt64; //GL_ARB_gpu_shader_int64
        bool shaderInt16; //GL_EXT_shader_explicit_arithmetic_types_int16 -> GL_AMD_gpu_shader_int16
        //bool shaderResourceResidency;
        bool shaderResourceMinLod; //OGL: true
        //bool sparseBinding;
        //bool sparseResidencyBuffer;
        //bool sparseResidencyImage2D;
        //bool sparseResidencyImage3D;
        //bool sparseResidency2Samples;
        //bool sparseResidency4Samples;
        //bool sparseResidency8Samples;
        //bool sparseResidency16Samples;
        //bool sparseResidencyAliased;
        bool variableMultisampleRate; //OGL: true
        //bool inheritedQueries;
        bool samplerMirrorClampToEdge; //OGL: true; VK_KHR_sampler_mirror_clamp_to_edge
        bool customBorderColors; //OGL: true; VK_EXT_custom_border_color
        bool customBorderColorWithoutFormat; //OGL: true; VK_EXT_custom_border_color
        bool validation_layer; //OGL: debug context; VK: validation alyer
        bool debug_messenger; //OGL: GL_DEBUG_OUTPUT(SYNC or not); VK: debug_utils + debug messenger
        // we must set default logger for OGL that will send all messages into stdout like in VK
    };

    enum class ContextDeviceType
    {
        Other,
        IntegratedGPU,
        DiscreteGPU,
        VirtualGPU,
        CPU
    };

    enum class ViewOrigin //X - right, Z - to screen
    {
        TopLeft, //VK
        BottomLeft //OGL
    };

    struct ClipSpaceDepthBounds
    {
        float min;
        float max;
    };

    struct ContextProperties
    {
        std::string context_name;
        Core::RenderBackendType supported_backend_type;
        std::uint32_t version; //major << 16 | minor
        std::string_view vendor_name;
        std::string_view device_name;
        std::vector<std::string> extensions;
        ShaderSyntaxFlags supported_syntax;
        std::vector<QueueFamilyProperties> queue_family_properties;
        std::vector<MemoryType> memory_types;
        CommandBufferStrategy command_buffer_strategy;
        ContextDeviceType device_type;
        ViewOrigin view_origin;
        ClipSpaceDepthBounds clip_space_depth_bounds;
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

    class Resolve;

    struct SelectedContextDesc
    {
        std::uint32_t index;
        std::vector<Render::QueueFamilyInfo> queue_family_infos;
    };

    constexpr inline auto RENDER_RESOLVE_FUNCTION_NAME = "RenderResolve";
    using PFN_RenderResolve = Resolve* (*)();
};