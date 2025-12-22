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

    constexpr inline std::uint32_t SWAPCHAIN_IMAGE_COUNT = 1;

    GLenum ComapreOpToNative(Render::CompareOp op);
    GLenum SampleCountToNative(Render::SampleCount samples);
    GLbitfield DecodeBufferStorageFlags(Render::MemoryTypePropertyFlags memory_flags,
                                        Render::BufferMapUsageFlags map_usage);
    GLenum FenceStatusToNative(Render::FenceStatus status);
    std::optional<GLenum> FormatToNative(Render::Format format) noexcept;
    GLenum ImageViewTypeToNative(Render::ImageViewType type);
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

    GLenum DecodeImageType(Render::ImageType type, bool layered, bool sampled);

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

    void GLAPIENTRY debug_messenger_callback(GLenum source,
                                             GLenum type,
                                             GLuint id,
                                             GLenum severity,
                                             GLsizei length,
                                             const GLchar* message,
                                             const void* user_param);
};