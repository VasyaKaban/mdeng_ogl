#pragma once

#include <cstdint>
#include "Common.h"

enum class Format
{
    //DXGI_FORMAT_UNKNOWN = 0,
    //DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
    R32G32B32A32_FLOAT = GL_RGBA32F, //DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
    R32G32B32A32_UINT = GL_RGBA32UI, //DXGI_FORMAT_R32G32B32A32_UINT = 3,
    R32G32B32A32_SINT = GL_RGBA32I, //DXGI_FORMAT_R32G32B32A32_SINT = 4,
    //DXGI_FORMAT_R32G32B32_TYPELESS = 5,
    R32G32B32_FLOAT = GL_RGB32F, //DXGI_FORMAT_R32G32B32_FLOAT = 6,
    R32G32B32_UINT = GL_RGB32UI, //DXGI_FORMAT_R32G32B32_UINT = 7,
    R32G32B32_SINT = GL_RGB32I, //DXGI_FORMAT_R32G32B32_SINT = 8,
    //DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
    R16G16B16A16_FLOAT = GL_RGBA16F, //DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
    R16G16B16A16_UNORM = GL_RGBA16, //DXGI_FORMAT_R16G16B16A16_UNORM = 11,
    R16G16B16A16_UINT = GL_RGBA16UI, //DXGI_FORMAT_R16G16B16A16_UINT = 12,
    R16G16B16A16_SNORM = GL_RGBA16_SNORM, //DXGI_FORMAT_R16G16B16A16_SNORM = 13,
    R16G16B16A16_SINT = GL_RGBA16I, //DXGI_FORMAT_R16G16B16A16_SINT = 14,
    //DXGI_FORMAT_R32G32_TYPELESS = 15,
    R32G32_FLOAT = GL_RG32F, //DXGI_FORMAT_R32G32_FLOAT = 16,
    R32G32_UINT = GL_RG32UI, //DXGI_FORMAT_R32G32_UINT = 17,
    R32G32_SINT = GL_RG32I, //DXGI_FORMAT_R32G32_SINT = 18,
    //DXGI_FORMAT_R32G8X24_TYPELESS = 19,
    D32_FLOAT_S8X24_UINT = GL_DEPTH32F_STENCIL8, //DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    //DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    //DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    //DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
    R10G10B10A2_UNORM = GL_RGB10_A2, //DXGI_FORMAT_R10G10B10A2_UNORM = 24,
    R10G10B10A2_UINT = GL_RGB10_A2UI, //DXGI_FORMAT_R10G10B10A2_UINT = 25,
    R11G11B10_FLOAT = GL_R11F_G11F_B10F, // DXGI_FORMAT_R11G11B10_FLOAT = 26,
    //DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
    R8G8B8A8_UNORM = GL_RGBA8, //DXGI_FORMAT_R8G8B8A8_UNORM = 28,
    R8G8B8A8_UNORM_SRGB = GL_SRGB8_ALPHA8, // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    R8G8B8A8_UINT = GL_RGBA8UI, // DXGI_FORMAT_R8G8B8A8_UINT = 30,
    R8G8B8A8_SNORM = GL_RGBA8_SNORM, // DXGI_FORMAT_R8G8B8A8_SNORM = 31,
    R8G8B8A8_SINT = GL_RGBA8I, // DXGI_FORMAT_R8G8B8A8_SINT = 32,
    //DXGI_FORMAT_R16G16_TYPELESS = 33,
    R16G16_FLOAT = GL_RG16F, // DXGI_FORMAT_R16G16_FLOAT = 34,
    R16G16_UNORM = GL_RG16, // DXGI_FORMAT_R16G16_UNORM = 35,
    R16G16_UINT = GL_RG16UI, // DXGI_FORMAT_R16G16_UINT = 36,
    R16G16_SNORM = GL_RG16_SNORM, // DXGI_FORMAT_R16G16_SNORM = 37,
    R16G16_SINT = GL_RG16I, // DXGI_FORMAT_R16G16_SINT = 38,
    //DXGI_FORMAT_R32_TYPELESS = 39,
    D32_FLOAT = GL_DEPTH_COMPONENT32F, //DXGI_FORMAT_D32_FLOAT = 40,
    R32_FLOAT = GL_R32F, // DXGI_FORMAT_R32_FLOAT = 41,
    R32_UINT = GL_R32UI, //DXGI_FORMAT_R32_UINT = 42,
    R32_SINT = GL_R32I, //DXGI_FORMAT_R32_SINT = 43,
    //DXGI_FORMAT_R24G8_TYPELESS = 44,
    D24_UNORM_S8_UINT = GL_DEPTH24_STENCIL8, // DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
    //DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    //DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
    //DXGI_FORMAT_R8G8_TYPELESS = 48,
    R8G8_UNORM = GL_RG8, // DXGI_FORMAT_R8G8_UNORM = 49,
    R8G8_UINT = GL_RG8UI, // DXGI_FORMAT_R8G8_UINT = 50,
    R8G8_SNORM = GL_RG8_SNORM, // DXGI_FORMAT_R8G8_SNORM = 51,
    R8G8_SINT = GL_RG8I, // DXGI_FORMAT_R8G8_SINT = 52,
    //DXGI_FORMAT_R16_TYPELESS = 53,
    R16_FLOAT = GL_R16F, // DXGI_FORMAT_R16_FLOAT = 54,
    D16_UNORM = GL_DEPTH_COMPONENT16, // DXGI_FORMAT_D16_UNORM = 55,
    R16_UNORM = GL_R16, // DXGI_FORMAT_R16_UNORM = 56,
    R16_UINT = GL_R16UI, // DXGI_FORMAT_R16_UINT = 57,
    R16_SNORM = GL_R16_SNORM, // DXGI_FORMAT_R16_SNORM = 58,
    R16_SINT = GL_R16I, //DXGI_FORMAT_R16_SINT = 59,
    //DXGI_FORMAT_R8_TYPELESS = 60,
    R8_UNORM = GL_R8, //  DXGI_FORMAT_R8_UNORM = 61,
    R8_UINT = GL_R8UI, //  DXGI_FORMAT_R8_UINT = 62,
    R8_SNORM = GL_R8_SNORM, //  DXGI_FORMAT_R8_SNORM = 63,
    R8_SINT = GL_R8I, // DXGI_FORMAT_R8_SINT = 64,
    //DXGI_FORMAT_A8_UNORM = 65,
    //DXGI_FORMAT_R1_UNORM = 66,
    R9G9B9E5_SHAREDEXP = GL_RGB9_E5, //DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
    //DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
    //DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
    //DXGI_FORMAT_BC1_TYPELESS = 70,
    BC1_UNORM = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, // DXGI_FORMAT_BC1_UNORM = 71,
    BC1_UNORM_SRGB = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, // DXGI_FORMAT_BC1_UNORM_SRGB = 72,
    //DXGI_FORMAT_BC2_TYPELESS = 73,
    BC2_UNORM = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, // DXGI_FORMAT_BC2_UNORM = 74,
    BC2_UNORM_SRGB = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, // DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    //DXGI_FORMAT_BC3_TYPELESS = 76,
    BC3_UNORM = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, // DXGI_FORMAT_BC3_UNORM = 77,
    BC3_UNORM_SRGB = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, // DXGI_FORMAT_BC3_UNORM_SRGB = 78,
    //DXGI_FORMAT_BC4_TYPELESS = 79,
    BC4_UNORM = GL_COMPRESSED_RED_RGTC1, // DXGI_FORMAT_BC4_UNORM = 80,
    BC4_SNORM = GL_COMPRESSED_SIGNED_RED_RGTC1, // DXGI_FORMAT_BC4_SNORM = 81,
    //DXGI_FORMAT_BC5_TYPELESS = 82,
    BC5_UNORM = GL_COMPRESSED_RG_RGTC2, // DXGI_FORMAT_BC5_UNORM = 83,
    BC5_SNORM = GL_COMPRESSED_SIGNED_RG_RGTC2, // DXGI_FORMAT_BC5_SNORM = 84,
    B5G6R5_UNORM = GL_RGB565, // DXGI_FORMAT_B5G6R5_UNORM = 85,
    B5G5R5A1_UNORM = GL_RGB5_A1, // DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    //DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    //DXGI_FORMAT_B8G8R8X8_UNORM = 88,
    //DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    //DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
    //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    //DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
    //DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
    //DXGI_FORMAT_BC6H_TYPELESS = 94,
    BC6H_UF16 = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, //DXGI_FORMAT_BC6H_UF16 = 95,
    BC6H_SF16 = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, // DXGI_FORMAT_BC6H_SF16 = 96,
    //DXGI_FORMAT_BC7_TYPELESS = 97,
    BC7_UNORM = GL_COMPRESSED_RGBA_BPTC_UNORM, // DXGI_FORMAT_BC7_UNORM = 98,
    BC7_UNORM_SRGB = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, // DXGI_FORMAT_BC7_UNORM_SRGB = 99,
    //DXGI_FORMAT_AYUV = 100,
    //DXGI_FORMAT_Y410 = 101,
    //DXGI_FORMAT_Y416 = 102,
    //DXGI_FORMAT_NV12 = 103,
    //DXGI_FORMAT_P010 = 104,
    //DXGI_FORMAT_P016 = 105,
    //DXGI_FORMAT_420_OPAQUE = 106,
    //DXGI_FORMAT_YUY2 = 107,
    //DXGI_FORMAT_Y210 = 108,
    //DXGI_FORMAT_Y216 = 109,
    //DXGI_FORMAT_NV11 = 110,
    //DXGI_FORMAT_AI44 = 111,
    //DXGI_FORMAT_IA44 = 112,
    //DXGI_FORMAT_P8 = 113,
    //DXGI_FORMAT_A8P8 = 114,
    B4G4R4A4_UNORM = GL_RGBA4, //DXGI_FORMAT_B4G4R4A4_UNORM = 115,
    //DXGI_FORMAT_P208 = 130,
    //DXGI_FORMAT_V208 = 131,
    //DXGI_FORMAT_V408 = 132,
    //DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
    //DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
    //DXGI_FORMAT_FORCE_UINT = 0xff'ff'ff'ff
};

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

