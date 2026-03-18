#include "Render.h"
#include <stdexcept>
#include <bit>
#include <cassert>
#include "Core/Utils/Binary.hpp"

namespace OpenGL
{
    GLenum ComapreOpToNative(Render::CompareOp op)
    {
        GLenum value;
        switch(op)
        {
            case Render::CompareOp::Never:
                value = GL_NEVER;
                break;
            case Render::CompareOp::Less:
                value = GL_LESS;
                break;
            case Render::CompareOp::Equal:
                value = GL_EQUAL;
                break;
            case Render::CompareOp::LessOrEqual:
                value = GL_LEQUAL;
                break;
            case Render::CompareOp::Greater:
                value = GL_GREATER;
                break;
            case Render::CompareOp::NotEqual:
                value = GL_NOTEQUAL;
                break;
            case Render::CompareOp::GreaterOrEqual:
                value = GL_GEQUAL;
                break;
            case Render::CompareOp::Always:
                value = GL_ALWAYS;
                break;
            default:
                throw std::runtime_error("No native CompareOp found");
                break;
        }

        return value;
    }

    GLenum SampleCountToNative(Render::SampleCount samples)
    {
        GLenum value;
        switch(samples)
        {
            case Render::SampleCount::SampleCount_1:
                value = 1;
                break;
            case Render::SampleCount::SampleCount_2:
                value = 2;
                break;
            case Render::SampleCount::SampleCount_4:
                value = 4;
                break;
            case Render::SampleCount::SampleCount_8:
                value = 8;
                break;
            case Render::SampleCount::SampleCount_16:
                value = 16;
                break;
            case Render::SampleCount::SampleCount_32:
                value = 32;
                break;
            case Render::SampleCount::SampleCount_64:
                value = 64;
                break;
            default:
                throw std::runtime_error("No native SampleCount found");
                break;
        }

        return value;
    }

    GLbitfield DecodeBufferStorageFlags(Render::MemoryTypePropertyFlags memory_flags,
                                        Render::BufferMapUsageFlags map_usage)
    {
        constexpr static std::pair<Render::MemoryTypePropertyFlagBits, GLbitfield>
            memory_mapping[] = {
                {Render::MemoryTypePropertyFlagBits::DeviceLocal, 0},
                {Render::MemoryTypePropertyFlagBits::HostVisible, GL_MAP_PERSISTENT_BIT},
                {Render::MemoryTypePropertyFlagBits::HostCoherent, GL_MAP_COHERENT_BIT},
                {Render::MemoryTypePropertyFlagBits::HostCached, GL_CLIENT_STORAGE_BIT}};

        GLbitfield mask = 0;
        for(const auto& pr: memory_mapping)
        {
            if(memory_flags & pr.first)
                mask |= pr.second;
        }

        GLbitfield map_mask = 0;
        if(map_usage & Render::BufferMapUsageFlagBits::BufferMapUsageRead)
            map_mask |= GL_MAP_READ_BIT;

        if(map_usage & Render::BufferMapUsageFlagBits::BufferMapUsageWrite)
            map_mask |= GL_MAP_WRITE_BIT;

        if(map_mask == 0)
            map_mask = GL_MAP_WRITE_BIT;

        return mask | map_mask;
    }

    GLenum FenceStatusToNative(Render::FenceStatus status)
    {
        GLenum value;
        switch(status)
        {
            case Render::FenceStatus::Signaled:
                value = GL_SIGNALED;
                break;
            case Render::FenceStatus::Unsignaled:
                value = GL_UNSIGNALED;
                break;
            default:
                throw std::runtime_error("No native FenceStatus found");
                break;
        }

        return value;
    }

    std::optional<GLenum> FormatToNative(Render::Format format) noexcept
    {
        std::optional<GLenum> value;
        switch(format)
        {
            case Render::Format::R4G4B4A4_UNORM_PACK16:
                value = GL_RGBA4;
                break;
            case Render::Format::B4G4R4A4_UNORM_PACK16:
                value = GL_RGBA4;
                break;
            case Render::Format::R5G6B5_UNORM_PACK16:
                value = GL_RGB565;
                break;
            case Render::Format::B5G6R5_UNORM_PACK16:
                value = GL_RGB565;
                break;
            case Render::Format::R5G5B5A1_UNORM_PACK16:
                value = GL_RGB5_A1;
                break;
            case Render::Format::B5G5R5A1_UNORM_PACK16:
                value = GL_RGB5_A1;
                break;
            case Render::Format::A1R5G5B5_UNORM_PACK16:
                value = GL_RGB5_A1;
                break;
            case Render::Format::R8_UNORM:
                value = GL_R8;
                break;
            case Render::Format::R8_SNORM:
                value = GL_R8_SNORM;
                break;
            case Render::Format::R8_UINT:
                value = GL_R8UI;
                break;
            case Render::Format::R8_SINT:
                value = GL_R8I;
                break;
            case Render::Format::R8_UNORM_SRGB:
                value = GL_SR8_EXT;
                break;
            case Render::Format::R8G8_UNORM:
                value = GL_RG8;
                break;
            case Render::Format::R8G8_SNORM:
                value = GL_RG8_SNORM;
                break;
            case Render::Format::R8G8_UINT:
                value = GL_RG8UI;
                break;
            case Render::Format::R8G8_SINT:
                value = GL_RG8I;
                break;
            case Render::Format::R8G8_UNORM_SRGB:
                value = GL_SRG8_EXT;
                break;
            case Render::Format::R8G8B8_UNORM:
                value = GL_RGB8;
                break;
            case Render::Format::R8G8B8_SNORM:
                value = GL_RGB8_SNORM;
                break;
            case Render::Format::R8G8B8_UINT:
                value = GL_RGB8UI;
                break;
            case Render::Format::R8G8B8_SINT:
                value = GL_RGB8I;
                break;
            case Render::Format::R8G8B8_UNORM_SRGB:
                value = GL_SRGB8;
                break;
            case Render::Format::B8G8R8_UNORM:
                value = GL_RGB8;
                break;
            case Render::Format::B8G8R8_SNORM:
                value = GL_RGB8_SNORM;
                break;
            case Render::Format::B8G8R8_UINT:
                value = GL_RGB8UI;
                break;
            case Render::Format::B8G8R8_SINT:
                value = GL_RGB8I;
                break;
            case Render::Format::B8G8R8_UNORM_SRGB:
                value = GL_SRGB8;
                break;
            case Render::Format::R8G8B8A8_UNORM:
                value = GL_RGBA8;
                break;
            case Render::Format::R8G8B8A8_SNORM:
                value = GL_RGBA8_SNORM;
                break;
            case Render::Format::R8G8B8A8_UINT:
                value = GL_RGBA8UI;
                break;
            case Render::Format::R8G8B8A8_SINT:
                value = GL_RGBA8I;
                break;
            case Render::Format::R8G8B8A8_UNORM_SRGB:
                value = GL_SRGB8_ALPHA8;
                break;
            case Render::Format::B8G8R8A8_UNORM:
                value = GL_RGBA8;
                break;
            case Render::Format::B8G8R8A8_SNORM:
                value = GL_RGBA8_SNORM;
                break;
            case Render::Format::B8G8R8A8_UINT:
                value = GL_RGBA8UI;
                break;
            case Render::Format::B8G8R8A8_SINT:
                value = GL_RGBA8I;
                break;
            case Render::Format::B8G8R8A8_UNORM_SRGB:
                value = GL_SRGB8_ALPHA8;
                break;
            case Render::Format::A8B8G8R8_UNORM_PACK32:
                value = GL_RGBA8;
                break;
            case Render::Format::A8B8G8R8_SNORM_PACK32:
                value = GL_RGBA8_SNORM;
                break;
            case Render::Format::A8B8G8R8_UINT_PACK32:
                value = GL_RGBA8UI;
                break;
            case Render::Format::A8B8G8R8_SINT_PACK32:
                value = GL_RGBA8I;
                break;
            case Render::Format::A8B8G8R8_UNORM_SRGB_PACK32:
                value = GL_SRGB8_ALPHA8;
                break;
            case Render::Format::A2R10G10B10_UNORM_PACK32:
                value = GL_RGB10_A2;
                break;
            case Render::Format::A2R10G10B10_UINT_PACK32:
                value = GL_RGB10_A2UI;
                break;
            case Render::Format::A2B10G10R10_UNORM_PACK32:
                value = GL_RGB10_A2;
                break;
            case Render::Format::A2B10G10R10_UINT_PACK32:
                value = GL_RGB10_A2UI;
                break;
            case Render::Format::R16_UNORM:
                value = GL_R16;
                break;
            case Render::Format::R16_SNORM:
                value = GL_R16_SNORM;
                break;
            case Render::Format::R16_UINT:
                value = GL_R16UI;
                break;
            case Render::Format::R16_SINT:
                value = GL_R16I;
                break;
            case Render::Format::R16_SFLOAT:
                value = GL_R16F;
                break;
            case Render::Format::R16G16_UNORM:
                value = GL_RG16;
                break;
            case Render::Format::R16G16_SNORM:
                value = GL_RG16_SNORM;
                break;
            case Render::Format::R16G16_UINT:
                value = GL_RG16UI;
                break;
            case Render::Format::R16G16_SINT:
                value = GL_RG16I;
                break;
            case Render::Format::R16G16_SFLOAT:
                value = GL_RG16F;
                break;
            case Render::Format::R16G16B16_UNORM:
                value = GL_RGB16;
                break;
            case Render::Format::R16G16B16_SNORM:
                value = GL_RGB16_SNORM;
                break;
            case Render::Format::R16G16B16_UINT:
                value = GL_RGB16UI;
                break;
            case Render::Format::R16G16B16_SINT:
                value = GL_RGB16I;
                break;
            case Render::Format::R16G16B16_SFLOAT:
                value = GL_RGB16F;
                break;
            case Render::Format::R16G16B16A16_UNORM:
                value = GL_RGBA16;
                break;
            case Render::Format::R16G16B16A16_SNORM:
                value = GL_RGBA16_SNORM;
                break;
            case Render::Format::R16G16B16A16_UINT:
                value = GL_RGBA16UI;
                break;
            case Render::Format::R16G16B16A16_SINT:
                value = GL_RGBA16I;
                break;
            case Render::Format::R16G16B16A16_SFLOAT:
                value = GL_RGBA16F;
                break;
            case Render::Format::R32_UINT:
                value = GL_R32UI;
                break;
            case Render::Format::R32_SINT:
                value = GL_R32I;
                break;
            case Render::Format::R32_SFLOAT:
                value = GL_R32F;
                break;
            case Render::Format::R32G32_UINT:
                value = GL_RG32UI;
                break;
            case Render::Format::R32G32_SINT:
                value = GL_RG32I;
                break;
            case Render::Format::R32G32_SFLOAT:
                value = GL_RG32F;
                break;
            case Render::Format::R32G32B32_UINT:
                value = GL_RGB32UI;
                break;
            case Render::Format::R32G32B32_SINT:
                value = GL_RGB32I;
                break;
            case Render::Format::R32G32B32_SFLOAT:
                value = GL_RGB32F;
                break;
            case Render::Format::R32G32B32A32_UINT:
                value = GL_RGBA32UI;
                break;
            case Render::Format::R32G32B32A32_SINT:
                value = GL_RGBA32I;
                break;
            case Render::Format::R32G32B32A32_SFLOAT:
                value = GL_RGBA32F;
                break;
            case Render::Format::B10G11R11_UFLOAT_PACK32:
                value = GL_R11F_G11F_B10F;
                break;
            case Render::Format::E5B9G9R9_UFLOAT_PACK32:
                value = GL_RGB9_E5;
                break;
            case Render::Format::D16_UNORM:
                value = GL_DEPTH_COMPONENT16;
                break;
            case Render::Format::X8_D24_UNORM_PACK32:
                value = GL_DEPTH24_STENCIL8;
                break;
            case Render::Format::D32_SFLOAT:
                value = GL_DEPTH_COMPONENT32F;
                break;
            case Render::Format::S8_UINT:
                value = GL_STENCIL_INDEX8;
                break;
            case Render::Format::D24_UNORM_S8_UINT:
                value = GL_DEPTH24_STENCIL8;
                break;
            case Render::Format::D32_SFLOAT_S8_UINT:
                value = GL_DEPTH32F_STENCIL8;
                break;
            case Render::Format::BC1_RGB_UNORM_BLOCK:
                value = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                break;
            case Render::Format::BC1_RGB_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
                break;
            case Render::Format::BC1_RGBA_UNORM_BLOCK:
                value = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case Render::Format::BC1_RGBA_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
                break;
            case Render::Format::BC2_UNORM_BLOCK:
                value = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case Render::Format::BC2_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
                break;
            case Render::Format::BC3_UNORM_BLOCK:
                value = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            case Render::Format::BC3_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
                break;
            case Render::Format::BC4_UNORM_BLOCK:
                value = GL_COMPRESSED_RED_RGTC1;
                break;
            case Render::Format::BC4_SNORM_BLOCK:
                value = GL_COMPRESSED_SIGNED_RED_RGTC1;
                break;
            case Render::Format::BC5_UNORM_BLOCK:
                value = GL_COMPRESSED_RG_RGTC2;
                break;
            case Render::Format::BC5_SNORM_BLOCK:
                value = GL_COMPRESSED_SIGNED_RG_RGTC2;
                break;
            case Render::Format::BC6H_UFLOAT_BLOCK:
                value = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
                break;
            case Render::Format::BC6H_SFLOAT_BLOCK:
                value = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
                break;
            case Render::Format::BC7_UNORM_BLOCK:
                value = GL_COMPRESSED_RGBA_BPTC_UNORM;
                break;
            case Render::Format::BC7_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
                break;
            case Render::Format::ETC2_R8G8B8_UNORM_BLOCK:
                value = GL_COMPRESSED_RGB8_ETC2;
                break;
            case Render::Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB8_ETC2;
                break;
            case Render::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
                value = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                break;
            case Render::Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                break;
            case Render::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
                value = GL_COMPRESSED_RGBA8_ETC2_EAC;
                break;
            case Render::Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
                value = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
                break;
            case Render::Format::EAC_R11_UNORM_BLOCK:
                value = GL_COMPRESSED_R11_EAC;
                break;
            case Render::Format::EAC_R11_SNORM_BLOCK:
                value = GL_COMPRESSED_SIGNED_R11_EAC;
                break;
            case Render::Format::EAC_R11G11_UNORM_BLOCK:
                value = GL_COMPRESSED_RG11_EAC;
                break;
            case Render::Format::EAC_R11G11_SNORM_BLOCK:
                value = GL_COMPRESSED_SIGNED_RG11_EAC;
                break;
            case Render::Format::A4R4G4B4_UNORM_PACK16:
                value = GL_RGBA4;
                break;
            case Render::Format::A4B4G4R4_UNORM_PACK16:
                value = GL_RGBA4;
                break;
            case Render::Format::A1B5G5R5_UNORM_PACK16:
                value = GL_RGB5_A1;
                break;
            case Render::Format::UNDEFINED:
            case Render::Format::R4G4_UNORM_PACK8:
            //scaled are not image formats
            case Render::Format::R8_USCALED:
            case Render::Format::R8_SSCALED:
            case Render::Format::R8G8_USCALED:
            case Render::Format::R8G8_SSCALED:
            case Render::Format::R8G8B8_USCALED:
            case Render::Format::R8G8B8_SSCALED:
            case Render::Format::B8G8R8_USCALED:
            case Render::Format::B8G8R8_SSCALED:
            case Render::Format::R8G8B8A8_USCALED:
            case Render::Format::R8G8B8A8_SSCALED:
            case Render::Format::B8G8R8A8_USCALED:
            case Render::Format::B8G8R8A8_SSCALED:
            case Render::Format::A8B8G8R8_USCALED_PACK32:
            case Render::Format::A8B8G8R8_SSCALED_PACK32:
            //signed A2_RGB10 not supported
            case Render::Format::A2R10G10B10_SNORM_PACK32:
            case Render::Format::A2R10G10B10_USCALED_PACK32:
            case Render::Format::A2R10G10B10_SSCALED_PACK32:
            case Render::Format::A2R10G10B10_SINT_PACK32:
            case Render::Format::A2B10G10R10_SNORM_PACK32:
            case Render::Format::A2B10G10R10_USCALED_PACK32:
            case Render::Format::A2B10G10R10_SSCALED_PACK32:
            case Render::Format::A2B10G10R10_SINT_PACK32:
            case Render::Format::R16_USCALED:
            case Render::Format::R16_SSCALED:
            case Render::Format::R16G16_USCALED:
            case Render::Format::R16G16_SSCALED:
            case Render::Format::R16G16B16_USCALED:
            case Render::Format::R16G16B16_SSCALED:
            case Render::Format::R16G16B16A16_USCALED:
            case Render::Format::R16G16B16A16_SSCALED:
            //64-per channel formats are only for vertex input
            case Render::Format::R64_UINT:
            case Render::Format::R64_SINT:
            case Render::Format::R64_SFLOAT:
            case Render::Format::R64G64_UINT:
            case Render::Format::R64G64_SINT:
            case Render::Format::R64G64_SFLOAT:
            case Render::Format::R64G64B64_UINT:
            case Render::Format::R64G64B64_SINT:
            case Render::Format::R64G64B64_SFLOAT:
            case Render::Format::R64G64B64A64_UINT:
            case Render::Format::R64G64B64A64_SINT:
            case Render::Format::R64G64B64A64_SFLOAT:
            case Render::Format::D16_UNORM_S8_UINT: //no in OGL
            case Render::Format::A8_UNORM: //lagacy
                break;
        }

        return value;
    }

