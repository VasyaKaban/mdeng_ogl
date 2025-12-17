#include "Render.h"
#include <stdexcept>

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

    GLbitfield DecodeMemoryTypePropertyFlagsToNative(Render::MemoryTypePropertyFlags flags)
    {
        constexpr static std::pair<Render::MemoryTypePropertyFlagBits, GLbitfield> mapping[] = {
            {Render::MemoryTypePropertyFlagBits::DeviceLocal, 0},
            {Render::MemoryTypePropertyFlagBits::HostMappingReadable,
             GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostMappingWritable,
             GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostCoherent, GL_MAP_COHERENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostCached, GL_CLIENT_STORAGE_BIT}};

        GLbitfield mask = 0;
        for(const auto& pr: mapping)
        {
            if(flags & pr.first)
                mask |= pr.second;
        }

        return mask;
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

    GLenum ImageViewTypeToNative(Render::ImageViewType type)
    {
        GLenum value;
        switch(type)
        {
            case Render::ImageViewType::ImageView1D:
                value = GL_TEXTURE_1D;
                break;
            case Render::ImageViewType::ImageView2D:
                value = GL_TEXTURE_2D;
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
                value = GL_TEXTURE_2D_ARRAY;
                break;
            case Render::ImageViewType::ImageViewCubeMapArray:
                value = GL_TEXTURE_CUBE_MAP_ARRAY;
                break;
            case Render::ImageViewType::ImageView2DMultisample:
                value = GL_TEXTURE_2D_MULTISAMPLE;
                break;
            case Render::ImageViewType::ImageView2DMultisampleArray:
                value = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
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
            case Render::PrimitiveTopology::Points:
                value = GL_POINTS;
                break;
            case Render::PrimitiveTopology::Lines:
                value = GL_LINES;
                break;
            case Render::PrimitiveTopology::LineStrip:
                value = GL_LINE_STRIP;
                break;
            case Render::PrimitiveTopology::Triangles:
                value = GL_TRIANGLES;
                break;
            case Render::PrimitiveTopology::TriangleStrip:
                value = GL_TRIANGLE_STRIP;
                break;
            case Render::PrimitiveTopology::TriangleFan:
                value = GL_TRIANGLE_FAN;
                break;
            case Render::PrimitiveTopology::LinesAdjacency:
                value = GL_LINES_ADJACENCY;
                break;
            case Render::PrimitiveTopology::LineStripAdjacency:
                value = GL_LINE_STRIP_ADJACENCY;
                break;
            case Render::PrimitiveTopology::TrianglesAdjacency:
                value = GL_TRIANGLES_ADJACENCY;
                break;
            case Render::PrimitiveTopology::TriangleStrIpAdjacency:
                value = GL_TRIANGLE_STRIP_ADJACENCY;
                break;
            case Render::PrimitiveTopology::Patches:
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
            case Render::BlendLogicOp::Noop:
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
            case Render::StencilOp::Increment:
                value = GL_INCR;
                break;
            case Render::StencilOp::IncrementWrap:
                value = GL_INCR_WRAP;
                break;
            case Render::StencilOp::Decrement:
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

    GLenum DecodeImageType(Render::ImageType type, bool layered, bool sampled)
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
                        _inner_type = GL_TEXTURE_2D;
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE;
                }
                else
                {
                    if(!sampled)
                        _inner_type = GL_TEXTURE_2D_ARRAY;
                    else
                        _inner_type = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                }
            }
            break;
            case Render::ImageType::Image3D:
            {
                _inner_type = GL_TEXTURE_3D;
            }
            break;
            case Render::ImageType::CubeMap:
                if(!layered)
                    _inner_type = GL_TEXTURE_CUBE_MAP;
                else
                    _inner_type = GL_TEXTURE_CUBE_MAP_ARRAY;
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
            {Render::AccessFlagBits::AccessTransferWiteBit,
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
            {Render::AccessFlagBits::AccessTransferWiteBit, GL_TEXTURE_UPDATE_BARRIER_BIT},
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
}