enum class InitDataFormat
{
    R = GL_RED,
    RG = GL_RG,
    RGB = GL_RGB,
    BGR = GL_BGR,
    RGBA = GL_RGBA,
    BGRA = GL_BGRA,
    Depth = GL_DEPTH_COMPONENT,
    Stencil = GL_STENCIL_INDEX
};

enum class InitDataType
{
    U8 = GL_UNSIGNED_BYTE,
    I8 = GL_BYTE,
    U16 = GL_UNSIGNED_SHORT,
    I16 = GL_SHORT,
    U32 = GL_UNSIGNED_INT,
    I32 = GL_INT,
    F16 = GL_HALF_FLOAT,
    F32 = GL_FLOAT,
    U8_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
    U8_2_3_3_Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
    U16_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
    U16_5_6_5_Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
    U16_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
    U16_4_4_4_4_Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
    U16_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
    U16_1_5_5_5_Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
    U32_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
    U32_8_8_8_8_Rev = GL_UNSIGNED_INT_8_8_8_8_REV,
    U32_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
    U32_2_10_10_10_Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
    U32_24_8 = GL_UNSIGNED_INT_24_8,
    U32_10F_11F_11F_Rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
    U32_5_9_9_9_Rev = GL_UNSIGNED_INT_5_9_9_9_REV,
    F32_U32_24_8_REV = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
};