    GLenum ImageViewTypeToNative(Render::ImageViewType type, bool is_image_multisampled)
    {
        GLenum value;
        switch(type)
        {
            case Render::ImageViewType::ImageView1D:
                value = GL_TEXTURE_1D;
                break;
            case Render::ImageViewType::ImageView2D:
                if(!is_image_multisampled)
                    value = GL_TEXTURE_2D;
                else
                    value = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case Render::ImageViewType::ImageView3D:
                value = GL_TEXTURE_3D;
                break;
            case Render::ImageViewType::ImageViewCubeMap:
                value = GL_TEXTURE_CUBE_MAP;
                break;
            case Render::ImageViewType::ImageView1DArray:
                value = GL_TEXTURE_1D_ARRAY;
                break;
            case Render::ImageViewType::ImageView2DArray:
                if(!is_image_multisampled)
                    value = GL_TEXTURE_2D_ARRAY;
                else
                    value = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                break;
            case Render::ImageViewType::ImageViewCubeMapArray:
                value = GL_TEXTURE_CUBE_MAP_ARRAY;
                break;
            default:
                throw std::runtime_error("No native ImageViewType found");
                break;
        }

        return value;
    }

    GLint ComponentSwizzleToNative(Render::ComponentSwizzle swizzle, GLenum base)
    {
        GLint value;
        switch(swizzle)
        {
            case Render::ComponentSwizzle::SwizzleRed:
                value = GL_RED;
                break;
            case Render::ComponentSwizzle::SwizzleGreen:
                value = GL_GREEN;
                break;
            case Render::ComponentSwizzle::SwizzleBlue:
                value = GL_BLUE;
                break;
            case Render::ComponentSwizzle::SwizzleAlpha:
                value = GL_ALPHA;
                break;
            case Render::ComponentSwizzle::SwizzleZero:
                value = GL_ZERO;
                break;
            case Render::ComponentSwizzle::SwizzleOne:
                value = GL_ONE;
                break;
            case Render::ComponentSwizzle::SwizzleIdentity:
                value = base;
                break;
            default:
                throw std::runtime_error("No native ComponentSwizzle found");
                break;
        }

        return value;
    }

    GLenum FilterToNative(Render::Filter filter)
    {
        GLenum value;
        switch(filter)
        {
            case Render::Filter::Nearest:
                value = GL_NEAREST;
                break;
            case Render::Filter::Linear:
                value = GL_LINEAR;
                break;
            default:
                throw std::runtime_error("No native Filter found");
                break;
        }

        return value;
    }

    GLenum AddressModeToNative(Render::AddressMode mode)
    {
        GLenum value;
        switch(mode)
        {
            case Render::AddressMode::Repeat:
                value = GL_REPEAT;
                break;
            case Render::AddressMode::MirroredRepeat:
                value = GL_MIRRORED_REPEAT;
                break;
            case Render::AddressMode::ClampToEdge:
                value = GL_CLAMP_TO_EDGE;
                break;
            case Render::AddressMode::ClampToBorder:
                value = GL_CLAMP_TO_BORDER;
                break;
            case Render::AddressMode::MirrorClampToEdge:
                value = GL_MIRROR_CLAMP_TO_EDGE;
                break;
            default:
                throw std::runtime_error("No native AddressMode found");
                break;
        }

        return value;
    }

    GLenum ShaderStageToNative(Render::ShaderStageFlagBits stage)
    {
        GLenum value;
        switch(stage)
        {
            case Render::ShaderStageFlagBits::Vertex:
                value = GL_VERTEX_SHADER;
                break;
            case Render::ShaderStageFlagBits::TessellationControl:
                value = GL_TESS_CONTROL_SHADER;
                break;
            case Render::ShaderStageFlagBits::TessellationEvaluation:
                value = GL_TESS_EVALUATION_SHADER;
                break;
            case Render::ShaderStageFlagBits::Geometry:
                value = GL_GEOMETRY_SHADER;
                break;
            case Render::ShaderStageFlagBits::Fragment:
                value = GL_FRAGMENT_SHADER;
                break;
            case Render::ShaderStageFlagBits::Compute:
                value = GL_COMPUTE_SHADER;
                break;
            default:
                throw std::runtime_error("No native ShaderStage found");
                break;
        }

        return value;
    }

    GLenum InputRateToNative(Render::InputRate rate)
    {
        GLenum value;
        switch(rate)
        {
            case Render::InputRate::VertexRate:
                value = 0;
                break;
            case Render::InputRate::InstanceRate:
                value = 1;
                break;
            default:
                throw std::runtime_error("No native InputRate found");
                break;
        }

        return value;
    }

    GLenum PrimitiveTopologyToNative(Render::PrimitiveTopology topology)
    {
        GLenum value;
        switch(topology)
        {
            case Render::PrimitiveTopology::PointList:
                value = GL_POINTS;
                break;
            case Render::PrimitiveTopology::LineList:
                value = GL_LINES;
                break;
            case Render::PrimitiveTopology::LineStrip:
                value = GL_LINE_STRIP;
                break;
            case Render::PrimitiveTopology::TriangleList:
                value = GL_TRIANGLES;
                break;
            case Render::PrimitiveTopology::TriangleStrip:
                value = GL_TRIANGLE_STRIP;
                break;
            case Render::PrimitiveTopology::TriangleFan:
                value = GL_TRIANGLE_FAN;
                break;
            case Render::PrimitiveTopology::LineListWithAdjacency:
                value = GL_LINES_ADJACENCY;
                break;
            case Render::PrimitiveTopology::LineStripWithAdjacency:
                value = GL_LINE_STRIP_ADJACENCY;
                break;
            case Render::PrimitiveTopology::TriangleListWithAdjacency:
                value = GL_TRIANGLES_ADJACENCY;
                break;
            case Render::PrimitiveTopology::TriangleStripWithAdjacency:
                value = GL_TRIANGLE_STRIP_ADJACENCY;
                break;
            case Render::PrimitiveTopology::PatchList:
                value = GL_PATCHES;
                break;
            default:
                throw std::runtime_error("No native PrimitiveTopology found");
                break;
        }

        return value;
    }

    GLenum BlendFactorToNative(Render::BlendFactor factor)
    {
        GLenum value;
        switch(factor)
        {
            case Render::BlendFactor::Zero:
                value = GL_ZERO;
                break;
            case Render::BlendFactor::One:
                value = GL_ONE;
                break;
            case Render::BlendFactor::SrcColor:
                value = GL_SRC_COLOR;
                break;
            case Render::BlendFactor::OneMinusSrcColor:
                value = GL_ONE_MINUS_SRC_COLOR;
                break;
            case Render::BlendFactor::DstColor:
                value = GL_DST_COLOR;
                break;
            case Render::BlendFactor::OneMinusDstColor:
                value = GL_ONE_MINUS_DST_COLOR;
                break;
            case Render::BlendFactor::SrcAlpha:
                value = GL_SRC_ALPHA;
                break;
            case Render::BlendFactor::OneMinusSrcAlpha:
                value = GL_ONE_MINUS_SRC_ALPHA;
                break;
            case Render::BlendFactor::DstAlpha:
                value = GL_DST_ALPHA;
                break;
            case Render::BlendFactor::OneMinusDstAlpha:
                value = GL_ONE_MINUS_DST_ALPHA;
                break;
            case Render::BlendFactor::ConstantColor:
                value = GL_CONSTANT_COLOR;
                break;

            case Render::BlendFactor::OneMinusConstantColor:
                value = GL_ONE_MINUS_CONSTANT_COLOR;
                break;
            case Render::BlendFactor::ConstantAlpha:
                value = GL_CONSTANT_ALPHA;
                break;
            case Render::BlendFactor::OneMinusConstantAlpha:
                value = GL_ONE_MINUS_CONSTANT_ALPHA;
                break;
            case Render::BlendFactor::SrcAlphaSaturate:
                value = GL_SRC_ALPHA_SATURATE;
                break;
            case Render::BlendFactor::Src1Color:
                value = GL_SRC1_COLOR;
                break;
            case Render::BlendFactor::OneMinusSrc1Color:
                value = GL_ONE_MINUS_SRC1_COLOR;
                break;
            case Render::BlendFactor::Src1Alpha:
                value = GL_SRC1_ALPHA;
                break;
            case Render::BlendFactor::OneMinusSrc1Alpha:
                value = GL_ONE_MINUS_SRC1_ALPHA;
                break;
            default:
                throw std::runtime_error("No native BlendFactor found");
                break;
        }

        return value;
    }

