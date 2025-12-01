#include "Render.h"
#include <stdexcept>
#include "hrs/mapping.hpp"

namespace OpenGL
{
#define CHECK_MAPPING_IS_SORTED(NAME) \
    static_assert(std::ranges::is_sorted(NAME, \
                                         [](const auto& pr1, const auto& pr2) \
                                         { \
                                             return pr1.first < pr2.first; \
                                         }));

    GLenum ComapreOpToNative(Render::CompareOp op)
    {
        constexpr static std::pair<Render::CompareOp, GLenum> mapping[] = {
            {Render::CompareOp::Never, GL_NEVER},
            {Render::CompareOp::Less, GL_LESS},
            {Render::CompareOp::Equal, GL_EQUAL},
            {Render::CompareOp::LessOrEqual, GL_LEQUAL},
            {Render::CompareOp::Greater, GL_GREATER},
            {Render::CompareOp::NotEqual, GL_NOTEQUAL},
            {Render::CompareOp::GreaterOrEqual, GL_GEQUAL},
            {Render::CompareOp::Always, GL_ALWAYS},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, op);
        if(native == nullptr)
            throw std::runtime_error("No native CompareOp found");

        return *native;
    }

    GLenum SampleCountToNative(Render::SampleCount samples)
    {
        constexpr static std::pair<Render::SampleCount, GLenum> mapping[] = {
            {Render::SampleCount::SampleCount_1, 1},
            {Render::SampleCount::SampleCount_2, 2},
            {Render::SampleCount::SampleCount_4, 4},
            {Render::SampleCount::SampleCount_8, 8},
            {Render::SampleCount::SampleCount_16, 16},
            {Render::SampleCount::SampleCount_32, 32},
            {Render::SampleCount::SampleCount_64, 64},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, samples);
        if(native == nullptr)
            throw std::runtime_error("No native SampleCount found");

        return *native;
    }

    GLbitfield DecodeMemoryTypePropertyFlagsToNative(Render::MemoryTypePropertyFlags flags)
    {
        constexpr static std::pair<Render::MemoryTypePropertyFlagBits, GLbitfield> map_mapping[] = {
            {Render::MemoryTypePropertyFlagBits::DeviceLocal, 0},
            {Render::MemoryTypePropertyFlagBits::HostMappingReadable,
             GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostMappingWritable,
             GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostCoherent, GL_MAP_COHERENT_BIT},
            {Render::MemoryTypePropertyFlagBits::HostCached, GL_CLIENT_STORAGE_BIT}};

        GLbitfield mask = 0;
        for(const auto& pr: map_mapping)
        {
            if(flags & pr.first)
                mask |= pr.second;
        }

        return mask;
    }

    GLenum FenceStatusToNative(Render::FenceStatus status)
    {
        constexpr static std::pair<Render::FenceStatus, GLenum> mapping[] = {
            {Render::FenceStatus::Signaled, GL_SIGNALED},
            {Render::FenceStatus::Unsignaled, GL_UNSIGNALED},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, status);
        if(native == nullptr)
            throw std::runtime_error("No native FenceStatus found");

        return *native;
    }