constexpr bool IsFormatCompressed(Format format) noexcept
{
    bool is_compressed;

    switch(format)
    {
        case Format::BC1_UNORM:
        case Format::BC1_UNORM_SRGB:
        case Format::BC2_UNORM:
        case Format::BC2_UNORM_SRGB:
        case Format::BC3_UNORM:
        case Format::BC3_UNORM_SRGB:
        case Format::BC4_UNORM:
        case Format::BC4_SNORM:
        case Format::BC5_UNORM:
        case Format::BC5_SNORM:
        case Format::BC6H_UF16:
        case Format::BC6H_SF16:
        case Format::BC7_UNORM:
        case Format::BC7_UNORM_SRGB:
            is_compressed = true;
            break;
        default:
            is_compressed = false;
            break;
    }

    return is_compressed;
}

constexpr std::uint8_t GetFormatBlockSize(Format format) noexcept
{
    std::uint8_t block_size;

    switch(format)
    {
        case Format::BC1_UNORM:
        case Format::BC1_UNORM_SRGB:
        case Format::BC4_UNORM:
        case Format::BC4_SNORM:
            block_size = 8;
            break;
        case Format::BC2_UNORM:
        case Format::BC2_UNORM_SRGB:
        case Format::BC3_UNORM:
        case Format::BC3_UNORM_SRGB:
        case Format::BC5_UNORM:
        case Format::BC5_SNORM:
        case Format::BC6H_UF16:
        case Format::BC6H_SF16:
        case Format::BC7_UNORM:
        case Format::BC7_UNORM_SRGB:
            block_size = 16;
            break;
        default:
            block_size = 0;
            break;
    }

    return block_size;
}