    GLenum BlendLogicOpToNative(Render::BlendLogicOp op)
    {
        GLenum value;
        switch(op)
        {
            case Render::BlendLogicOp::Clear:
                value = GL_CLEAR;
                break;
            case Render::BlendLogicOp::Set:
                value = GL_SET;
                break;
            case Render::BlendLogicOp::Copy:
                value = GL_COPY;
                break;
            case Render::BlendLogicOp::CopyInverted:
                value = GL_COPY_INVERTED;
                break;
            case Render::BlendLogicOp::NoOp:
                value = GL_NOOP;
                break;
            case Render::BlendLogicOp::Invert:
                value = GL_INVERT;
                break;
            case Render::BlendLogicOp::And:
                value = GL_AND;
                break;
            case Render::BlendLogicOp::NotAnd:
                value = GL_NAND;
                break;
            case Render::BlendLogicOp::Or:
                value = GL_OR;
                break;
            case Render::BlendLogicOp::NotOr:
                value = GL_NOR;
                break;
            case Render::BlendLogicOp::Xor:
                value = GL_XOR;
                break;

            case Render::BlendLogicOp::Equivalent:
                value = GL_EQUIV;
                break;
            case Render::BlendLogicOp::AndReverse:
                value = GL_AND_REVERSE;
                break;
            case Render::BlendLogicOp::AndInverted:
                value = GL_AND_INVERTED;
                break;
            case Render::BlendLogicOp::OrReverse:
                value = GL_OR_REVERSE;
                break;
            case Render::BlendLogicOp::OrInverted:
                value = GL_OR_INVERTED;
                break;
            default:
                throw std::runtime_error("No native BlendLogicOp found");
                break;
        }

        return value;
    }

    GLenum BlendOpToNative(Render::BlendOp op)
    {
        GLenum value;
        switch(op)
        {
            case Render::BlendOp::Add:
                value = GL_FUNC_ADD;
                break;
            case Render::BlendOp::Subtract:
                value = GL_FUNC_SUBTRACT;
                break;
            case Render::BlendOp::ReverseSubstract:
                value = GL_FUNC_REVERSE_SUBTRACT;
                break;
            case Render::BlendOp::Min:
                value = GL_MIN;
                break;
            case Render::BlendOp::Max:
                value = GL_MAX;
                break;
            default:
                throw std::runtime_error("No native BlendOp found");
                break;
        }

        return value;
    }

    GLenum StencilOpToNative(Render::StencilOp op)
    {
        GLenum value;
        switch(op)
        {
            case Render::StencilOp::Keep:
                value = GL_KEEP;
                break;
            case Render::StencilOp::Zero:
                value = GL_ZERO;
                break;
            case Render::StencilOp::Replace:
                value = GL_REPLACE;
                break;
            case Render::StencilOp::IncrementClamp:
                value = GL_INCR;
                break;
            case Render::StencilOp::IncrementWrap:
                value = GL_INCR_WRAP;
                break;
            case Render::StencilOp::DecrementClamp:
                value = GL_DECR;
                break;
            case Render::StencilOp::DecrementWrap:
                value = GL_DECR_WRAP;
                break;
            case Render::StencilOp::Invert:
                value = GL_INVERT;
                break;
            default:
                throw std::runtime_error("No native StencilOp found");
                break;
        }

        return value;
    }

    GLenum PolygonModeToNative(Render::PolygonMode mode)
    {
        GLenum value;
        switch(mode)
        {
            case Render::PolygonMode::Point:
                value = GL_POINT;
                break;
            case Render::PolygonMode::Line:
                value = GL_LINE;
                break;
            case Render::PolygonMode::Fill:
                value = GL_FILL;
                break;
            default:
                throw std::runtime_error("No native PolygonMode found");
                break;
        }

        return value;
    }

    GLenum DecodePolygonOffsetMode(Render::PolygonMode mode)
    {
        GLenum value;
        switch(mode)
        {
            case Render::PolygonMode::Point:
                value = GL_POLYGON_OFFSET_POINT;
                break;
            case Render::PolygonMode::Line:
                value = GL_POLYGON_OFFSET_LINE;
                break;
            case Render::PolygonMode::Fill:
                value = GL_POLYGON_OFFSET_FILL;
                break;
            default:
                throw std::runtime_error("No native PolygonOffsetMode found");
                break;
        }

        return value;
    }

    GLenum CullModeToNative(Render::CullMode mode)
    {
        GLenum value;
        switch(mode)
        {
            case Render::CullMode::None:
                value = OGL_CULL_MODE_NONE;
                break;
            case Render::CullMode::Front:
                value = GL_FRONT;
                break;
            case Render::CullMode::Back:
                value = GL_BACK;
                break;
            case Render::CullMode::FrontAndBack:
                value = GL_FRONT_AND_BACK;
                break;
            default:
                throw std::runtime_error("No native CullMode found");
                break;
        }

        return value;
    }

    GLenum FrontFaceToNative(Render::FrontFace face)
    {
        GLenum value;
        switch(face)
        {
            case Render::FrontFace::CounterClockwise:
                value = GL_CCW;
                break;
            case Render::FrontFace::Clockwise:
                value = GL_CW;
                break;
            default:
                throw std::runtime_error("No native FrontFace found");
                break;
        }

        return value;
    }

    GLenum IndexTypeToNative(Render::IndexType type)
    {
        GLenum value;
        switch(type)
        {
            case Render::IndexType::U8:
                value = GL_UNSIGNED_BYTE;
                break;
            case Render::IndexType::U16:
                value = GL_UNSIGNED_SHORT;
                break;
            case Render::IndexType::U32:
                value = GL_UNSIGNED_INT;
                break;
            default:
                throw std::runtime_error("No native IndexType found");
                break;
        }

        return value;
    }

    GLenum
    DecodeImageType(Render::ImageType type, bool layered, bool sampled, bool cubemap_compatible)
    {
        GLenum _inner_type;
        switch(type)
        {
            case Render::ImageType::Image1D:
            {
                if(!layered)
                    _inner_type = GL_TEXTURE_1D;
                else
                    _inner_type = GL_TEXTURE_1D_ARRAY;
            }
            break;
            case Render::ImageType::Image2D:
            {
                if(!layered)
                {
                    if(!sampled)
                    {
                        if(!cubemap_compatible)
                            _inner_type = GL_TEXTURE_2D;
                        else
                            _inner_type = GL_TEXTURE_CUBE_MAP;
                    }
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE;
                }
                else
                {
                    if(!sampled)
                    {
                        if(!cubemap_compatible)
                            _inner_type = GL_TEXTURE_2D_ARRAY;
                        else
                            _inner_type = GL_TEXTURE_CUBE_MAP_ARRAY;
                    }
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                }
            }
            break;
            case Render::ImageType::Image3D:
                _inner_type = GL_TEXTURE_3D;
                break;
        }

        return _inner_type;
    }