    GLenum FormatToNative(Render::Format format)
    {
        constexpr static std::pair<Render::Format, GLenum> mapping[] = {
            {Render::Format::R32G32B32A32_FLOAT, GL_RGBA32F},
            {Render::Format::R32G32B32A32_UINT, GL_RGBA32UI},
            {Render::Format::R32G32B32A32_SINT, GL_RGBA32I},

            {Render::Format::R32G32B32_FLOAT, GL_RGB32F},
            {Render::Format::R32G32B32_UINT, GL_RGB32UI},
            {Render::Format::R32G32B32_SINT, GL_RGB32I},

            {Render::Format::R16G16B16A16_FLOAT, GL_RGBA16F},
            {Render::Format::R16G16B16A16_UNORM, GL_RGBA16},
            {Render::Format::R16G16B16A16_UINT, GL_RGBA16UI},
            {Render::Format::R16G16B16A16_SNORM, GL_RGBA16_SNORM},
            {Render::Format::R16G16B16A16_SINT, GL_RGBA16I},

            {Render::Format::R32G32_FLOAT, GL_RG32F},
            {Render::Format::R32G32_UINT, GL_RG32UI},
            {Render::Format::R32G32_SINT, GL_RG32I},

            {Render::Format::D32_FLOAT_S8X24_UINT, GL_DEPTH32F_STENCIL8},

            {Render::Format::R10G10B10A2_UNORM, GL_RGB10_A2},
            {Render::Format::R10G10B10A2_UINT, GL_RGB10_A2UI},
            {Render::Format::R11G11B10_FLOAT, GL_R11F_G11F_B10F},

            {Render::Format::R8G8B8A8_UNORM, GL_RGBA8},
            {Render::Format::R8G8B8A8_UNORM_SRGB, GL_SRGB8_ALPHA8},
            {Render::Format::R8G8B8A8_UINT, GL_RGBA8UI},
            {Render::Format::R8G8B8A8_SNORM, GL_RGBA8_SNORM},
            {Render::Format::R8G8B8A8_SINT, GL_RGBA8I},

            {Render::Format::R16G16_FLOAT, GL_RG16F},
            {Render::Format::R16G16_UNORM, GL_RG16},
            {Render::Format::R16G16_UINT, GL_RG16UI},
            {Render::Format::R16G16_SNORM, GL_RG16_SNORM},
            {Render::Format::R16G16_SINT, GL_RG16I},

            {Render::Format::D32_FLOAT, GL_DEPTH_COMPONENT32F},
            {Render::Format::R32_FLOAT, GL_R32F},
            {Render::Format::R32_UINT, GL_R32UI},
            {Render::Format::R32_SINT, GL_R32I},

            {Render::Format::D24_UNORM_S8_UINT, GL_DEPTH24_STENCIL8},

            {Render::Format::R8G8_UNORM, GL_RG8},
            {Render::Format::R8G8_UINT, GL_RG8UI},
            {Render::Format::R8G8_SNORM, GL_RG8_SNORM},
            {Render::Format::R8G8_SINT, GL_RG8I},

            {Render::Format::R16_FLOAT, GL_R16F},
            {Render::Format::D16_UNORM, GL_DEPTH_COMPONENT16},
            {Render::Format::R16_UNORM, GL_R16},
            {Render::Format::R16_UINT, GL_R16UI},
            {Render::Format::R16_SNORM, GL_R16_SNORM},
            {Render::Format::R16_SINT, GL_R16I},

            {Render::Format::R8_UNORM, GL_R8},
            {Render::Format::R8_UINT, GL_R8UI},
            {Render::Format::R8_SNORM, GL_R8_SNORM},
            {Render::Format::R8_SINT, GL_R8I},

            {Render::Format::R9G9B9E5_SHAREDEXP, GL_RGB9_E5},

            {Render::Format::BC1_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
            {Render::Format::BC1_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT},
            {Render::Format::BC2_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
            {Render::Format::BC2_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT},

            {Render::Format::BC3_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},
            {Render::Format::BC3_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT},
            {Render::Format::BC4_UNORM, GL_COMPRESSED_RED_RGTC1},
            {Render::Format::BC4_SNORM, GL_COMPRESSED_SIGNED_RED_RGTC1},

            {Render::Format::BC5_UNORM, GL_COMPRESSED_RG_RGTC2},
            {Render::Format::BC5_SNORM, GL_COMPRESSED_SIGNED_RG_RGTC2},
            {Render::Format::B5G6R5_UNORM, GL_RGB565},
            {Render::Format::B5G5R5A1_UNORM, GL_RGB5_A1},

            {Render::Format::BC6H_UF16, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT},
            {Render::Format::BC6H_SF16, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT},
            {Render::Format::BC7_UNORM, GL_COMPRESSED_RGBA_BPTC_UNORM},
            {Render::Format::BC7_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM},

            {Render::Format::B4G4R4A4_UNORM, GL_RGBA4},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, format);
        if(native == nullptr)
            throw std::runtime_error("No native format found");

        return *native;

        //RGB
        //R3G3B2_Unorm = GL_R3_G3_B2,
        //RGB4_UNorm = GL_RGB4,
        //RGB5_Unorm = GL_RGB5,

        //RGB10_UNorm = GL_RGB10,
        //RGB12_UNorm = GL_RGB12,

        //RGB8_UNorm = GL_RGB8,
        //RGB8_SNorm = GL_RGB8_SNORM,
        //RGB8_Int = GL_RGB8I,
        //RGB8_UInt = GL_RGB8UI,

        //RGB16_UNorm = GL_RGB16,
        //RGB16_SNorm = GL_RGB16_SNORM,
        //RGB16_Int = GL_RGB16I,
        //RGB16_UInt = GL_RGB16UI,
        //RGB16_Float = GL_RGB16F,

        //SRGB8_UNorm = GL_SRGB8,

        //RGBA
        //RGBA2_UNorm = GL_RGBA2,

        //RGBA12_UNorm = GL_RGBA12,

        //Depth
        //D24 = GL_DEPTH_COMPONENT24,
        //D32 = GL_DEPTH_COMPONENT32,

        //StencilIndex
        //S1 = GL_STENCIL_INDEX1,
        //S4 = GL_STENCIL_INDEX4,
        //S8 = GL_STENCIL_INDEX8,
        //S16 = GL_STENCIL_INDEX16,

        //Compressed EAC/ETC2
        //R11_EAC_UNorm = GL_COMPRESSED_R11_EAC,
        //RG11_EAC_UNorm = GL_COMPRESSED_RG11_EAC,
        //RGBA8_ETC2_EAC_UNorm = GL_COMPRESSED_RGBA8_ETC2_EAC,
        //R11_EAC_SNorm = GL_COMPRESSED_SIGNED_R11_EAC,
        //RG11_EAC_SNorm = GL_COMPRESSED_SIGNED_RG11_EAC,
        //RGB8_ETC2 = GL_COMPRESSED_RGB8_ETC2,
        //RGB8_A1_ETC2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
        //SRGBA8_ETC2_EAC = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
        //SRGB8_ETC2 = GL_COMPRESSED_SRGB8_ETC2,
        //SRGB8_A1_ET2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
    }

    GLenum ImageViewTypeToNative(Render::ImageViewType type)
    {
        constexpr static std::pair<Render::ImageViewType, GLenum> mapping[] = {
            {Render::ImageViewType::ImageView1D, GL_TEXTURE_1D},
            {Render::ImageViewType::ImageView2D, GL_TEXTURE_2D},
            {Render::ImageViewType::ImageView3D, GL_TEXTURE_3D},
            {Render::ImageViewType::ImageViewCubeMap, GL_TEXTURE_CUBE_MAP},
            {Render::ImageViewType::ImageView1DArray, GL_TEXTURE_1D_ARRAY},
            {Render::ImageViewType::ImageView2DArray, GL_TEXTURE_2D_ARRAY},
            {Render::ImageViewType::ImageViewCubeMapArray, GL_TEXTURE_CUBE_MAP_ARRAY},
            {Render::ImageViewType::ImageView2DMultisample, GL_TEXTURE_2D_MULTISAMPLE},
            {Render::ImageViewType::ImageView2DMultisampleArray, GL_TEXTURE_2D_MULTISAMPLE_ARRAY},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, type);
        if(native == nullptr)
            throw std::runtime_error("No native ImageViewType found");

        return *native;
    }

    GLenum ComponentSwizzleToNative(Render::ComponentSwizzle swizzle)
    {
        constexpr static std::pair<Render::ComponentSwizzle, GLenum> mapping[] = {
            {Render::ComponentSwizzle::SwizzleRed, GL_RED},
            {Render::ComponentSwizzle::SwizzleGreen, GL_GREEN},
            {Render::ComponentSwizzle::SwizzleBlue, GL_BLUE},
            {Render::ComponentSwizzle::SwizzleAlpha, GL_ALPHA},
            {Render::ComponentSwizzle::SwizzleZero, GL_ZERO},
            {Render::ComponentSwizzle::SwizzleOne, GL_ONE},
            {Render::ComponentSwizzle::SwizzleIdentity, OGL_IDENTITY_SWIZZLE},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, swizzle);
        if(native == nullptr)
            throw std::runtime_error("No native ComponentSwizzle found");

        return *native;
    }

    GLenum FilterToNative(Render::Filter filter)
    {
        constexpr static std::pair<Render::Filter, GLenum> mapping[] = {
            {Render::Filter::Nearest, GL_NEAREST},
            {Render::Filter::Linear, GL_LINEAR},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, filter);
        if(native == nullptr)
            throw std::runtime_error("No native Filter found");

        return *native;
    }

    GLenum AddressModeToNative(Render::AddressMode mode)
    {
        constexpr static std::pair<Render::AddressMode, GLenum> mapping[] = {
            {Render::AddressMode::Repeat, GL_REPEAT},
            {Render::AddressMode::MirroredRepeat, GL_MIRRORED_REPEAT},
            {Render::AddressMode::ClampToEdge, GL_CLAMP_TO_EDGE},
            {Render::AddressMode::ClampToBorder, GL_CLAMP_TO_BORDER},
            {Render::AddressMode::MirrorClampToEdge, GL_MIRROR_CLAMP_TO_EDGE},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, mode);
        if(native == nullptr)
            throw std::runtime_error("No native AddressMode found");

        return *native;
    }

    GLenum ShaderStageToNative(Render::ShaderStageFlagBits stage)
    {
        constexpr static std::pair<Render::ShaderStageFlagBits, GLenum> mapping[] = {
            {Render::ShaderStageFlagBits::Vertex, GL_VERTEX_SHADER},
            {Render::ShaderStageFlagBits::TessellationControl, GL_TESS_CONTROL_SHADER},
            {Render::ShaderStageFlagBits::TessellationEvaluation, GL_TESS_EVALUATION_SHADER},
            {Render::ShaderStageFlagBits::Geometry, GL_GEOMETRY_SHADER},
            {Render::ShaderStageFlagBits::Fragment, GL_FRAGMENT_SHADER},
            {Render::ShaderStageFlagBits::Compute, GL_COMPUTE_SHADER},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, stage);
        if(native == nullptr)
            throw std::runtime_error("No native ShaderStage found");

        return *native;
    }

    GLenum InputRateToNative(Render::InputRate rate)
    {
        constexpr static std::pair<Render::InputRate, GLenum> mapping[] = {
            {Render::InputRate::VertexRate, 0},
            {Render::InputRate::InstanceRate, 1},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, rate);
        if(native == nullptr)
            throw std::runtime_error("No native InputRate found");

        return *native;
    }

    GLenum PrimitiveTopologyToNative(Render::PrimitiveTopology topology)
    {
        constexpr static std::pair<Render::PrimitiveTopology, GLenum> mapping[] = {
            {Render::PrimitiveTopology::Points, GL_POINTS},
            {Render::PrimitiveTopology::Lines, GL_LINES},
            {Render::PrimitiveTopology::LineStrip, GL_LINE_STRIP},
            {Render::PrimitiveTopology::Triangles, GL_TRIANGLES},
            {Render::PrimitiveTopology::TriangleStrip, GL_TRIANGLE_STRIP},
            {Render::PrimitiveTopology::TriangleFan, GL_TRIANGLE_FAN},
            {Render::PrimitiveTopology::LinesAdjacency, GL_LINES_ADJACENCY},
            {Render::PrimitiveTopology::LineStripAdjacency, GL_LINE_STRIP_ADJACENCY},
            {Render::PrimitiveTopology::TrianglesAdjacency, GL_TRIANGLES_ADJACENCY},
            {Render::PrimitiveTopology::TriangleStrIpAdjacency, GL_TRIANGLE_STRIP_ADJACENCY},
            {Render::PrimitiveTopology::Patches, GL_PATCHES},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, topology);
        if(native == nullptr)
            throw std::runtime_error("No native PrimitiveTopology found");

        return *native;
    }

    GLenum BlendFactorToNative(Render::BlendFactor factor)
    {
        constexpr static std::pair<Render::BlendFactor, GLenum> mapping[] = {
            {Render::BlendFactor::Zero, GL_ZERO},
            {Render::BlendFactor::One, GL_ONE},
            {Render::BlendFactor::SrcColor, GL_SRC_COLOR},
            {Render::BlendFactor::OneMinusSrcColor, GL_ONE_MINUS_SRC_COLOR},
            {Render::BlendFactor::DstColor, GL_DST_COLOR},
            {Render::BlendFactor::OneMinusDstColor, GL_ONE_MINUS_DST_COLOR},
            {Render::BlendFactor::SrcAlpha, GL_SRC_ALPHA},
            {Render::BlendFactor::OneMinusSrcAlpha, GL_ONE_MINUS_SRC_ALPHA},
            {Render::BlendFactor::DstAlpha, GL_DST_ALPHA},
            {Render::BlendFactor::OneMinusDstAlpha, GL_ONE_MINUS_DST_ALPHA},
            {Render::BlendFactor::ConstantColor, GL_CONSTANT_COLOR},

            {Render::BlendFactor::OneMinusConstantColor, GL_ONE_MINUS_CONSTANT_COLOR},
            {Render::BlendFactor::ConstantAlpha, GL_CONSTANT_ALPHA},
            {Render::BlendFactor::OneMinusConstantAlpha, GL_ONE_MINUS_CONSTANT_ALPHA},
            {Render::BlendFactor::SrcAlphaSaturate, GL_SRC_ALPHA_SATURATE},
            {Render::BlendFactor::Src1Color, GL_SRC1_COLOR},
            {Render::BlendFactor::OneMinusSrc1Color, GL_ONE_MINUS_SRC1_COLOR},
            {Render::BlendFactor::Src1Alpha, GL_SRC1_ALPHA},
            {Render::BlendFactor::OneMinusSrc1Alpha, GL_ONE_MINUS_SRC1_ALPHA},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, factor);
        if(native == nullptr)
            throw std::runtime_error("No native BlendFactor found");

        return *native;
    }

    GLenum BlendLogicOpToNative(Render::BlendLogicOp op)
    {
        constexpr static std::pair<Render::BlendLogicOp, GLenum> mapping[] = {
            {Render::BlendLogicOp::Clear, GL_CLEAR},
            {Render::BlendLogicOp::Set, GL_SET},
            {Render::BlendLogicOp::Copy, GL_COPY},
            {Render::BlendLogicOp::CopyInverted, GL_COPY_INVERTED},
            {Render::BlendLogicOp::Noop, GL_NOOP},
            {Render::BlendLogicOp::Invert, GL_INVERT},
            {Render::BlendLogicOp::And, GL_AND},
            {Render::BlendLogicOp::NotAnd, GL_NAND},
            {Render::BlendLogicOp::Or, GL_OR},
            {Render::BlendLogicOp::NotOr, GL_NOR},
            {Render::BlendLogicOp::Xor, GL_XOR},

            {Render::BlendLogicOp::Equivalent, GL_EQUIV},
            {Render::BlendLogicOp::AndReverse, GL_AND_REVERSE},
            {Render::BlendLogicOp::AndInverted, GL_AND_INVERTED},
            {Render::BlendLogicOp::OrReverse, GL_OR_REVERSE},
            {Render::BlendLogicOp::OrInverted, GL_OR_INVERTED},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, op);
        if(native == nullptr)
            throw std::runtime_error("No native BlendLogicOp found");

        return *native;
    }

    GLenum BlendOpToNative(Render::BlendOp op)
    {
        constexpr static std::pair<Render::BlendOp, GLenum> mapping[] = {
            {Render::BlendOp::Add, GL_FUNC_ADD},
            {Render::BlendOp::Subtract, GL_FUNC_SUBTRACT},
            {Render::BlendOp::ReverseSubstract, GL_FUNC_REVERSE_SUBTRACT},
            {Render::BlendOp::Min, GL_MIN},
            {Render::BlendOp::Max, GL_MAX},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, op);
        if(native == nullptr)
            throw std::runtime_error("No native BlendOp found");

        return *native;
    }

    GLenum StencilOpToNative(Render::StencilOp op)
    {
        constexpr static std::pair<Render::StencilOp, GLenum> mapping[] = {
            {Render::StencilOp::Keep, GL_KEEP},
            {Render::StencilOp::Zero, GL_ZERO},
            {Render::StencilOp::Replace, GL_REPLACE},
            {Render::StencilOp::Increment, GL_INCR},
            {Render::StencilOp::IncrementWrap, GL_INCR_WRAP},
            {Render::StencilOp::Decrement, GL_DECR},
            {Render::StencilOp::DecrementWrap, GL_DECR_WRAP},
            {Render::StencilOp::Invert, GL_INVERT},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, op);
        if(native == nullptr)
            throw std::runtime_error("No native StencilOp found");

        return *native;
    }

    GLenum PolygonModeToNative(Render::PolygonMode mode)
    {
        constexpr static std::pair<Render::PolygonMode, GLenum> mapping[] = {
            {Render::PolygonMode::Point, GL_POINT},
            {Render::PolygonMode::Line, GL_LINE},
            {Render::PolygonMode::Fill, GL_FILL},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, mode);
        if(native == nullptr)
            throw std::runtime_error("No native PolygonMode found");

        return *native;
    }

    GLenum CullModeToNative(Render::CullMode mode)
    {
        constexpr static std::pair<Render::CullMode, GLenum> mapping[] = {
            {Render::CullMode::None, OGL_CULL_MODE_NONE},
            {Render::CullMode::Front, GL_FRONT},
            {Render::CullMode::Back, GL_BACK},
            {Render::CullMode::FrontAndBack, GL_FRONT_AND_BACK},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, mode);
        if(native == nullptr)
            throw std::runtime_error("No native CullMode found");

        return *native;
    }

    GLenum FrontFaceToNative(Render::FrontFace face)
    {
        constexpr static std::pair<Render::FrontFace, GLenum> mapping[] = {
            {Render::FrontFace::CounterClockwise, GL_CCW},
            {Render::FrontFace::Clockwise, GL_CW},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, face);
        if(native == nullptr)
            throw std::runtime_error("No native FrontFace found");

        return *native;
    }

    GLenum IndexTypeToNative(Render::IndexType type)
    {
        constexpr static std::pair<Render::IndexType, GLenum> mapping[] = {
            {Render::IndexType::U8, GL_UNSIGNED_BYTE},
            {Render::IndexType::U16, GL_UNSIGNED_SHORT},
            {Render::IndexType::U32, GL_UNSIGNED_INT},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const GLenum* native = hrs::mapping_search(mapping, type);
        if(native == nullptr)
            throw std::runtime_error("No native IndexType found");

        return *native;
    }

    TransferImageTypeFormat DecodeTransferTypeFormatPair(Render::Format format)
    {
        constexpr static std::pair<Render::Format, TransferImageTypeFormat> mapping[] = {
            {Render::Format::R32G32B32A32_FLOAT,
             TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RGBA}},
            {Render::Format::R32G32B32A32_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RGBA_INTEGER}},
            {Render::Format::R32G32B32A32_SINT,
             TransferImageTypeFormat{.type = GL_INT, .format = GL_RGBA_INTEGER}},

            {Render::Format::R32G32B32_FLOAT,
             TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RGB}},
            {Render::Format::R32G32B32_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RGB_INTEGER}},
            {Render::Format::R32G32B32_SINT,
             TransferImageTypeFormat{.type = GL_INT, .format = GL_RGB_INTEGER}},

            {Render::Format::R16G16B16A16_FLOAT,
             TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RGBA}},
            {Render::Format::R16G16B16A16_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGBA}},
            {Render::Format::R16G16B16A16_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RGBA_INTEGER}},
            {Render::Format::R16G16B16A16_SNORM,
             TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGBA}},
            {Render::Format::R16G16B16A16_SINT,
             TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RGBA_INTEGER}},

            {Render::Format::R32G32_FLOAT,
             TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RG}},
            {Render::Format::R32G32_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RG_INTEGER}},
            {Render::Format::R32G32_SINT,
             TransferImageTypeFormat{.type = GL_INT, .format = GL_RG_INTEGER}},

            {Render::Format::D32_FLOAT_S8X24_UINT,
             TransferImageTypeFormat{.type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
                                     .format = GL_DEPTH_STENCIL}},

            {Render::Format::R10G10B10A2_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT_2_10_10_10_REV, .format = GL_RGBA}},
            {Render::Format::R10G10B10A2_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT_10_10_10_2,
                                     .format = GL_RGBA_INTEGER}},
            {Render::Format::R11G11B10_FLOAT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT_10F_11F_11F_REV, .format = GL_BGR}},

            {Render::Format::R8G8B8A8_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA}},
            {Render::Format::R8G8B8A8_UNORM_SRGB,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA}},
            {Render::Format::R8G8B8A8_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RGBA_INTEGER}},
            {Render::Format::R8G8B8A8_SNORM,
             TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGBA}},
            {Render::Format::R8G8B8A8_SINT,
             TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RGBA_INTEGER}},

            {Render::Format::R16G16_FLOAT,
             TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RG}},
            {Render::Format::R16G16_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RG}},
            {Render::Format::R16G16_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RG_INTEGER}},
            {Render::Format::R16G16_SNORM,
             TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RG}},
            {Render::Format::R16G16_SINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RG_INTEGER}},

            {Render::Format::D32_FLOAT,
             TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_DEPTH_COMPONENT}},
            {Render::Format::R32_FLOAT,
             TransferImageTypeFormat{.type = GL_FLOAT, .format = GL_RED}},
            {Render::Format::R32_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT, .format = GL_RED_INTEGER}},
            {Render::Format::R32_SINT,
             TransferImageTypeFormat{.type = GL_INT, .format = GL_RED_INTEGER}},

            {Render::Format::D24_UNORM_S8_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT_24_8, .format = GL_DEPTH_STENCIL}},

            {Render::Format::R8G8_UNORM, TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RG}},
            {Render::Format::R8G8_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RG_INTEGER}},
            {Render::Format::R8G8_SNORM, TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RG}},
            {Render::Format::R8G8_SINT,
             TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RG_INTEGER}},

            {Render::Format::R16_FLOAT,
             TransferImageTypeFormat{.type = GL_HALF_FLOAT, .format = GL_RED}},
            {Render::Format::D16_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_DEPTH_COMPONENT}},
            {Render::Format::R16_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RED}},
            {Render::Format::R16_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT, .format = GL_RED_INTEGER}},
            {Render::Format::R16_SNORM,
             TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RED}},
            {Render::Format::R16_SINT,
             TransferImageTypeFormat{.type = GL_SHORT, .format = GL_RED_INTEGER}},

            {Render::Format::R8_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RED}},
            {Render::Format::R8_UINT,
             TransferImageTypeFormat{.type = GL_UNSIGNED_BYTE, .format = GL_RED_INTEGER}},
            {Render::Format::R8_SNORM, TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RED}},
            {Render::Format::R8_SINT,
             TransferImageTypeFormat{.type = GL_BYTE, .format = GL_RED_INTEGER}},

            {Render::Format::R9G9B9E5_SHAREDEXP,
             TransferImageTypeFormat{.type = GL_UNSIGNED_INT_5_9_9_9_REV,
                                     .format = GL_BGR_INTEGER}},

            {Render::Format::B5G6R5_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_6_5, .format = GL_BGR}},
            {Render::Format::B5G5R5A1_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_5_5_5_1, .format = GL_BGRA}},

            {Render::Format::B4G4R4A4_UNORM,
             TransferImageTypeFormat{.type = GL_UNSIGNED_SHORT_4_4_4_4, .format = GL_BGRA}},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const TransferImageTypeFormat* native = hrs::mapping_search(mapping, format);
        if(native == nullptr)
            throw std::runtime_error("No native format found");

        return *native;

        //RGB
        //R3G3B2_Unorm = GL_R3_G3_B2,
        //RGB4_UNorm = GL_RGB4,
        //RGB5_Unorm = GL_RGB5,

        //RGB10_UNorm = GL_RGB10,
        //RGB12_UNorm = GL_RGB12,

        //RGB8_UNorm = GL_RGB8,
        //RGB8_SNorm = GL_RGB8_SNORM,
        //RGB8_Int = GL_RGB8I,
        //RGB8_UInt = GL_RGB8UI,

        //RGB16_UNorm = GL_RGB16,
        //RGB16_SNorm = GL_RGB16_SNORM,
        //RGB16_Int = GL_RGB16I,
        //RGB16_UInt = GL_RGB16UI,
        //RGB16_Float = GL_RGB16F,

        //SRGB8_UNorm = GL_SRGB8,

        //RGBA
        //RGBA2_UNorm = GL_RGBA2,

        //RGBA12_UNorm = GL_RGBA12,

        //Depth
        //D24 = GL_DEPTH_COMPONENT24,
        //D32 = GL_DEPTH_COMPONENT32,

        //StencilIndex
        //S1 = GL_STENCIL_INDEX1,
        //S4 = GL_STENCIL_INDEX4,
        //S8 = GL_STENCIL_INDEX8,
        //S16 = GL_STENCIL_INDEX16,

        //Compressed EAC/ETC2
        //R11_EAC_UNorm = GL_COMPRESSED_R11_EAC,
        //RG11_EAC_UNorm = GL_COMPRESSED_RG11_EAC,
        //RGBA8_ETC2_EAC_UNorm = GL_COMPRESSED_RGBA8_ETC2_EAC,
        //R11_EAC_SNorm = GL_COMPRESSED_SIGNED_R11_EAC,
        //RG11_EAC_SNorm = GL_COMPRESSED_SIGNED_RG11_EAC,
        //RGB8_ETC2 = GL_COMPRESSED_RGB8_ETC2,
        //RGB8_A1_ETC2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
        //SRGBA8_ETC2_EAC = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
        //SRGB8_ETC2 = GL_COMPRESSED_SRGB8_ETC2,
        //SRGB8_A1_ET2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
    }

    VertexInputTypeSize DecodeVertexInputTypeSizePair(Render::Format format)
    {
        constexpr static std::pair<Render::Format, VertexInputTypeSize> mapping[] = {
            {Render::Format::R32G32B32A32_FLOAT,
             VertexInputTypeSize{.type = GL_FLOAT, .size = 4, .normalized = false}},
            {Render::Format::R32G32B32A32_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT, .size = 4, .normalized = false}},
            {Render::Format::R32G32B32A32_SINT,
             VertexInputTypeSize{.type = GL_INT, .size = 4, .normalized = false}},

            {Render::Format::R32G32B32_FLOAT,
             VertexInputTypeSize{.type = GL_FLOAT, .size = 3, .normalized = false}},
            {Render::Format::R32G32B32_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT, .size = 3, .normalized = false}},
            {Render::Format::R32G32B32_SINT,
             VertexInputTypeSize{.type = GL_INT, .size = 3, .normalized = false}},

            {Render::Format::R16G16B16A16_FLOAT,
             VertexInputTypeSize{.type = GL_HALF_FLOAT, .size = 4, .normalized = false}},
            {Render::Format::R16G16B16A16_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 4, .normalized = true}},
            {Render::Format::R16G16B16A16_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 4, .normalized = false}},
            {Render::Format::R16G16B16A16_SNORM,
             VertexInputTypeSize{.type = GL_SHORT, .size = 4, .normalized = true}},
            {Render::Format::R16G16B16A16_SINT,
             VertexInputTypeSize{.type = GL_SHORT, .size = 4, .normalized = false}},

            {Render::Format::R32G32_FLOAT,
             VertexInputTypeSize{.type = GL_FLOAT, .size = 2, .normalized = false}},
            {Render::Format::R32G32_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT, .size = 2, .normalized = false}},
            {Render::Format::R32G32_SINT,
             VertexInputTypeSize{.type = GL_INT, .size = 2, .normalized = false}},

            {Render::Format::R10G10B10A2_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                 .size = GL_BGRA,
                                 .normalized = true}},
            {Render::Format::R10G10B10A2_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT_2_10_10_10_REV,
                                 .size = GL_BGRA,
                                 .normalized = false}},
            {Render::Format::R11G11B10_FLOAT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT_10F_11F_11F_REV,
                                 .size = 3,
                                 .normalized = false}},

            {Render::Format::R8G8B8A8_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 4, .normalized = true}},
            {Render::Format::R8G8B8A8_UNORM_SRGB,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 4, .normalized = true}},
            {Render::Format::R8G8B8A8_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 4, .normalized = false}},
            {Render::Format::R8G8B8A8_SNORM,
             VertexInputTypeSize{.type = GL_BYTE, .size = 4, .normalized = true}},
            {Render::Format::R8G8B8A8_SINT,
             VertexInputTypeSize{.type = GL_BYTE, .size = 4, .normalized = false}},

            {Render::Format::R16G16_FLOAT,
             VertexInputTypeSize{.type = GL_HALF_FLOAT, .size = 2, .normalized = false}},
            {Render::Format::R16G16_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 2, .normalized = true}},
            {Render::Format::R16G16_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 2, .normalized = false}},
            {Render::Format::R16G16_SNORM,
             VertexInputTypeSize{.type = GL_SHORT, .size = 2, .normalized = true}},
            {Render::Format::R16G16_SINT,
             VertexInputTypeSize{.type = GL_SHORT, .size = 2, .normalized = false}},

            {Render::Format::R32_FLOAT,
             VertexInputTypeSize{.type = GL_FLOAT, .size = 1, .normalized = false}},
            {Render::Format::R32_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_INT, .size = 1, .normalized = false}},
            {Render::Format::R32_SINT,
             VertexInputTypeSize{.type = GL_INT, .size = 1, .normalized = false}},

            {Render::Format::R8G8_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 2, .normalized = true}},
            {Render::Format::R8G8_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 2, .normalized = false}},
            {Render::Format::R8G8_SNORM,
             VertexInputTypeSize{.type = GL_BYTE, .size = 2, .normalized = true}},
            {Render::Format::R8G8_SINT,
             VertexInputTypeSize{.type = GL_BYTE, .size = 2, .normalized = false}},

            {Render::Format::R16_FLOAT,
             VertexInputTypeSize{.type = GL_HALF_FLOAT, .size = 1, .normalized = false}},
            {Render::Format::R16_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 1, .normalized = true}},
            {Render::Format::R16_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_SHORT, .size = 1, .normalized = false}},
            {Render::Format::R16_SNORM,
             VertexInputTypeSize{.type = GL_SHORT, .size = 1, .normalized = true}},
            {Render::Format::R16_SINT,
             VertexInputTypeSize{.type = GL_SHORT, .size = 1, .normalized = false}},

            {Render::Format::R8_UNORM,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 1, .normalized = true}},
            {Render::Format::R8_UINT,
             VertexInputTypeSize{.type = GL_UNSIGNED_BYTE, .size = 1, .normalized = false}},
            {Render::Format::R8_SNORM,
             VertexInputTypeSize{.type = GL_BYTE, .size = 1, .normalized = true}},
            {Render::Format::R8_SINT,
             VertexInputTypeSize{.type = GL_BYTE, .size = 1, .normalized = false}},
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const VertexInputTypeSize* native = hrs::mapping_search(mapping, format);
        if(native == nullptr)
            throw std::runtime_error("No native format found");

        return *native;

        //RGB
        //R3G3B2_Unorm = GL_R3_G3_B2,
        //RGB4_UNorm = GL_RGB4,
        //RGB5_Unorm = GL_RGB5,

        //RGB10_UNorm = GL_RGB10,
        //RGB12_UNorm = GL_RGB12,

        //RGB8_UNorm = GL_RGB8,
        //RGB8_SNorm = GL_RGB8_SNORM,
        //RGB8_Int = GL_RGB8I,
        //RGB8_UInt = GL_RGB8UI,

        //RGB16_UNorm = GL_RGB16,
        //RGB16_SNorm = GL_RGB16_SNORM,
        //RGB16_Int = GL_RGB16I,
        //RGB16_UInt = GL_RGB16UI,
        //RGB16_Float = GL_RGB16F,

        //SRGB8_UNorm = GL_SRGB8,

        //RGBA
        //RGBA2_UNorm = GL_RGBA2,

        //RGBA12_UNorm = GL_RGBA12,

        //Depth
        //D24 = GL_DEPTH_COMPONENT24,
        //D32 = GL_DEPTH_COMPONENT32,

        //StencilIndex
        //S1 = GL_STENCIL_INDEX1,
        //S4 = GL_STENCIL_INDEX4,
        //S8 = GL_STENCIL_INDEX8,
        //S16 = GL_STENCIL_INDEX16,

        //Compressed EAC/ETC2
        //R11_EAC_UNorm = GL_COMPRESSED_R11_EAC,
        //RG11_EAC_UNorm = GL_COMPRESSED_RG11_EAC,
        //RGBA8_ETC2_EAC_UNorm = GL_COMPRESSED_RGBA8_ETC2_EAC,
        //R11_EAC_SNorm = GL_COMPRESSED_SIGNED_R11_EAC,
        //RG11_EAC_SNorm = GL_COMPRESSED_SIGNED_RG11_EAC,
        //RGB8_ETC2 = GL_COMPRESSED_RGB8_ETC2,
        //RGB8_A1_ETC2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
        //SRGBA8_ETC2_EAC = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
        //SRGB8_ETC2 = GL_COMPRESSED_SRGB8_ETC2,
        //SRGB8_A1_ET2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
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
        constexpr static std::pair<GLenum, Render::DebugMessengerTypeFlagBits> mapping[] = {
            {GL_DEBUG_TYPE_ERROR, Render::DebugMessengerTypeFlagBits::Validation}, //56
            {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
             Render::DebugMessengerTypeFlagBits::Validation}, //57
            {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, Render::DebugMessengerTypeFlagBits::Validation}, //58
            {GL_DEBUG_TYPE_PORTABILITY, Render::DebugMessengerTypeFlagBits::General}, //59
            {GL_DEBUG_TYPE_PERFORMANCE, Render::DebugMessengerTypeFlagBits::Performance}, //60
            {GL_DEBUG_TYPE_OTHER, Render::DebugMessengerTypeFlagBits::General}, //61
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const Render::DebugMessengerTypeFlagBits* native = hrs::mapping_search(mapping, type);
        if(native == nullptr)
            return Render::DebugMessengerTypeFlagBits::
                General; //return General due to driver-specific codes

        return *native;
    }

    Render::DebugMessengerSeverityFlagBits
    NativeDebugMessengerSeverityFlagBitToSpec(GLenum severity)
    {
        constexpr static std::pair<GLenum, Render::DebugMessengerSeverityFlagBits> mapping[] = {
            {GL_DEBUG_SEVERITY_NOTIFICATION,
             Render::DebugMessengerSeverityFlagBits::Verbose}, //33387
            {GL_DEBUG_SEVERITY_HIGH, Render::DebugMessengerSeverityFlagBits::Error}, //37190
            {GL_DEBUG_SEVERITY_MEDIUM, Render::DebugMessengerSeverityFlagBits::Warning}, //37191
            {GL_DEBUG_SEVERITY_LOW, Render::DebugMessengerSeverityFlagBits::Info}, //37192
        };

        CHECK_MAPPING_IS_SORTED(mapping)

        const Render::DebugMessengerSeverityFlagBits* native =
            hrs::mapping_search(mapping, severity);
        if(native == nullptr)
            return Render::DebugMessengerSeverityFlagBits::
                Verbose; //return Verbose due to driver-specific codes

        return *native;
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