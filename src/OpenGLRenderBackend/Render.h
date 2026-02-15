#pragma once

#include <limits>
#include <optional>
#include "glad/gl.h"
#include "Core/Render/Render.h"

namespace OpenGL
{
    class Buffer;
    class BufferView;
    class CommandBuffer;
    class CommandPool;
    class DescriptorPool;
    class DescriptorSet;
    class DescriptorSetLayout;
    class Device;
    class Fence;
    class Framebuffer;
    class Image;
    class ImageView;
    class Instance;
    class PhysicalDevice;
    class Pipeline;
    class Queue;
    class RenderPass;
    class Sampler;
    class Semaphore;
    class Shader;
    class Surface;
    class Swapchain;

    using GLHandle = GLuint;

    constexpr inline GLuint OGL_NULL_HANDLE = 0;

    /*
    1. DeviceLocal
    2. DeviceLocal | HostVisible
    3. DeviceLocal | HostVisible | HostCoherent
    4. DeviceLocal | HostVisible | HostCached
    5. DeviceLocal | HostVisible | HostCoherent | HostCached

    6. HostVisible
    7. HostVisible | HostCoherent
    8. HostVisible | HostCached
    9. HostVisible | HostCoherent | HostCached
    */

    constexpr static Render::MemoryType AVAILABLE_MEMORY_TYPES[] = {
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::DeviceLocal},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::DeviceLocal |
             Render::MemoryTypePropertyFlagBits::HostVisible},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::DeviceLocal |
             Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCoherent},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::DeviceLocal |
             Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCached},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::DeviceLocal |
             Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCoherent |
             Render::MemoryTypePropertyFlagBits::HostCached},

        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::HostVisible},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCoherent},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCached},
        {Render::MemoryHeapFlagBits::DeviceLocalHeap,
         Render::MemoryTypePropertyFlagBits::HostVisible |
             Render::MemoryTypePropertyFlagBits::HostCoherent |
             Render::MemoryTypePropertyFlagBits::HostCached}};

    struct SurfaceConnectInfo
    {
        PhysicalDevice* physical_device;
        std::uint32_t format_index;
        bool robust_buffer_access_enabled;
    };

    //shared across all OpenGL implementations
    constexpr inline std::uint32_t SWAPCHAIN_IMAGE_COUNT = 1;
    constexpr inline std::uint32_t SURFACE_MIN_IMAGE_COUNT = 1;
    constexpr inline std::uint32_t SURFACE_MAX_IMAGE_COUNT = 1;
    constexpr inline Render::Extent2D SURFACE_MIN_EXTENT = {.width = 0, .height = 0};
    constexpr inline Render::Extent2D SURFACE_CURRENT_EXTENT = {.width = 0, .height = 0};
    constexpr inline Render::Extent2D SURFACE_MAX_EXTENT = {.width = 0, .height = 0};
    constexpr inline Render::SurfaceExtentMode SURFACE_EXTENT_MODE =
        Render::SurfaceExtentMode::DoNotCare;

    struct PhysicalDeviceSurfaceDesc
    {
        Render::PresentModeFlags supported_present_modes;
        std::vector<Render::Format> supported_formats;
    };

    GLenum ComapreOpToNative(Render::CompareOp op);
    GLenum SampleCountToNative(Render::SampleCount samples);
    GLbitfield DecodeBufferStorageFlags(Render::MemoryTypePropertyFlags memory_flags,
                                        Render::BufferMapUsageFlags map_usage);
    GLenum FenceStatusToNative(Render::FenceStatus status);
    std::optional<GLenum> FormatToNative(Render::Format format) noexcept;
    GLenum ImageViewTypeToNative(Render::ImageViewType type, bool image_multisampled);
    GLint ComponentSwizzleToNative(Render::ComponentSwizzle swizzle, GLenum base);
    GLenum FilterToNative(Render::Filter filter);
    GLenum AddressModeToNative(Render::AddressMode mode);
    GLenum ShaderStageToNative(Render::ShaderStageFlagBits stage);
    GLenum InputRateToNative(Render::InputRate rate);
    GLenum PrimitiveTopologyToNative(Render::PrimitiveTopology topology);
    GLenum BlendFactorToNative(Render::BlendFactor factor);
    GLenum BlendLogicOpToNative(Render::BlendLogicOp op);
    GLenum BlendOpToNative(Render::BlendOp op);
    GLenum StencilOpToNative(Render::StencilOp op);
    GLenum PolygonModeToNative(Render::PolygonMode mode);
    GLenum DecodePolygonOffsetMode(Render::PolygonMode mode);

    constexpr inline GLenum OGL_CULL_MODE_NONE = 0;
    GLenum CullModeToNative(Render::CullMode mode);
    GLenum FrontFaceToNative(Render::FrontFace face);
    GLenum IndexTypeToNative(Render::IndexType type);

    GLenum
    DecodeImageType(Render::ImageType type, bool layered, bool sampled, bool cubemap_compatible);

    struct TransferImageTypeFormat
    {
        GLenum type;
        GLenum format;
    };

    std::optional<TransferImageTypeFormat>
    DecodeTransferTypeFormatPair(Render::Format format) noexcept;

    enum class VertexInputFunctionType
    {
        Normalized,
        Unnormalized,
        Integer,
        Double
    };

    struct VertexInputTypeSize
    {
        GLenum type;
        GLenum size;
        VertexInputFunctionType function_type;
    };

    std::optional<VertexInputTypeSize>
    DecodeVertexInputTypeSizePair(Render::Format format) noexcept;

    template<std::size_t N>
    struct ArrayDecodeResult
    {
        std::array<GLenum, N> data;
        std::size_t size;
    };

    ArrayDecodeResult<9>
    DebugMessengerTypeFlagsToNativeInverted(Render::DebugMessengerTypeFlags types);
    ArrayDecodeResult<4>
    DebugMessengerSeverityFlagsToNativeInverted(Render::DebugMessengerSeverityFlags severities);

    Render::DebugMessengerTypeFlagBits NativeDebugMessengerTypeFlagBitToSpec(GLenum type);
    Render::DebugMessengerSeverityFlagBits
    NativeDebugMessengerSeverityFlagBitToSpec(GLenum severity);

    GLbitfield PipelineBarrierToNative(const Render::PipelineBarrier& barrier);

    /*
    registers:
        textures: CombinedImageSampler + SampledImage + UniformTexelBuffer + StorageTexelBuffer + InputAttachment
        uniform buffers: UnifromBuffer
        storage buffer: StorageBuffer
        storage image: StorageImage
    */

    struct alignas(8) DescriptorTextureDesc
    {
        GLHandle sampler; //CombinedImageSampler only
        GLHandle image_view;
    };

    struct alignas(8) DescriptorUniformBufferDesc
    {
        GLHandle buffer;
        GLintptr offset;
        GLsizeiptr size;
    };

    using DescriptorStorageBufferDesc = DescriptorUniformBufferDesc;

    struct alignas(8) DescriptorStorageImageDesc
    {
        GLHandle image_view;
    };

    constexpr inline std::size_t TEXTURE_DESCRIPTOR_SIZE = sizeof(DescriptorTextureDesc);
    constexpr inline std::size_t UNIFORM_BUFFER_DESCRIPTOR_SIZE =
        sizeof(DescriptorUniformBufferDesc);
    constexpr inline std::size_t STORAGE_BUFFER_DESCRIPTOR_SIZE =
        sizeof(DescriptorStorageBufferDesc);
    constexpr inline std::size_t STORAGE_IMAGE_DESCRIPTOR_SIZE = sizeof(DescriptorStorageImageDesc);

    constexpr inline std::size_t DESCRIPTOR_ALIGNMENT =
        std::max({alignof(DescriptorTextureDesc),
                  alignof(DescriptorUniformBufferDesc),
                  alignof(DescriptorStorageBufferDesc),
                  alignof(DescriptorStorageImageDesc)});

    void EnableDebugMessenger(const GladGLContext& loader);
    void SetDebugMessenger(const GladGLContext& loader, const Render::DebugMessengerInfo& info);

    std::optional<Render::Format> DecodePixelFormat(const GladGLContext& loader,
                                                    std::uint8_t red_bits,
                                                    std::uint8_t red_shift,
                                                    std::uint8_t green_bits,
                                                    std::uint8_t green_shift,
                                                    std::uint8_t blue_bits,
                                                    std::uint8_t blue_shift,
                                                    std::uint8_t alpha_bits,
                                                    std::uint8_t alpha_shift,
                                                    std::uint8_t color_bits,
                                                    bool is_srgb,
                                                    bool is_float);
};