#pragma once

#include <limits>
#include "glad/gl.h"
#include "Core/Render/Render.h"

namespace OpenGL
{
    class Context;
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
    class Pipeline;
    class Queue;
    class RenderPass;
    class Sampler;
    class Semaphore;
    class Shader;

    using GLHandle = GLuint;

    constexpr inline GLuint OGL_NULL_HANDLE = 0;

    GLenum ComapreOpToNative(Render::CompareOp op);
    GLenum SampleCountToNative(Render::SampleCount samples);
    GLbitfield DecodeMemoryTypePropertyFlagsToNative(Render::MemoryTypePropertyFlags flags);
    GLenum FenceStatusToNative(Render::FenceStatus status);
    GLenum FormatToNative(Render::Format format);
    GLenum ImageViewTypeToNative(Render::ImageViewType type);

    constexpr inline GLenum OGL_IDENTITY_SWIZZLE = std::numeric_limits<GLenum>::max();
    GLenum ComponentSwizzleToNative(Render::ComponentSwizzle swizzle);
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

    constexpr inline GLenum OGL_CULL_MODE_NONE = 0;
    GLenum CullModeToNative(Render::CullMode mode);
    GLenum FrontFaceToNative(Render::FrontFace face);
    GLenum IndexTypeToNative(Render::IndexType type);

    struct TransferImageTypeFormat
    {
        GLenum type;
        GLenum format;
    };

    TransferImageTypeFormat DecodeTransferTypeFormatPair(Render::Format format);

    struct VertexInputTypeSize
    {
        GLenum type;
        GLenum size;
        bool normalized;
    };

    VertexInputTypeSize DecodeVertexInputTypeSizePair(Render::Format format);

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

    struct DescriptorCombinedImageSamplerDesc
    {
        GLHandle sampler;
        GLHandle image_view;
    };

    struct DescriptorBufferDesc
    {
        GLHandle buffer;
        GLintptr offset;
        GLsizeiptr size;
    };

    struct DescriptorStorageImageDesc
    {
        GLHandle image_view;
    };

    constexpr inline std::size_t COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE =
        sizeof(DescriptorCombinedImageSamplerDesc);
    constexpr inline std::size_t STORAGE_IMAGE_DESCRIPTOR_SIZE = sizeof(DescriptorStorageImageDesc);
    constexpr inline std::size_t UNIFORM_BUFFER_DESCRIPTOR_SIZE = sizeof(DescriptorBufferDesc);
    constexpr inline std::size_t STOARGE_BUFFER_DESCRIPTOR_SIZE = sizeof(DescriptorBufferDesc);

    constexpr inline std::size_t DESCRIPTOR_ALIGNMENT =
        std::max({alignof(DescriptorCombinedImageSamplerDesc),
                  alignof(DescriptorStorageImageDesc),
                  alignof(DescriptorBufferDesc),
                  alignof(DescriptorBufferDesc)});
};