constexpr std::uint32_t GetFormatRegionSize(Format format, const Extent3D& extent) noexcept
{
    std::uint32_t region_size;
    switch(format)
    {
        case Format::R32G32B32A32_FLOAT:
        case Format::R32G32B32A32_UINT:
        case Format::R32G32B32A32_SINT:
            region_size = extent.width * extent.height * extent.depth * 16;
            break;
        case Format::R32G32B32_FLOAT:
        case Format::R32G32B32_UINT:
        case Format::R32G32B32_SINT:
            region_size = extent.width * extent.height * extent.depth * 12;
            break;
        case Format::R16G16B16A16_FLOAT:
        case Format::R16G16B16A16_UNORM:
        case Format::R16G16B16A16_UINT:
        case Format::R16G16B16A16_SNORM:
        case Format::R16G16B16A16_SINT:
        case Format::R32G32_FLOAT:
        case Format::R32G32_UINT:
        case Format::R32G32_SINT:
        case Format::D32_FLOAT_S8X24_UINT:
            region_size = extent.width * extent.height * extent.depth * 8;
            break;
        case Format::R10G10B10A2_UNORM:
        case Format::R10G10B10A2_UINT:
        case Format::R11G11B10_FLOAT:
        case Format::R8G8B8A8_UNORM:
        case Format::R8G8B8A8_UNORM_SRGB:
        case Format::R8G8B8A8_UINT:
        case Format::R8G8B8A8_SNORM:
        case Format::R8G8B8A8_SINT:
        case Format::R16G16_FLOAT:
        case Format::R16G16_UNORM:
        case Format::R16G16_UINT:
        case Format::R16G16_SNORM:
        case Format::R16G16_SINT:
        case Format::D32_FLOAT:
        case Format::R32_FLOAT:
        case Format::R32_UINT:
        case Format::R32_SINT:
        case Format::D24_UNORM_S8_UINT:
        case Format::R9G9B9E5_SHAREDEXP:
            region_size = extent.width * extent.height * extent.depth * 4;
            break;
        case Format::R8G8_UNORM:
        case Format::R8G8_UINT:
        case Format::R8G8_SNORM:
        case Format::R8G8_SINT:
        case Format::R16_FLOAT:
        case Format::D16_UNORM:
        case Format::R16_UNORM:
        case Format::R16_UINT:
        case Format::R16_SNORM:
        case Format::R16_SINT:
        case Format::B5G6R5_UNORM:
        case Format::B5G5R5A1_UNORM:
        case Format::B4G4R4A4_UNORM:
            region_size = extent.width * extent.height * extent.depth * 2;
            break;
        case Format::R8_UNORM:
        case Format::R8_UINT:
        case Format::R8_SNORM:
        case Format::R8_SINT:
            region_size = extent.width * extent.height * extent.depth;
            break;
        case Format::BC1_UNORM:
        case Format::BC1_UNORM_SRGB:
        case Format::BC2_UNORM:
        case Format::BC2_UNORM_SRGB:
        case Format::BC3_UNORM:
        case Format::BC3_UNORM_SRGB:
        case Format::BC4_UNORM:
        case Format::BC4_SNORM:
        case Format::BC5_UNORM:
        case Format::BC5_SNORM:
        case Format::BC6H_UF16:
        case Format::BC6H_SF16:
        case Format::BC7_UNORM:
        case Format::BC7_UNORM_SRGB:
            region_size = ((extent.width + 3) / 4) * ((extent.height + 3) / 4) *
                          ((extent.depth + 3) / 4) * GetFormatBlockSize(format);
            break;
        default:
            region_size = 0;
            break;
    }

    return region_size;
}