    std::optional<TransferImageTypeFormat>
    DecodeTransferTypeFormatPair(Render::Format format) noexcept
    {
        std::optional<TransferImageTypeFormat> value;
        switch(format)
        {
            case Render::Format::R4G4B4A4_UNORM_PACK16:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_4_4_4_4, .format = GL_RGBA};
                break;
            case Render::Format::B4G4R4A4_UNORM_PACK16:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_4_4_4_4, .format = GL_BGRA};
                break;
            case Render::Format::R5G6B5_UNORM_PACK16:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_6_5, .format = GL_RGB};
                break;
            case Render::Format::B5G6R5_UNORM_PACK16:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_6_5_REV, .format = GL_RGB};
                break;
            case Render::Format::R5G5B5A1_UNORM_PACK16:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_5_5_1, .format = GL_RGBA};
                break;
            case Render::Format::B5G5R5A1_UNORM_PACK16:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_5_5_1, .format = GL_BGRA};
                break;
            case Render::Format::A1R5G5B5_UNORM_PACK16:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_1_5_5_5_REV,
                                                .format = GL_BGRA};
                break;
            case Render::Format::R8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RED};
                break;
            case Render::Format::R8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RED};
                break;
            case Render::Format::R8_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RED};
                break;
            case Render::Format::R8G8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RG};
                break;
            case Render::Format::R8G8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RG};
                break;
            case Render::Format::R8G8_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R8G8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R8G8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RG};
                break;
            case Render::Format::R8G8B8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGB};
                break;
            case Render::Format::R8G8B8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGB};
                break;
            case Render::Format::R8G8B8_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R8G8B8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R8G8B8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGB};
                break;
            case Render::Format::B8G8R8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGR};
                break;
            case Render::Format::B8G8R8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_BGR};
                break;
            case Render::Format::B8G8R8_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGR_INTEGER};
                break;
            case Render::Format::B8G8R8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_BGR_INTEGER};
                break;
            case Render::Format::B8G8R8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGR};
                break;
            case Render::Format::R8G8B8A8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA};
                break;
            case Render::Format::R8G8B8A8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGBA};
                break;
            case Render::Format::R8G8B8A8_UINT:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R8G8B8A8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R8G8B8A8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA};
                break;
            case Render::Format::B8G8R8A8_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGRA};
                break;
            case Render::Format::B8G8R8A8_SNORM:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_BGRA};
                break;
            case Render::Format::B8G8R8A8_UINT:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGRA_INTEGER};
                break;
            case Render::Format::B8G8R8A8_SINT:
                value = TransferImageTypeFormat{.type = GL_BYTE, .format = GL_BGRA_INTEGER};
                break;
            case Render::Format::B8G8R8A8_UNORM_SRGB:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_BGRA};
                break;
            case Render::Format::A8B8G8R8_UNORM_PACK32:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_INT_8_8_8_8_REV, .format = GL_RGBA};
                break;
            case Render::Format::A8B8G8R8_UINT_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_8_8_8_8_REV,
                                                .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::A8B8G8R8_UNORM_SRGB_PACK32:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_INT_8_8_8_8_REV, .format = GL_RGBA};
                break;
            case Render::Format::A2R10G10B10_UNORM_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                                .format = GL_BGRA};
                break;
            case Render::Format::A2R10G10B10_UINT_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                                .format = GL_BGRA_INTEGER};
                break;
            case Render::Format::A2B10G10R10_UNORM_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                                .format = GL_RGBA};
                break;
            case Render::Format::A2B10G10R10_UINT_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                                .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R16_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RED};
                break;
            case Render::Format::R16_SNORM:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RED};
                break;
            case Render::Format::R16_UINT:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R16_SINT:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R16_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RED};
                break;
            case Render::Format::R16G16_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RG};
                break;
            case Render::Format::R16G16_SNORM:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RG};
                break;
            case Render::Format::R16G16_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R16G16_SINT:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R16G16_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RG};
                break;
            case Render::Format::R16G16B16_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGB};
                break;
            case Render::Format::R16G16B16_SNORM:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGB};
                break;
            case Render::Format::R16G16B16_UINT:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R16G16B16_SINT:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R16G16B16_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RGB};
                break;
            case Render::Format::R16G16B16A16_UNORM:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGBA};
                break;
            case Render::Format::R16G16B16A16_SNORM:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGBA};
                break;
            case Render::Format::R16G16B16A16_UINT:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R16G16B16A16_SINT:
                value = TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R16G16B16A16_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RGBA};
                break;
            case Render::Format::R32_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R32_SINT:
                value = TransferImageTypeFormat{.type = GL_INT, .format = GL_RED_INTEGER};
                break;
            case Render::Format::R32_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RED};
                break;
            case Render::Format::R32G32_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R32G32_SINT:
                value = TransferImageTypeFormat{.type = GL_INT, .format = GL_RG_INTEGER};
                break;
            case Render::Format::R32G32_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RG};
                break;
            case Render::Format::R32G32B32_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R32G32B32_SINT:
                value = TransferImageTypeFormat{.type = GL_INT, .format = GL_RGB_INTEGER};
                break;
            case Render::Format::R32G32B32_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RGB};
                break;
            case Render::Format::R32G32B32A32_UINT:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R32G32B32A32_SINT:
                value = TransferImageTypeFormat{.type = GL_INT, .format = GL_RGBA_INTEGER};
                break;
            case Render::Format::R32G32B32A32_SFLOAT:
                value = TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RGBA};
                break;
            case Render::Format::B10G11R11_UFLOAT_PACK32:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_INT_10F_11F_11F_REV,
                                                .format = GL_RGB};
                break;
            case Render::Format::E5B9G9R9_UFLOAT_PACK32:
                value =
                    TransferImageTypeFormat{.type = GL_UNSIGNED_INT_5_9_9_9_REV, .format = GL_RGB};
                break;
            case Render::Format::A4R4G4B4_UNORM_PACK16:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_4_4_4_4_REV,
                                                .format = GL_BGRA};
                break;
            case Render::Format::A4B4G4R4_UNORM_PACK16:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_4_4_4_4_REV,
                                                .format = GL_RGBA};
                break;
            case Render::Format::A1B5G5R5_UNORM_PACK16:
                value = TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_1_5_5_5_REV,
                                                .format = GL_RGBA};
                break;
            case Render::Format::UNDEFINED:
            case Render::Format::R4G4_UNORM_PACK8:
            //scled are not supported as image formats
            case Render::Format::R8_USCALED:
            case Render::Format::R8_SSCALED:
            case Render::Format::R8G8_USCALED:
            case Render::Format::R8G8_SSCALED:
            case Render::Format::R8G8B8_USCALED:
            case Render::Format::R8G8B8_SSCALED:
            case Render::Format::B8G8R8_USCALED:
            case Render::Format::B8G8R8_SSCALED:
            case Render::Format::R8G8B8A8_USCALED:
            case Render::Format::R8G8B8A8_SSCALED:
            case Render::Format::B8G8R8A8_USCALED:
            case Render::Format::B8G8R8A8_SSCALED:
            //A8B8G8R8 for signed not supported due to the lack of GL_INT_8_8_8_8
            case Render::Format::A8B8G8R8_SNORM_PACK32:
            case Render::Format::A8B8G8R8_USCALED_PACK32:
            case Render::Format::A8B8G8R8_SSCALED_PACK32:
            case Render::Format::A8B8G8R8_SINT_PACK32:
            //no signed A2_RGB10 formats
            case Render::Format::A2R10G10B10_SNORM_PACK32:
            case Render::Format::A2R10G10B10_USCALED_PACK32:
            case Render::Format::A2R10G10B10_SSCALED_PACK32:
            case Render::Format::A2R10G10B10_SINT_PACK32:
            case Render::Format::A2B10G10R10_SNORM_PACK32:
            case Render::Format::A2B10G10R10_USCALED_PACK32:
            case Render::Format::A2B10G10R10_SSCALED_PACK32:
            case Render::Format::A2B10G10R10_SINT_PACK32:
            case Render::Format::R16_USCALED:
            case Render::Format::R16_SSCALED:
            case Render::Format::R16G16_USCALED:
            case Render::Format::R16G16_SSCALED:
            case Render::Format::R16G16B16_USCALED:
            case Render::Format::R16G16B16_SSCALED:
            case Render::Format::R16G16B16A16_USCALED:
            case Render::Format::R16G16B16A16_SSCALED:
            //64-bit per channel formats are only for vertex input
            case Render::Format::R64_UINT:
            case Render::Format::R64_SINT:
            case Render::Format::R64_SFLOAT:
            case Render::Format::R64G64_UINT:
            case Render::Format::R64G64_SINT:
            case Render::Format::R64G64_SFLOAT:
            case Render::Format::R64G64B64_UINT:
            case Render::Format::R64G64B64_SINT:
            case Render::Format::R64G64B64_SFLOAT:
            case Render::Format::R64G64B64A64_UINT:
            case Render::Format::R64G64B64A64_SINT:
            case Render::Format::R64G64B64A64_SFLOAT:
            //depth-stencil formats are not transferable(due to the VK aspect + maintenance extensions + impl. defined layout -> do not care)
            case Render::Format::D16_UNORM:
            case Render::Format::X8_D24_UNORM_PACK32:
            case Render::Format::D32_SFLOAT:
            case Render::Format::S8_UINT:
            case Render::Format::D16_UNORM_S8_UINT:
            case Render::Format::D24_UNORM_S8_UINT:
            case Render::Format::D32_SFLOAT_S8_UINT:
            //compressed formats are being transfered as is without type and format
            case Render::Format::BC1_RGB_UNORM_BLOCK:
            case Render::Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Render::Format::BC1_RGBA_UNORM_BLOCK:
            case Render::Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Render::Format::BC2_UNORM_BLOCK:
            case Render::Format::BC2_UNORM_SRGB_BLOCK:
            case Render::Format::BC3_UNORM_BLOCK:
            case Render::Format::BC3_UNORM_SRGB_BLOCK:
            case Render::Format::BC4_UNORM_BLOCK:
            case Render::Format::BC4_SNORM_BLOCK:
            case Render::Format::BC5_UNORM_BLOCK:
            case Render::Format::BC5_SNORM_BLOCK:
            case Render::Format::BC6H_UFLOAT_BLOCK:
            case Render::Format::BC6H_SFLOAT_BLOCK:
            case Render::Format::BC7_UNORM_BLOCK:
            case Render::Format::BC7_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
            case Render::Format::EAC_R11_UNORM_BLOCK:
            case Render::Format::EAC_R11_SNORM_BLOCK:
            case Render::Format::EAC_R11G11_UNORM_BLOCK:
            case Render::Format::EAC_R11G11_SNORM_BLOCK:
            case Render::Format::A8_UNORM: //legacy
                break;
        }

        return value;
    }

    std::optional<VertexInputTypeSize> DecodeVertexInputTypeSizePair(Render::Format format) noexcept
    {
        std::optional<VertexInputTypeSize> value;
        switch(format)
        {
            case Render::Format::R8_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8_SNORM:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8_SSCALED:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8_SINT:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8_SNORM:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8_SSCALED:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8_SINT:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8B8_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8B8_SNORM:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8B8_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8B8_SSCALED:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8B8_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8B8_SINT:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8B8A8_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8B8A8_SNORM:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R8G8B8A8_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8B8A8_SSCALED:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R8G8B8A8_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R8G8B8A8_SINT:
                value = VertexInputTypeSize{.type = GL_BYTE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::B8G8R8A8_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_BYTE,
                                            .size = GL_BGRA,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::A2R10G10B10_UNORM_PACK32:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                            .size = GL_BGRA,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::A2R10G10B10_SNORM_PACK32:
                value = VertexInputTypeSize{.type = GL_INT_2_10_10_10_REV,
                                            .size = GL_BGRA,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::A2B10G10R10_UNORM_PACK32:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::A2B10G10R10_SNORM_PACK32:
                value = VertexInputTypeSize{.type = GL_INT_2_10_10_10_REV,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::A2B10G10R10_USCALED_PACK32:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::A2B10G10R10_SSCALED_PACK32:
                value = VertexInputTypeSize{.type = GL_INT_2_10_10_10_REV,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16_SNORM:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16_SSCALED:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16_SINT:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16_SFLOAT:
                value = VertexInputTypeSize{.type = GL_HALF_FLOAT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16_SNORM:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16_SSCALED:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16_SINT:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16_SFLOAT:
                value = VertexInputTypeSize{.type = GL_HALF_FLOAT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16B16_SNORM:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16B16_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16_SSCALED:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16B16_SINT:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16B16_SFLOAT:
                value = VertexInputTypeSize{.type = GL_HALF_FLOAT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16A16_UNORM:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16B16A16_SNORM:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Normalized};
                break;
            case Render::Format::R16G16B16A16_USCALED:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16A16_SSCALED:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R16G16B16A16_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16B16A16_SINT:
                value = VertexInputTypeSize{.type = GL_SHORT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R16G16B16A16_SFLOAT:
                value = VertexInputTypeSize{.type = GL_HALF_FLOAT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R32_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32_SINT:
                value = VertexInputTypeSize{.type = GL_INT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32_SFLOAT:
                value = VertexInputTypeSize{.type = GL_FLOAT,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R32G32_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32_SINT:
                value = VertexInputTypeSize{.type = GL_INT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32_SFLOAT:
                value = VertexInputTypeSize{.type = GL_FLOAT,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R32G32B32_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32B32_SINT:
                value = VertexInputTypeSize{.type = GL_INT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32B32_SFLOAT:
                value = VertexInputTypeSize{.type = GL_FLOAT,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R32G32B32A32_UINT:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32B32A32_SINT:
                value = VertexInputTypeSize{.type = GL_INT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Integer};
                break;
            case Render::Format::R32G32B32A32_SFLOAT:
                value = VertexInputTypeSize{.type = GL_FLOAT,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            case Render::Format::R64_SFLOAT:
                value = VertexInputTypeSize{.type = GL_DOUBLE,
                                            .size = 1,
                                            .function_type = VertexInputFunctionType::Double};
                break;
            case Render::Format::R64G64_SFLOAT:
                value = VertexInputTypeSize{.type = GL_DOUBLE,
                                            .size = 2,
                                            .function_type = VertexInputFunctionType::Double};
                break;
            case Render::Format::R64G64B64_SFLOAT:
                value = VertexInputTypeSize{.type = GL_DOUBLE,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Double};
                break;
            case Render::Format::R64G64B64A64_SFLOAT:
                value = VertexInputTypeSize{.type = GL_DOUBLE,
                                            .size = 4,
                                            .function_type = VertexInputFunctionType::Double};
                break;
            case Render::Format::B10G11R11_UFLOAT_PACK32:
                value = VertexInputTypeSize{.type = GL_UNSIGNED_INT_10F_11F_11F_REV,
                                            .size = 3,
                                            .function_type = VertexInputFunctionType::Unnormalized};
                break;
            /*
                An INVALID_ENUM error is generated by VertexAttribIFormat and VertexAttribLFormat if type is UNSIGNED_INT_10F_11F_11F_REV.
                An INVALID_OPERATION error is generated under any of the following
                conditions:
                • size is BGRA and type is not UNSIGNED_BYTE, INT_2_10_10_10_REV
                or UNSIGNED_INT_2_10_10_10_REV;
                • type is INT_2_10_10_10_REV or UNSIGNED_INT_2_10_10_10_-
                REV, and size is neither 4 nor BGRA;
                • type is UNSIGNED_INT_10F_11F_11F_REV and size is not 3;
                • size is BGRA and normalized is FALSE.
                An INVALID_VALUE error is generated if relativeoffset is larger than the
                value of MAX_VERTEX_ATTRIB_RELATIVE_OFFSET.
            */
            case Render::Format::UNDEFINED:
            case Render::Format::R4G4_UNORM_PACK8:
            case Render::Format::R4G4B4A4_UNORM_PACK16:
            case Render::Format::B4G4R4A4_UNORM_PACK16:
            case Render::Format::R5G6B5_UNORM_PACK16:
            case Render::Format::B5G6R5_UNORM_PACK16:
            case Render::Format::R5G5B5A1_UNORM_PACK16:
            case Render::Format::B5G5R5A1_UNORM_PACK16:
            case Render::Format::A1R5G5B5_UNORM_PACK16:
            case Render::Format::R8_UNORM_SRGB:
            case Render::Format::R8G8_UNORM_SRGB:
            case Render::Format::R8G8B8_UNORM_SRGB:
            case Render::Format::B8G8R8_UNORM:
            case Render::Format::B8G8R8_SNORM:
            case Render::Format::B8G8R8_USCALED:
            case Render::Format::B8G8R8_SSCALED:
            case Render::Format::B8G8R8_UINT:
            case Render::Format::B8G8R8_SINT:
            case Render::Format::B8G8R8_UNORM_SRGB:
            case Render::Format::R8G8B8A8_UNORM_SRGB:
            case Render::Format::B8G8R8A8_SNORM:
            case Render::Format::B8G8R8A8_USCALED:
            case Render::Format::B8G8R8A8_SSCALED:
            case Render::Format::B8G8R8A8_UINT:
            case Render::Format::B8G8R8A8_SINT:
            case Render::Format::B8G8R8A8_UNORM_SRGB:
            case Render::Format::A8B8G8R8_UNORM_PACK32:
            case Render::Format::A8B8G8R8_SNORM_PACK32:
            case Render::Format::A8B8G8R8_USCALED_PACK32:
            case Render::Format::A8B8G8R8_SSCALED_PACK32:
            case Render::Format::A8B8G8R8_UINT_PACK32:
            case Render::Format::A8B8G8R8_SINT_PACK32:
            case Render::Format::A8B8G8R8_UNORM_SRGB_PACK32:
            case Render::Format::A2R10G10B10_USCALED_PACK32:
            case Render::Format::A2R10G10B10_SSCALED_PACK32:
            case Render::Format::A2R10G10B10_UINT_PACK32:
            case Render::Format::A2R10G10B10_SINT_PACK32:
            case Render::Format::A2B10G10R10_UINT_PACK32:
            case Render::Format::A2B10G10R10_SINT_PACK32:
            case Render::Format::R64_UINT:
            case Render::Format::R64_SINT:
            case Render::Format::R64G64_UINT:
            case Render::Format::R64G64_SINT:
            case Render::Format::R64G64B64_UINT:
            case Render::Format::R64G64B64_SINT:
            case Render::Format::R64G64B64A64_UINT:
            case Render::Format::R64G64B64A64_SINT:
            case Render::Format::E5B9G9R9_UFLOAT_PACK32:
            case Render::Format::D16_UNORM:
            case Render::Format::X8_D24_UNORM_PACK32:
            case Render::Format::D32_SFLOAT:
            case Render::Format::S8_UINT:
            case Render::Format::D16_UNORM_S8_UINT:
            case Render::Format::D24_UNORM_S8_UINT:
            case Render::Format::D32_SFLOAT_S8_UINT:
            case Render::Format::BC1_RGB_UNORM_BLOCK:
            case Render::Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Render::Format::BC1_RGBA_UNORM_BLOCK:
            case Render::Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Render::Format::BC2_UNORM_BLOCK:
            case Render::Format::BC2_UNORM_SRGB_BLOCK:
            case Render::Format::BC3_UNORM_BLOCK:
            case Render::Format::BC3_UNORM_SRGB_BLOCK:
            case Render::Format::BC4_UNORM_BLOCK:
            case Render::Format::BC4_SNORM_BLOCK:
            case Render::Format::BC5_UNORM_BLOCK:
            case Render::Format::BC5_SNORM_BLOCK:
            case Render::Format::BC6H_UFLOAT_BLOCK:
            case Render::Format::BC6H_SFLOAT_BLOCK:
            case Render::Format::BC7_UNORM_BLOCK:
            case Render::Format::BC7_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Render::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            case Render::Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
            case Render::Format::EAC_R11_UNORM_BLOCK:
            case Render::Format::EAC_R11_SNORM_BLOCK:
            case Render::Format::EAC_R11G11_UNORM_BLOCK:
            case Render::Format::EAC_R11G11_SNORM_BLOCK:
            case Render::Format::A4R4G4B4_UNORM_PACK16:
            case Render::Format::A4B4G4R4_UNORM_PACK16:
            case Render::Format::A1B5G5R5_UNORM_PACK16:
            case Render::Format::A8_UNORM:
                break;
        }

        return value;
    }

    ArrayDecodeResult<9>
    DebugMessengerTypeFlagsToNativeInverted(Render::DebugMessengerTypeFlags types)
    {
        ArrayDecodeResult<9> res = {.data = {}, .size = 0};

        if(!(types & Render::DebugMessengerTypeFlagBits::General))
        {
            res.data[res.size++] = GL_DEBUG_TYPE_PORTABILITY;
            res.data[res.size++] = GL_DEBUG_TYPE_OTHER;
        }

        if(!(types & Render::DebugMessengerTypeFlagBits::Validation))
        {
            res.data[res.size++] = GL_DEBUG_TYPE_ERROR;
            res.data[res.size++] = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
            res.data[res.size++] = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
        }

        if(!(types & Render::DebugMessengerTypeFlagBits::Performance))
        {
            res.data[res.size++] = GL_DEBUG_TYPE_PERFORMANCE;
        }

        res.data[res.size++] = GL_DEBUG_TYPE_MARKER;
        res.data[res.size++] = GL_DEBUG_TYPE_PUSH_GROUP;
        res.data[res.size++] = GL_DEBUG_TYPE_POP_GROUP;

        return res;
    }

    ArrayDecodeResult<4>
    DebugMessengerSeverityFlagsToNativeInverted(Render::DebugMessengerSeverityFlags severities)
    {
        ArrayDecodeResult<4> res = {.data = {}, .size = 0};

        if(!(severities & Render::DebugMessengerSeverityFlagBits::Verbose))
            res.data[res.size++] = GL_DEBUG_SEVERITY_NOTIFICATION;

        if(!(severities & Render::DebugMessengerSeverityFlagBits::Info))
            res.data[res.size++] = GL_DEBUG_SEVERITY_LOW;

        if(!(severities & Render::DebugMessengerSeverityFlagBits::Warning))
            res.data[res.size++] = GL_DEBUG_SEVERITY_MEDIUM;

        if(!(severities & Render::DebugMessengerSeverityFlagBits::Error))
            res.data[res.size++] = GL_DEBUG_SEVERITY_HIGH;

        return res;
    }

    Render::DebugMessengerTypeFlagBits NativeDebugMessengerTypeFlagBitToSpec(GLenum type)
    {
        Render::DebugMessengerTypeFlagBits value;
        switch(type)
        {
            case GL_DEBUG_TYPE_ERROR:
                value = Render::DebugMessengerTypeFlagBits::Validation;
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                value = Render::DebugMessengerTypeFlagBits::Validation;
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                value = Render::DebugMessengerTypeFlagBits::Validation;
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                value = Render::DebugMessengerTypeFlagBits::General;
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                value = Render::DebugMessengerTypeFlagBits::Performance;
                break;
            case GL_DEBUG_TYPE_OTHER:
                value = Render::DebugMessengerTypeFlagBits::General;
                break;
            default:
                value = Render::DebugMessengerTypeFlagBits::General;
                break;
        }

        return value;
    }

    Render::DebugMessengerSeverityFlagBits
    NativeDebugMessengerSeverityFlagBitToSpec(GLenum severity)
    {
        Render::DebugMessengerSeverityFlagBits value;
        switch(severity)
        {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                value = Render::DebugMessengerSeverityFlagBits::Verbose;
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                value = Render::DebugMessengerSeverityFlagBits::Error;
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                value = Render::DebugMessengerSeverityFlagBits::Warning;
                break;
            case GL_DEBUG_SEVERITY_LOW:
                value = Render::DebugMessengerSeverityFlagBits::Info;
                break;
            default:
                value = Render::DebugMessengerSeverityFlagBits::
                    Verbose; //return Verbose due to driver-specific codes
                break;
        }

        return value;
    }

    static GLbitfield
    DecodeAccessFlags(std::span<const std::pair<Render::AccessFlagBits, GLbitfield>> mapping,
                      Render::AccessFlags access)
    {
        if(access == 0)
            return 0;

        GLbitfield mask = 0;
        for(const auto& pr: mapping)
        {
            if(access & pr.first)
                mask |= pr.second;
        }

        return mask;
    }

    GLbitfield PipelineBarrierToNative(const Render::PipelineBarrier& barrier)
    {
        //only see on dst accesss
        constexpr static std::pair<Render::AccessFlagBits, GLbitfield> buffer_mapping[] = {
            {Render::AccessFlagBits::AccessVertexAttributeReadBit,
             GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT},
            {Render::AccessFlagBits::AccessIndexReadBit, GL_ELEMENT_ARRAY_BARRIER_BIT},
            {Render::AccessFlagBits::AccessUniformReadBit, GL_UNIFORM_BARRIER_BIT},
            {Render::AccessFlagBits::AccessShaderReadBit, GL_SHADER_STORAGE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessShaderWriteBit, GL_SHADER_STORAGE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessIndirectCommandReadBit, GL_COMMAND_BARRIER_BIT},
            {Render::AccessFlagBits::AccessHostReadBit,
             GL_PIXEL_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT |
                 GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessHostWriteBit,
             GL_PIXEL_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT |
                 GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessTransferReadBit,
             GL_PIXEL_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessTransferWriteBit,
             GL_PIXEL_BUFFER_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessMemoryReadBit, GL_ALL_BARRIER_BITS},
            {Render::AccessFlagBits::AccessMemoryWriteBit, GL_ALL_BARRIER_BITS},
        };

        constexpr static std::pair<Render::AccessFlagBits, GLbitfield> image_mapping[] = {
            {Render::AccessFlagBits::AccessShaderReadBit,
             GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT},
            {Render::AccessFlagBits::AccessShaderWriteBit, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT},
            {Render::AccessFlagBits::AccessColorAttachmentReadBit, GL_FRAMEBUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessColorAttachmentWriteBit, GL_FRAMEBUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessDepthStencilAttachmentReadBit,
             GL_FRAMEBUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessDepthStencilAttachmentWriteBit,
             GL_FRAMEBUFFER_BARRIER_BIT},
            {Render::AccessFlagBits::AccessTransferReadBit, GL_TEXTURE_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessTransferWriteBit, GL_TEXTURE_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessHostReadBit, GL_TEXTURE_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessHostWriteBit, GL_TEXTURE_UPDATE_BARRIER_BIT},
            {Render::AccessFlagBits::AccessMemoryReadBit, GL_ALL_BARRIER_BITS},
            {Render::AccessFlagBits::AccessMemoryWriteBit, GL_ALL_BARRIER_BITS},
        };

        GLbitfield native = 0;
        for(const auto& buffer: barrier.buffer_barriers)
        {
            native |=
                DecodeAccessFlags({buffer_mapping, std::size(buffer_mapping)}, buffer.dst_access);
        }

        for(const auto& image: barrier.image_barriers)
        {
            native |=
                DecodeAccessFlags({image_mapping, std::size(image_mapping)}, image.dst_access);
        }

        return native;
    }

    void GLAPIENTRY debug_messenger_callback(GLenum source,
                                             GLenum type,
                                             GLuint id,
                                             GLenum severity,
                                             GLsizei length,
                                             const GLchar* message,
                                             const void* user_param)
    {
        const std::function<Render::DebugMessengerCallback>* callback =
            reinterpret_cast<const std::function<Render::DebugMessengerCallback>*>(user_param);

        (*callback)(NativeDebugMessengerSeverityFlagBitToSpec(severity),
                    NativeDebugMessengerTypeFlagBitToSpec(type),
                    id,
                    {reinterpret_cast<const char*>(message), static_cast<std::size_t>(length)});
    }

    void EnableDebugMessenger(const GladGLContext& loader)
    {
        loader.Enable(GL_DEBUG_OUTPUT);
#ifndef NDEBUG
        loader.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    }

    void SetDebugMessenger(const GladGLContext& loader, const Render::DebugMessengerInfo& info)
    {
        auto filter_types = DebugMessengerTypeFlagsToNativeInverted(info.types);
        auto filter_severities = DebugMessengerSeverityFlagsToNativeInverted(info.severities);

        loader.DebugMessageControl(GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   GL_DONT_CARE,
                                   0,
                                   nullptr,
                                   GL_TRUE); //drop all filters

        for(std::size_t i = 0; i < filter_types.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       filter_types.data[i],
                                       GL_DONT_CARE,
                                       0,
                                       nullptr,
                                       GL_FALSE);

        for(std::size_t i = 0; i < filter_severities.size; i++)
            loader.DebugMessageControl(GL_DONT_CARE,
                                       GL_DONT_CARE,
                                       filter_severities.data[i],
                                       0,
                                       nullptr,
                                       GL_FALSE);

        loader.DebugMessageCallback(debug_messenger_callback, &info.callback);
    }

    std::optional<Render::Format> DecodeSurfaceFormat(/*const GladGLContext& loader,*/
                                                      const SurfaceConfig& config)
    {
        std::optional<Render::Format> out;

        if(config.color_bits == 8)
        {
            if(config.green_bits == 4 && config.green_shift == 0 && config.red_bits == 4 &&
               config.red_shift == 4 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::R4G4_UNORM_PACK8;
            else if(config.red_bits == 8 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::R8_UNORM;
            else if(config.red_bits == 8 && config.is_srgb == true && config.is_float == false)
                out = Render::Format::R8_UNORM_SRGB;
            else if(config.alpha_bits == 8 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::A8_UNORM;
        }
        else if(config.color_bits == 16)
        {
            if(config.alpha_bits == 4 && config.alpha_shift == 0 && config.blue_bits == 4 &&
               config.blue_shift == 4 && config.green_bits == 4 && config.green_shift == 8 &&
               config.red_bits == 4 && config.red_shift == 12 && config.is_srgb == false &&
               config.is_float == false)
                out = Render::Format::R4G4B4A4_UNORM_PACK16;
            else if(config.alpha_bits == 4 && config.alpha_shift == 0 && config.red_bits == 4 &&
                    config.red_shift == 4 && config.green_bits == 4 && config.green_shift == 8 &&
                    config.blue_bits == 4 && config.blue_shift == 12 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::B4G4R4A4_UNORM_PACK16;
            else if(config.blue_bits == 5 && config.blue_shift == 0 && config.green_bits == 6 &&
                    config.green_shift == 5 && config.red_bits == 5 && config.red_shift == 11 &&
                    config.is_srgb == false && config.is_float == false)
                out = Render::Format::R5G6B5_UNORM_PACK16;
            else if(config.red_bits == 5 && config.red_shift == 0 && config.green_bits == 6 &&
                    config.green_shift == 5 && config.blue_bits == 5 && config.blue_shift == 11 &&
                    config.is_srgb == false && config.is_float == false)
                out = Render::Format::B5G6R5_UNORM_PACK16;
            else if(config.alpha_bits == 1 && config.alpha_shift == 0 && config.blue_bits == 5 &&
                    config.blue_shift == 1 && config.green_bits == 5 && config.green_shift == 6 &&
                    config.red_bits == 5 && config.red_shift == 11 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::R5G5B5A1_UNORM_PACK16;
            else if(config.alpha_bits == 1 && config.alpha_shift == 0 && config.red_bits == 5 &&
                    config.red_shift == 1 && config.green_bits == 5 && config.green_shift == 6 &&
                    config.blue_bits == 5 && config.blue_shift == 11 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::B5G5R5A1_UNORM_PACK16;
            else if(config.blue_bits == 5 && config.blue_shift == 0 && config.green_bits == 5 &&
                    config.green_shift == 5 && config.red_bits == 5 && config.red_shift == 10 &&
                    config.alpha_bits == 1 && config.alpha_shift == 15 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A1R5G5B5_UNORM_PACK16;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::R8G8_UNORM;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.is_srgb == true && config.is_float == false)
                out = Render::Format::R8G8_UNORM_SRGB;
            else if(config.red_bits == 16 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::R16_UNORM;
            else if(config.red_bits == 16 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::R16_SFLOAT;
            else if(config.blue_bits == 4 && config.blue_shift == 0 && config.green_bits == 4 &&
                    config.green_shift == 4 && config.red_bits == 4 && config.red_shift == 8 &&
                    config.alpha_bits == 4 && config.alpha_shift == 12 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A4R4G4B4_UNORM_PACK16;
            else if(config.red_bits == 4 && config.red_shift == 0 && config.green_bits == 4 &&
                    config.green_shift == 4 && config.blue_bits == 4 && config.blue_shift == 8 &&
                    config.alpha_bits == 4 && config.alpha_shift == 12 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A4B4G4R4_UNORM_PACK16;
            else if(config.red_bits == 5 && config.red_shift == 0 && config.green_bits == 5 &&
                    config.green_shift == 5 && config.blue_bits == 5 && config.blue_shift == 10 &&
                    config.alpha_bits == 1 && config.alpha_shift == 15 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A1B5G5R5_UNORM_PACK16;
        }
        else if(config.color_bits == 24)
        {
            if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
               config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
               config.is_srgb == false && config.is_float == false)
                out = Render::Format::R8G8B8_UNORM;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
                    config.is_srgb == true && config.is_float == false)
                out = Render::Format::R8G8B8_UNORM_SRGB;
            else if(config.blue_bits == 8 && config.blue_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.red_bits == 8 && config.red_shift == 16 &&
                    config.is_srgb == false && config.is_float == false)
                out = Render::Format::B8G8R8_UNORM;
            else if(config.blue_bits == 8 && config.blue_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.red_bits == 8 && config.red_shift == 16 &&
                    config.is_srgb == true && config.is_float == false)
                out = Render::Format::B8G8R8_UNORM_SRGB;
        }
        else if(config.color_bits == 32)
        {
            if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
               config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
               config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == false &&
               config.is_float == false)
                out = Render::Format::R8G8B8A8_UNORM;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
                    config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == true &&
                    config.is_float == false)
                out = Render::Format::R8G8B8A8_UNORM_SRGB;
            else if(config.blue_bits == 8 && config.blue_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.red_bits == 8 && config.red_shift == 16 &&
                    config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::B8G8R8A8_UNORM;
            else if(config.blue_bits == 8 && config.blue_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.red_bits == 8 && config.red_shift == 16 &&
                    config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == true &&
                    config.is_float == false)
                out = Render::Format::B8G8R8A8_UNORM_SRGB;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
                    config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A8B8G8R8_UNORM_PACK32;
            else if(config.red_bits == 8 && config.red_shift == 0 && config.green_bits == 8 &&
                    config.green_shift == 8 && config.blue_bits == 8 && config.blue_shift == 16 &&
                    config.alpha_bits == 8 && config.alpha_shift == 24 && config.is_srgb == true &&
                    config.is_float == false)
                out = Render::Format::A8B8G8R8_UNORM_SRGB_PACK32;
            else if(config.blue_bits == 10 && config.blue_shift == 0 && config.green_bits == 10 &&
                    config.green_shift == 10 && config.red_bits == 10 && config.red_shift == 20 &&
                    config.alpha_bits == 2 && config.alpha_shift == 30 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A2R10G10B10_UNORM_PACK32;
            else if(config.red_bits == 10 && config.red_shift == 0 && config.green_bits == 10 &&
                    config.green_shift == 10 && config.blue_bits == 10 && config.blue_shift == 20 &&
                    config.alpha_bits == 2 && config.alpha_shift == 30 && config.is_srgb == false &&
                    config.is_float == false)
                out = Render::Format::A2B10G10R10_UNORM_PACK32;
            else if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
                    config.green_shift == 16 && config.is_srgb == false && config.is_float == false)
                out = Render::Format::R16G16_UNORM;
            else if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
                    config.green_shift == 16 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::R16G16_SFLOAT;
            else if(config.red_bits == 32 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::R32_SFLOAT;
            /*else if(config.red_bits == 11 && config.red_shift == 0 && config.green_bits == 11 && config.green_shift == 11 &&
                    config.blue_bits == 10 && config.blue_shift == 22 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::B10G11R11_UFLOAT_PACK32;
            else if(config.red_bits == 9 && config.red_shift == 0 && config.green_bits == 9 && config.green_shift == 9 &&
                    config.blue_bits == 9 && config.blue_shift == 18 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::E5B9G9R9_UFLOAT_PACK32;*/
        }
        else if(config.color_bits == 48)
        {
            if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
               config.green_shift == 16 && config.blue_bits == 16 && config.blue_shift == 32 &&
               config.is_srgb == false && config.is_float == false)
                out = Render::Format::R16G16B16_UNORM;
            else if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
                    config.green_shift == 16 && config.blue_bits == 16 && config.blue_shift == 32 &&
                    config.is_srgb == false && config.is_float == true)
                out = Render::Format::R16G16B16_SFLOAT;
        }
        else if(config.color_bits == 64)
        {
            if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
               config.green_shift == 16 && config.blue_bits == 16 && config.blue_shift == 32 &&
               config.alpha_bits == 16 && config.alpha_shift == 48 && config.is_srgb == false &&
               config.is_float == false)
                out = Render::Format::R16G16B16A16_UNORM;
            else if(config.red_bits == 16 && config.red_shift == 0 && config.green_bits == 16 &&
                    config.green_shift == 16 && config.blue_bits == 16 && config.blue_shift == 32 &&
                    config.alpha_bits == 16 && config.alpha_shift == 48 &&
                    config.is_srgb == false && config.is_float == true)
                out = Render::Format::R16G16B16A16_SFLOAT;
            else if(config.red_bits == 32 && config.red_shift == 0 && config.green_bits == 32 &&
                    config.green_shift == 32 && config.is_srgb == false && config.is_float == true)
                out = Render::Format::R32G32_SFLOAT;
        }
        else if(config.color_bits == 96)
        {
            if(config.red_bits == 32 && config.red_shift == 0 && config.green_bits == 32 &&
               config.green_shift == 32 && config.blue_bits == 32 && config.blue_shift == 64 &&
               config.is_srgb == false && config.is_float == true)
                out = Render::Format::R32G32B32_SFLOAT;
        }
        else if(config.color_bits == 128)
        {
            if(config.red_bits == 32 && config.red_shift == 0 && config.green_bits == 32 &&
               config.green_shift == 32 && config.blue_bits == 32 && config.blue_shift == 64 &&
               config.alpha_bits == 32 && config.alpha_shift == 96 && config.is_srgb == false &&
               config.is_float == true)
                out = Render::Format::R32G32B32A32_SFLOAT;
        }

        /*if(out.has_value())
        {
            GLenum inner_type = DecodeImageType(Render::ImageType::Image2D, false, false, {});
            auto native_format_opt = FormatToNative(*out);

            if(!native_format_opt)
                out = std::nullopt;
            else
            {
                GLenum native_format = native_format_opt.value();

                GLint supported = GL_FALSE;
                //do not check extension formats -< we just init is with GL_FALSE so on error we still get false
                loader.GetInternalformativ(inner_type,
                                           native_format,
                                           GL_INTERNALFORMAT_SUPPORTED,
                                           1,
                                           &supported);
                if(supported == GL_FALSE)
                    out = std::nullopt;
            }
        }*/

        return out;

        /*
    R4G4_UNORM_PACK8 -> G(0-3)0, R(4-7)4
    R4G4B4A4_UNORM_PACK16 -> A(0-3)0, B(4-7)4, G(8-11)8, R(12-15)12
    B4G4R4A4_UNORM_PACK16
    R5G6B5_UNORM_PACK16
    B5G6R5_UNORM_PACK16
    R5G5B5A1_UNORM_PACK16
    B5G5R5A1_UNORM_PACK16
    A1R5G5B5_UNORM_PACK16
    R8_UNORM
    R8_UNORM_SRGB
    R8G8_UNORM
    R8G8_UNORM_SRGB
    R8G8B8_UNORM
    R8G8B8_UNORM_SRGB
    B8G8R8_UNORM
    B8G8R8_UNORM_SRGB
    R8G8B8A8_UNORM
    R8G8B8A8_UNORM_SRGB
    B8G8R8A8_UNORM
    B8G8R8A8_UNORM_SRGB
    A8B8G8R8_UNORM_PACK32
    A8B8G8R8_UNORM_SRGB_PACK32
    A2R10G10B10_UNORM_PACK32
    A2B10G10R10_UNORM_PACK32
    R16_UNORM
    R16_SFLOAT
    R16G16_UNORM
    R16G16_SFLOAT
    R16G16B16_UNORM,
    R16G16B16_SFLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_SFLOAT,
    R32_SFLOAT
    R32G32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32B32A32_SFLOAT,
    A4R4G4B4_UNORM_PACK16
    A4B4G4R4_UNORM_PACK16
    A1B5G5R5_UNORM_PACK16
    A8_UNORM*/
    }

    template<typename T>
    static auto get_property(const GladGLContext& loader, GLenum name) noexcept
    {
        if constexpr(std::same_as<std::uint32_t, T> || std::same_as<std::int32_t, T>)
        {
            GLint value;
            loader.GetIntegerv(name, &value);

            return static_cast<T>(value);
        }
        else if constexpr(std::same_as<std::uint64_t, T> || std::same_as<std::int64_t, T>)
        {
            GLint64 value;
            loader.GetInteger64v(name, &value);

            return static_cast<T>(value);
        }
        else if constexpr(std::same_as<float, T>)
        {
            float value;
            loader.GetFloatv(name, &value);

            return value;
        }
        else if constexpr(std::same_as<Render::ComputeGroupSize, T>)
        {
            GLint values[3];
            loader.GetIntegeri_v(name, 0, values + 0);
            loader.GetIntegeri_v(name, 1, values + 1);
            loader.GetIntegeri_v(name, 2, values + 2);

            return Render::ComputeGroupSize{.x = static_cast<std::uint32_t>(values[0]),
                                            .y = static_cast<std::uint32_t>(values[1]),
                                            .z = static_cast<std::uint32_t>(values[2])};
        }
        else if constexpr(std::same_as<Render::Extent2D, T>)
        {
            GLfloat values[2];
            loader.GetFloatv(name, values);

            return Render::Extent2D{.width = static_cast<std::uint32_t>(values[0]),
                                    .height = static_cast<std::uint32_t>(values[1])};
        }
        else if constexpr(std::same_as<Render::Range, T>)
        {
            GLfloat values[2];
            loader.GetFloatv(name, values);

            return Render::Range{.min = values[0], .max = values[1]};
        }
        else if constexpr(std::same_as<Render::SampleCount, T>)
        {
            GLint value;
            loader.GetIntegerv(name, &value);

            static_assert(/*std::is_signed_v<Render::SampleCountFlags> &&*/
                          sizeof(Render::SampleCountFlags) == 4);

            std::uint32_t u_value = static_cast<std::uint32_t>(value);
            if(!Core::IsPowerOf2(u_value))
                value = std::bit_floor(u_value);

            Render::SampleCountFlags samples = u_value;
            if(samples == 1)
                return samples;

            return samples | (samples - 1);
        }
        else
            assert(false);
    }

    template<typename T, typename... I>
    static T get_property_min(const GladGLContext& loader, I... names) noexcept
    {
        if constexpr(std::same_as<std::uint32_t, T> || std::same_as<std::int32_t, T>)
        {
            return std::min({(get_property<T>(loader, names), ...)});
        }
        else
            assert(false);
    }

    Render::PhysicalDeviceProperties GetPhysicalDeviceProperties(const GladGLContext& loader,
                                                                 bool robust_buffer_access)
    {
        GLint extensions_number = 0;
        loader.GetIntegerv(GL_NUM_EXTENSIONS, &extensions_number);

        std::vector<std::string> extensions;

        extensions.resize(extensions_number);
        for(std::size_t i = 0; i < extensions_number; i++)
            extensions[i] = reinterpret_cast<const char*>(loader.GetStringi(GL_EXTENSIONS, i));

        GLint major = 0;
        GLint minor = 0;
        loader.GetIntegerv(GL_MAJOR_VERSION, &major);
        loader.GetIntegerv(GL_MINOR_VERSION, &minor);

        //properties:
        std::uint32_t max_uniform_size =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_UNIFORM_LOCATIONS,
                                            GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
                                            GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
                                            GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
                                            GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS,
                                            GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS,
                                            GL_MAX_VERTEX_UNIFORM_COMPONENTS) *
            4;

        std::uint32_t max_per_stage_descriptor_textures =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS,
                                            GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);

        std::uint32_t max_per_stage_descriptor_uniform_buffers =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_COMBINED_UNIFORM_BLOCKS,
                                            GL_MAX_COMPUTE_UNIFORM_BLOCKS,
                                            GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
                                            GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
                                            GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
                                            GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,
                                            GL_MAX_VERTEX_UNIFORM_BLOCKS);

        std::uint32_t max_per_stage_descriptor_storage_buffers =
            get_property_min<std::uint32_t>(loader,
                                            GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
                                            GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);

        std::uint32_t max_per_stage_descriptor_storage_images = get_property_min<std::uint32_t>(
            loader,
            max_per_stage_descriptor_textures, //because texel buffers in VK are storage images but in OGL they are textures...
            GL_MAX_COMBINED_IMAGE_UNIFORMS,
            GL_MAX_COMPUTE_IMAGE_UNIFORMS,
            GL_MAX_FRAGMENT_IMAGE_UNIFORMS,
            GL_MAX_GEOMETRY_IMAGE_UNIFORMS,
            GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS,
            GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS,
            GL_MAX_VERTEX_IMAGE_UNIFORMS);

        Render::PhysicalDeviceLimits limits = Render::PhysicalDeviceLimits{
            .max_image_dimension_1D = get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
            .max_image_dimension_2D = get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_SIZE),
            .max_image_dimension_3D = get_property<std::uint32_t>(loader, GL_MAX_3D_TEXTURE_SIZE),
            .max_image_dimension_cube =
                get_property<std::uint32_t>(loader, GL_MAX_CUBE_MAP_TEXTURE_SIZE),
            .max_image_array_layers =
                get_property<std::uint32_t>(loader, GL_MAX_ARRAY_TEXTURE_LAYERS),
            .max_texel_buffer_elements =
                get_property<std::uint32_t>(loader, GL_MAX_TEXTURE_BUFFER_SIZE),
            .max_uniform_buffer_range =
                get_property<std::uint32_t>(loader, GL_MAX_UNIFORM_BLOCK_SIZE),
            .max_storage_buffer_range =
                get_property<std::uint64_t>(loader, GL_MAX_SHADER_STORAGE_BLOCK_SIZE),
            .max_uniform_size = max_uniform_size,
            .max_sampler_allocation_count = std::numeric_limits<std::uint32_t>::max(),
            .max_bound_descriptor_sets = std::numeric_limits<std::uint32_t>::max(),
            .max_per_stage_descriptor_samplers = max_per_stage_descriptor_textures,
            .max_per_stage_descriptor_uniform_buffers = max_per_stage_descriptor_uniform_buffers,
            .max_per_stage_descriptor_storage_buffers = max_per_stage_descriptor_storage_buffers,
            .max_per_stage_descriptor_sampled_images = max_per_stage_descriptor_textures,
            .max_per_stage_descriptor_storage_images = max_per_stage_descriptor_storage_images,
            .max_per_stage_descriptor_input_attachments = max_per_stage_descriptor_textures,
            .max_per_stage_resources =
                max_per_stage_descriptor_textures + max_per_stage_descriptor_uniform_buffers +
                max_per_stage_descriptor_storage_buffers + max_per_stage_descriptor_storage_images,
            .max_descriptor_set_samplers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_uniform_buffers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_storage_buffers = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_sampled_images = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_storage_images = std::numeric_limits<std::uint32_t>::max(),
            .max_descriptor_set_input_attachments = std::numeric_limits<std::uint32_t>::max(),
            .max_vertex_input_attributes =
                get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIBS),
            .max_vertex_input_bindings =
                get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_BINDINGS),
            .max_vertex_input_attribute_offset =
                get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET),
            .max_vertex_input_binding_stride =
                get_property<std::uint32_t>(loader, GL_MAX_VERTEX_ATTRIB_STRIDE),
            .max_vertex_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_VERTEX_OUTPUT_COMPONENTS),
            .max_tessellation_generation_level =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_GEN_LEVEL),
            .max_tessellation_patch_size =
                get_property<std::uint32_t>(loader, GL_MAX_PATCH_VERTICES),
            .max_tessellation_control_per_vertex_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_INPUT_COMPONENTS),
            .max_tessellation_control_per_vertex_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS),
            .max_tessellation_control_per_patch_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_PATCH_COMPONENTS),
            .max_tessellation_control_total_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS),
            .max_tessellation_evaluation_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS),
            .max_tessellation_evaluation_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS),
            .max_geometry_shader_invocations =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_SHADER_INVOCATIONS),
            .max_geometry_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_INPUT_COMPONENTS),
            .max_geometry_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS),
            .max_geometry_output_vertices =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_OUTPUT_VERTICES),
            .max_geometry_total_output_components =
                get_property<std::uint32_t>(loader, GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS),
            .max_fragment_input_components =
                get_property<std::uint32_t>(loader, GL_MAX_FRAGMENT_INPUT_COMPONENTS),
            .max_fragment_output_attachments =
                get_property<std::uint32_t>(loader, GL_MAX_DRAW_BUFFERS),
            .max_fragment_dual_src_attachments =
                get_property<std::uint32_t>(loader, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS),
            .max_fragment_combined_output_resources =
                get_property<std::uint32_t>(loader, GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES),
            .max_compute_shared_memory_size =
                get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_SHARED_MEMORY_SIZE),
            .max_compute_work_group_count =
                get_property<Render::ComputeGroupSize>(loader, GL_MAX_COMPUTE_WORK_GROUP_COUNT),
            .max_compute_work_group_invocations =
                get_property<std::uint32_t>(loader, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS),
            .max_compute_work_group_size =
                get_property<Render::ComputeGroupSize>(loader, GL_MAX_COMPUTE_WORK_GROUP_SIZE),
            .sub_pixel_precision_bits = get_property<std::uint32_t>(loader, GL_SUBPIXEL_BITS),
            .max_draw_indexed_index_value =
                std::numeric_limits<std::uint32_t>::max() - 1 /*GL_MAX_ELEMENT_INDEX*/,
            .max_draw_indirect_count = std::numeric_limits<std::uint32_t>::max(),
            .max_sampler_lod_bias = get_property<float>(loader, GL_MAX_TEXTURE_LOD_BIAS),
            .max_sampler_anisotropy =
                ((loader.VERSION_4_6 || loader.ARB_texture_filter_anisotropic) ?
                     get_property<float>(loader, GL_MAX_TEXTURE_MAX_ANISOTROPY) :
                     1.0f),
            .max_viewports = get_property<std::uint32_t>(loader, GL_MAX_VIEWPORTS),
            .max_viewport_dimensions = get_property<Render::Extent2D>(loader, GL_MAX_VIEWPORT_DIMS),
            .viewport_bounds_range = get_property<Render::Range>(loader, GL_VIEWPORT_BOUNDS_RANGE),
            .viewport_sub_pixel_bits =
                get_property<std::uint32_t>(loader, GL_VIEWPORT_SUBPIXEL_BITS),
            .min_memory_map_alignment =
                get_property<std::uint32_t>(loader, GL_MIN_MAP_BUFFER_ALIGNMENT),
            .min_texel_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT),
            .min_uniform_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT),
            .min_storage_buffer_offset_alignment =
                get_property<std::uint32_t>(loader, GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT),
            .min_texel_offset = get_property<std::int32_t>(loader, GL_MIN_PROGRAM_TEXEL_OFFSET),
            .max_texel_offset = get_property<std::uint32_t>(loader, GL_MAX_PROGRAM_TEXEL_OFFSET),
            .min_texel_gather_offset =
                get_property<std::int32_t>(loader, GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET),
            .max_texel_gather_offset =
                get_property<std::uint32_t>(loader, GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET),
            .min_interpolation_offset =
                get_property<float>(loader, GL_MIN_FRAGMENT_INTERPOLATION_OFFSET),
            .max_interpolation_offset =
                get_property<float>(loader, GL_MAX_FRAGMENT_INTERPOLATION_OFFSET),
            .sub_pixel_interpolation_offset_bits =
                get_property<std::uint32_t>(loader, GL_FRAGMENT_INTERPOLATION_OFFSET_BITS),
            .max_framebuffer_width = get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_WIDTH),
            .max_framebuffer_height =
                get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_HEIGHT),
            .max_framebuffer_layers =
                get_property<std::uint32_t>(loader, GL_MAX_FRAMEBUFFER_LAYERS),
            .framebuffer_color_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
            .framebuffer_depth_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
            .framebuffer_stencil_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
            .framebuffer_no_attachments_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_FRAMEBUFFER_SAMPLES),
            .max_color_attachments = get_property<std::uint32_t>(loader, GL_MAX_COLOR_ATTACHMENTS),
            .sampled_image_color_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_COLOR_TEXTURE_SAMPLES),
            .sampled_image_integer_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_INTEGER_SAMPLES),
            .sampled_image_depth_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_DEPTH_TEXTURE_SAMPLES),
            .sampled_image_stencil_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_DEPTH_TEXTURE_SAMPLES),
            .storage_image_sample_counts =
                get_property<Render::SampleCount>(loader, GL_MAX_IMAGE_SAMPLES),
            .max_sample_mask_words = get_property<std::uint32_t>(loader, GL_MAX_SAMPLE_MASK_WORDS),
            .max_clip_distances = get_property<std::uint32_t>(loader, GL_MAX_CLIP_DISTANCES),
            .max_cull_distances = get_property<std::uint32_t>(loader, GL_MAX_CULL_DISTANCES),
            .max_combined_clip_and_cull_distances =
                get_property<std::uint32_t>(loader, GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES),
            .discrete_queue_priorities = std::numeric_limits<std::uint32_t>::max(),
            .point_size_range = get_property<Render::Range>(loader, GL_POINT_SIZE_RANGE),
            .line_width_range = get_property<Render::Range>(loader, GL_SMOOTH_LINE_WIDTH_RANGE),
            .point_size_granularity = get_property<float>(loader, GL_POINT_SIZE_GRANULARITY),
            .line_width_granularity = get_property<float>(loader, GL_SMOOTH_LINE_WIDTH_GRANULARITY),
            .optimal_buffer_copy_offset_alignment = 1,
            .optimal_buffer_copy_row_pitch_alignment = 1,
            .non_coherent_atom_size = 1,
            .max_custom_border_color_samplers = std::numeric_limits<std::uint32_t>::max()};

        Render::PhysicalDeviceFeatures features = Render::PhysicalDeviceFeatures{
            .robust_buffer_access = robust_buffer_access,
            .full_draw_index_uint32 = true,
            .image_cube_array = true,
            .independent_blend = true,
            .geometry_shader = true,
            .tessellation_shader = true,
            .sample_rate_shading = true,
            .dual_src_blend = true,
            .logic_op = true,
            .multi_draw_indirect = true,
            .draw_indirect_first_instance = true,
            .depth_clamp = true,
            .depth_bias_clamp = (loader.VERSION_4_6 || loader.ARB_polygon_offset_clamp),
            .fill_mode_non_solid = true,
            .depth_bounds = static_cast<bool>(loader.EXT_depth_bounds_test),
            .wide_lines = !(limits.line_width_range.min == limits.line_width_range.max &&
                            limits.line_width_range.min == 1.0f),
            .large_points = !(limits.point_size_range.min == limits.point_size_range.max &&
                              limits.point_size_range.min == 1.0f),
            .alpha_to_one = true,
            .multi_viewport = (limits.max_viewports > 1),
            .sampler_anisotropy =
                static_cast<bool>(loader.VERSION_4_6 || loader.ARB_texture_filter_anisotropic),
            .vertex_pipeline_stores_and_atomics = true,
            .fragment_stores_and_atomics = true,
            .shader_tessellation_and_geometry_point_size = true,
            .shader_image_gather_extended = true,
            .shader_storage_image_multisample = true,
            .shader_storage_image_read_without_format =
                static_cast<bool>(loader.EXT_shader_image_load_formatted),
            .shader_storage_image_write_without_format = true,
            .shader_uniform_buffer_array_dynamic_indexing = true,
            .shader_sampled_image_array_dynamic_indexing = true,
            .shader_storage_buffer_array_dynamic_indexing = true,
            .shader_storage_image_array_dynamic_indexing = true,
            .shader_clip_distance = true,
            .shader_cull_distance = true,
            .shader_float64 = true,
            .shader_int64 = static_cast<bool>(loader.ARB_gpu_shader_int64),
            .shader_int16 = static_cast<bool>(loader.AMD_gpu_shader_int16),
            .shader_resource_min_lod = true,
            .variable_multisample_rate = true,
            .sampler_mirror_clamp_to_edge = true,
            .custom_border_colors = true,
            .custom_border_color_without_format = true,
            .index_type_uint8 = true};

        return Render::PhysicalDeviceProperties{
            .version = Render::MakeVersion(major, minor),
            .vendor_name = reinterpret_cast<const char*>(loader.GetString(GL_VENDOR)),
            .device_name = reinterpret_cast<const char*>(loader.GetString(GL_RENDERER)),
            .extensions = std::move(extensions),
            .supported_syntax = Render::ShaderSyntaxFlagBits::GLSL,
            .queue_family_properties = {Render::QueueFamilyProperties{
                .specialization = Render::QueueSpecializationFlagBits::TransferSpec |
                                  Render::QueueSpecializationFlagBits::ComputeSpec |
                                  Render::QueueSpecializationFlagBits::GraphicsSpec,
                .queue_count = 1,
                .min_image_transfer_granularity =
                    Render::Extent3D{.width = 1, .height = 1, .depth = 1}}},
            .memory_types =
                std::vector(std::begin(AVAILABLE_MEMORY_TYPES), std::end(AVAILABLE_MEMORY_TYPES)),
            .command_buffer_strategy = Render::CommandBufferStrategy::Immediate,
            .device_type = Render::PhysicalDeviceType::Other,
            .view_origin = Render::ViewOrigin::BottomLeft,
            .clip_space_depth_bounds =
                Render::ClipSpaceDepthBounds{.min = 0.0f,
                                             .max = 1.0f}, //due to glClipControl(GL_ZERO_TO_ONE)
            .limits = limits,
            .features = features};
    }

    std::optional<Render::BufferFormatProperties>
    GetPhysicalDeviceBufferFormatProperties(const GladGLContext& loader,
                                            const Render::BufferFormatInfo& info) noexcept
    {
        Render::BufferFormatProperties props = {.features = {}};

        auto opt = DecodeVertexInputTypeSizePair(info.format);

        if(opt)
            props.features = Render::FormatFeatureFlagBits::FormatFeatureVertexBufferBit;

        Render::FormatFeatureFlags image_format_features = {};
        switch(info.format)
        {
            case Render::Format::R8_UNORM:
            case Render::Format::R16_UNORM:
            case Render::Format::R16_SFLOAT:
            case Render::Format::R32_SFLOAT:
            case Render::Format::R8_SINT:
            case Render::Format::R16_SINT:
            case Render::Format::R32_SINT:
            case Render::Format::R8_UINT:
            case Render::Format::R16_UINT:
            case Render::Format::R32_UINT:
            case Render::Format::R8G8_UNORM:
            case Render::Format::R16G16_UNORM:
            case Render::Format::R16G16_SFLOAT:
            case Render::Format::R32G32_SFLOAT:
            case Render::Format::R8G8_SINT:
            case Render::Format::R16G16_SINT:
            case Render::Format::R32G32_SINT:
            case Render::Format::R8G8_UINT:
            case Render::Format::R16G16_UINT:
            case Render::Format::R32G32_UINT:
            case Render::Format::R32G32B32_SFLOAT:
            case Render::Format::R32G32B32_SINT:
            case Render::Format::R32G32B32_UINT:
            case Render::Format::R8G8B8A8_UNORM:
            case Render::Format::R16G16B16A16_UNORM:
            case Render::Format::R16G16B16A16_SFLOAT:
            case Render::Format::R32G32B32A32_SFLOAT:
            case Render::Format::R8G8B8A8_SINT:
            case Render::Format::R16G16B16A16_SINT:
            case Render::Format::R32G32B32A32_SINT:
            case Render::Format::R8G8B8A8_UINT:
            case Render::Format::R16G16B16A16_UINT:
            case Render::Format::R32G32B32A32_UINT:
                image_format_features =
                    Render::FormatFeatureFlagBits::FormatFeatureUniformTexelBufferBit |
                    Render::FormatFeatureFlagBits::FormatFeatureStorageTexelBufferBit;

                if(info.format == Render::Format::R32_SINT ||
                   info.format == Render::Format::R32_UINT)
                    image_format_features |=
                        Render::FormatFeatureFlagBits::FormatFeatureStorageTexelBufferAtomicBit;
                break;
            default:
                image_format_features = {};
                break;
        }

        if(image_format_features != 0)
        {
            auto native_format_opt = FormatToNative(info.format);
            if(native_format_opt)
            {
                GLint format_supported = GL_FALSE;
                loader.GetInternalformativ(GL_TEXTURE_BUFFER,
                                           native_format_opt.value(),
                                           GL_INTERNALFORMAT_SUPPORTED,
                                           1,
                                           &format_supported);
                if(format_supported != GL_FALSE)
                    props.features |= image_format_features;
            }
        }

        if(props.features != 0)
            return props;

        return std::nullopt;
    }

    std::optional<Render::ImageFormatProperties>
    GetPhysicalDeviceImageFormatProperties(const GladGLContext& loader,
                                           const Render::ImageFormatInfo& info) noexcept
    {
        GLenum inner_type =
            DecodeImageType(info.type,
                            info.layered,
                            info.sampled,
                            info.flags & Render::ImageFlagBits::ImageCubeCompatible);
        auto native_format_opt = FormatToNative(info.format);

        if(!native_format_opt)
            return std::nullopt;

        GLenum native_format = native_format_opt.value();

        GLint supported = GL_FALSE;
        //do not check extension formats -> we just init is with GL_FALSE so on error we still get false
        loader.GetInternalformativ(inner_type,
                                   native_format,
                                   GL_INTERNALFORMAT_SUPPORTED,
                                   1,
                                   &supported);
        if(supported == GL_FALSE)
            return std::nullopt;

        GLint width;
        GLint height;
        GLint depth;
        loader.GetInternalformativ(inner_type, native_format, GL_MAX_WIDTH, 1, &width);

        if(info.type == Render::ImageType::Image1D)
        {
            height = 1;
            width = 1;
        }
        else
        {
            loader.GetInternalformativ(inner_type, native_format, GL_MAX_HEIGHT, 1, &height);
            if(info.type == Render::ImageType::Image2D)
                depth = 1;
            else
                loader.GetInternalformativ(inner_type, native_format, GL_MAX_DEPTH, 1, &depth);
        }

        GLint support_mipmaps;
        loader.GetInternalformativ(inner_type, native_format, GL_MIPMAP, 1, &support_mipmaps);

        GLint max_mip_levels;
        if(!support_mipmaps)
            max_mip_levels = 1;
        else
            max_mip_levels =
                Render::LOD_CLAMP_NONE; //in OpenGL spec it is a default value -> so do not care...

        GLint max_array_layers;
        if(info.type == Render::ImageType::Image3D && !info.layered)
            max_array_layers = 1;
        else
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_MAX_LAYERS,
                                       1,
                                       &max_array_layers);

        static std::vector<GLint> samples(std::popcount(
            (static_cast<std::uint64_t>(Render::SampleCount::SampleCount_64) << 1) - 1));

        Render::SampleCountFlags sample_count = {};
        if(!info.sampled)
            sample_count = Render::SampleCount::SampleCount_1;
        else
        {
            GLint num_samples;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_NUM_SAMPLE_COUNTS,
                                       1,
                                       &num_samples);
            if(num_samples > samples.size())
                samples.resize(num_samples);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_SAMPLES,
                                       num_samples,
                                       samples.data());

            for(std::size_t i = 0; i < num_samples; i++)
            {
                if(/*samples[i] <= Render::SampleCount::SampleCount_64 &&*/
                   std::popcount(static_cast<std::uint32_t>(samples[i])) == 1) //is popwer of two
                {
                    sample_count |= samples[i];
                }
            }
        }

        Render::FormatFeatureFlags features = {};

        //FormatFeatureSampledImageBit; -> all???
        //FormatFeatureStorageImageBit; -> set
        //FormatFeatureStorageImageAtomicBit; -> set
        //FormatFeatureVertexBufferBit; -> none
        //FormatFeatureColorAttachmentBit; -> query
        //FormatFeatureColorAttachmentBlendBit; -> query
        //FormatFeatureDepthStencilAttachmentBit; -> query
        //FormatFeaturesampledImageFilterLinearBit; -> query
        //FormatFeatureTransferSrcBit; -> decode
        //FormatFeatureTransferDstBit; -> decode

        GLint texture_view_support;
        loader.GetInternalformativ(inner_type,
                                   native_format,
                                   GL_TEXTURE_VIEW,
                                   1,
                                   &texture_view_support);
        if(texture_view_support != GL_NONE)
        {
            features |= Render::FormatFeatureSampledImageBit;

            switch(info.format)
            {
                case Render::Format::R32G32B32A32_SFLOAT:
                case Render::Format::R16G16B16A16_SFLOAT:
                case Render::Format::R32G32_SFLOAT:
                case Render::Format::R16G16_SFLOAT:
                case Render::Format::B10G11R11_UFLOAT_PACK32:
                case Render::Format::R32_SFLOAT:
                case Render::Format::R16_SFLOAT:

                case Render::Format::R32G32B32A32_UINT:
                case Render::Format::R16G16B16A16_UINT:
                case Render::Format::A2B10G10R10_UINT_PACK32:
                case Render::Format::R8G8B8A8_UINT:
                case Render::Format::R32G32_UINT:
                case Render::Format::R16G16_UINT:
                case Render::Format::R8G8_UINT:
                case Render::Format::R32_UINT:
                case Render::Format::R16_UINT:
                case Render::Format::R8_UINT:

                case Render::Format::R32G32B32A32_SINT:
                case Render::Format::R16G16B16A16_SINT:
                case Render::Format::R8G8B8A8_SINT:
                case Render::Format::R32G32_SINT:
                case Render::Format::R16G16_SINT:
                case Render::Format::R8G8_SINT:
                case Render::Format::R32_SINT:
                case Render::Format::R16_SINT:
                case Render::Format::R8_SINT:

                case Render::Format::R16G16B16A16_UNORM:
                case Render::Format::A2B10G10R10_UNORM_PACK32:
                case Render::Format::R8G8B8A8_UNORM:
                case Render::Format::R16G16_UNORM:
                case Render::Format::R8G8_UNORM:
                case Render::Format::R16_UNORM:
                case Render::Format::R8_UNORM:

                case Render::Format::R16G16B16A16_SNORM:
                case Render::Format::R8G8B8A8_SNORM:
                case Render::Format::R16G16_SNORM:
                case Render::Format::R8G8_SNORM:
                case Render::Format::R16_SNORM:
                case Render::Format::R8_SNORM:
                    features |= Render::FormatFeatureStorageImageBit;
                    break;
                default:
                    break;
            }

            if(info.format == Render::Format::R32_SINT || info.format == Render::Format::R32_UINT)
                features |= Render::FormatFeatureStorageImageAtomicBit;

            GLint color_renderable;
            GLint depth_renderable;
            GLint stencil_renderable;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_COLOR_RENDERABLE,
                                       1,
                                       &color_renderable);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_DEPTH_RENDERABLE,
                                       1,
                                       &depth_renderable);

            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_STENCIL_RENDERABLE,
                                       1,
                                       &stencil_renderable);

            if(color_renderable == GL_TRUE)
                features |= Render::FormatFeatureColorAttachmentBit;

            if(depth_renderable == GL_TRUE || stencil_renderable == GL_TRUE)
                features |= Render::FormatFeatureDepthStencilAttachmentBit;

            GLint color_blend;
            loader.GetInternalformativ(inner_type,
                                       native_format,
                                       GL_FRAMEBUFFER_BLEND,
                                       1,
                                       &color_blend);
            if(color_blend == GL_TRUE)
                features |= Render::FormatFeatureColorAttachmentBlendBit;

            GLint linear_filter;
            loader.GetInternalformativ(inner_type, native_format, GL_FILTER, 1, &linear_filter);
            if(linear_filter == GL_TRUE)
                features |= Render::FormatFeatureSampledImageFilterLinearBit;

            if(DecodeTransferTypeFormatPair(info.format).has_value())
                features |=
                    Render::FormatFeatureTransferSrcBit | Render::FormatFeatureTransferDstBit;
        }

        return Render::ImageFormatProperties{
            .max_extent = {.width = static_cast<std::uint32_t>(width),
                           .height = static_cast<std::uint32_t>(height),
                           .depth = static_cast<std::uint32_t>(depth)},
            .max_mip_levels = static_cast<std::uint32_t>(max_mip_levels),
            .max_array_layers = static_cast<std::uint32_t>(max_array_layers),
            .sample_count = sample_count,
            .features = features};
    }
}