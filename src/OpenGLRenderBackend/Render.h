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

    using GLHandle = GLuint;

    constexpr inline GLuint OGL_NULL_HANDLE = 0;

    GLenum ComapreOpToNative(Render::CompareOp op);
    GLenum SampleCountToNative(Render::SampleCount samples);
    GLbitfield BufferFlagsToNative(Render::BufferFlags flags);
    GLenum FenceStatusToNative(Render::FenceStatus status);
    GLenum FormatToNative(Render::Format format);
    GLenum ImageViewTypeToNative(Render::ImageViewType type);

    constexpr inline GLenum OGL_IDENTITY_SWIZZLE = std::numeric_limits<GLenum>::max();
    GLenum ComponentSwizzleToNative(Render::ComponentSwizzle swizzle);
    GLenum FilterToNative(Render::Filter filter);
    GLenum AddressModeToNative(Render::AddressMode mode);
    GLenum ShaderStageToNative(Render::ShaderStage stage);
    GLenum InputRateToNative(Render::InputRate rate);
    GLenum PrimitiveTopologyToNative(Render::PrimitiveTopology topology);
    GLenum BlendFactorToNative(Render::BlendFactor factor);
    GLenum BlendLogicOpToNative(Render::BlendLogicOp op);
    GLenum BlendEquationToNative(Render::BlendEquation eq);
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
};

/*enum MemoryBarrierFlagBits
    {
        VertexAttribArrayBarrier = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
        ElementArrayBarrier = GL_ELEMENT_ARRAY_BARRIER_BIT,
        UniformBarrier = GL_UNIFORM_BARRIER_BIT,
        TextureFetchBarrier = GL_TEXTURE_FETCH_BARRIER_BIT,
        ShaderImageAccessBarrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
        CommandBarrier = GL_COMMAND_BARRIER_BIT,
        PixelBufferBarrier = GL_PIXEL_BUFFER_BARRIER_BIT,
        TextureUpdateBarrier = GL_TEXTURE_UPDATE_BARRIER_BIT,
        QueyBufferbarrier = GL_QUERY_BUFFER_BARRIER_BIT,
        BufferUpdateBarrier = GL_BUFFER_UPDATE_BARRIER_BIT,
        FramebufferBarrier = GL_FRAMEBUFFER_BARRIER_BIT,
        TransformFeedbackBarrier = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
        AtomicCounterBarrier = GL_ATOMIC_COUNTER_BARRIER_BIT,
        ShaderStorageBarrier = GL_SHADER_STORAGE_BARRIER_BIT,
        ClientMappedBufferBarrier = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
        AllBarrier = GL_ALL_BARRIER_BITS
    };

    using MemoryBarrierFlags = std::underlying_type_t<MemoryBarrierFlagBits>;*/