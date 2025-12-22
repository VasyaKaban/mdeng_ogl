#pragma once

#include <cstdint>
#include <type_traits>
#include <span>
#include <vector>
#include <stdexcept>
#include "hrs/expected.hpp"
#include "../Render/Render.h"

namespace DDS
{
    enum class Result
    {
        BadData,
        UnsupportedPixelFormat,
        UnsupportedFourCC,
        UnsupportedDXGIFormat
    };

    class Exception : public std::exception
    {
    public:
        Exception(Result _result, std::string_view _message);
        Exception(Result _result, std::string&& _message) noexcept;

        Exception(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(const Exception&) = default;
        Exception& operator=(Exception&&) = default;

        virtual ~Exception() override = default;

        virtual const char* what() const noexcept override;

        Result GetResult() const noexcept;
        const std::string& GetMessage() const noexcept;
    private:
        Result result;
        std::string message;
    };

    using DDS_DWORD = std::uint32_t;

    constexpr DDS_DWORD
    MakeFourCC(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept
    {
        return static_cast<DDS_DWORD>(static_cast<std::uint8_t>(a)) << 0 |
               static_cast<DDS_DWORD>(static_cast<std::uint8_t>(b)) << 8 |
               static_cast<DDS_DWORD>(static_cast<std::uint8_t>(c)) << 16 |
               static_cast<DDS_DWORD>(static_cast<std::uint8_t>(d)) << 24;
    }

    constexpr std::array<std::uint8_t, 4> SparseFourCC(DDS_DWORD value) noexcept
    {
        return std::array<std::uint8_t, 4>{static_cast<std::uint8_t>(value),
                                           static_cast<std::uint8_t>(value >> 8),
                                           static_cast<std::uint8_t>(value >> 16),
                                           static_cast<std::uint8_t>(value >> 24)};
    }

    enum HeaderFlagBits : DDS_DWORD
    {
        DDSD_CAPS = 0x1, //
        DDSD_HEIGHT = 0x2, //
        DDSD_WIDTH = 0x4, //
        DDSD_PITCH = 0x8,
        DDSD_PIXELFORMAT = 0x10'00, //
        DDSD_MIPMAPCOUNT = 0x2'00'00,
        DDSD_LINEARSIZE = 0x8'00'00, //
        DDSD_DEPTH = 0x80'00'00
    };

    using HeaderFlags = std::underlying_type_t<HeaderFlagBits>;

    enum PixelFormatFlagBits : DDS_DWORD
    {
        DDPF_ALPHAPIXELS = 0x1,
        DDPF_ALPHA = 0x2,
        DDPF_FOURCC = 0x4,
        DDPF_RGB = 0x40,
        DDPF_YUV = 0x200,
        DDPF_LUMINANCE = 0x2'00'00
    };

    using PixelFormatFlags = std::underlying_type_t<PixelFormatFlagBits>;

    enum PixelFormatFourCC : DDS_DWORD
    {
        DXT1 = MakeFourCC('D', 'X', 'T', '1'),
        DXT2 = MakeFourCC('D', 'X', 'T', '2'),
        DXT3 = MakeFourCC('D', 'X', 'T', '3'),
        DXT4 = MakeFourCC('D', 'X', 'T', '4'),
        DXT5 = MakeFourCC('D', 'X', 'T', '5'),
        DX10 = MakeFourCC('D', 'X', '1', '0'),

        BC4U = MakeFourCC('B', 'C', '4', 'U'),
        BC4S = MakeFourCC('B', 'C', '4', 'S'),
        BC4U_ATI1 = MakeFourCC('A', 'T', 'I', '1'),
        BC5U = MakeFourCC('A', 'T', 'I', '2'),
        BC5S = MakeFourCC('B', 'C', '5', 'S'),

        R8G8_B8G8_U = MakeFourCC('R', 'G', 'B', 'G'),
        G8R8_G8B8_U = MakeFourCC('G', 'R', 'G', 'B'),

        R16G16B16A16_U = 36,
        R16G16B16A16_S = 110,
        R16_F = 111,
        R16G16_F = 112,
        R16G16B16A16_F = 113,
        R32_F = 114,
        R32G32_F = 115,
        R32G32B32A32_F = 116,

        UYVY = MakeFourCC('U', 'Y', 'V', 'Y'),
        YUY2 = MakeFourCC('Y', 'U', 'Y', '2'),
        CxV8U8 = 117
    };

    struct PixelFormat
    {
        DDS_DWORD size;
        PixelFormatFlags flags;
        PixelFormatFourCC four_cc;
        DDS_DWORD rgb_bit_count;
        DDS_DWORD red_bit_mask;
        DDS_DWORD green_bit_mask;
        DDS_DWORD blue_bit_mask;
        DDS_DWORD alpha_bit_mask;
    };

    enum HeaderCaps1FlagBits : DDS_DWORD
    {
        DDSCAPS_COMPLEX = 0x8, //
        DDSCAPS_MIPMAP = 0x40'00'00,
        DDSCAPS_TEXTURE = 0x10'00 //
    };

    using HeaderCaps1Flags = std::underlying_type_t<HeaderCaps1FlagBits>;

    enum HeaderCaps2FlagBits : DDS_DWORD
    {
        DDSCAPS2_CUBEMAP = 0x200, //
        DDSCAPS2_CUBEMAP_POSITIVEX = 0x400, //
        DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800, //
        DDSCAPS2_CUBEMAP_POSITIVEY = 0x10'00, //
        DDSCAPS2_CUBEMAP_NEGATIVEY = 0x20'00, //
        DDSCAPS2_CUBEMAP_POSITIVEZ = 0x40'00, //
        DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x80'00, //
        DDSCAPS2_VOLUME = 0x20'00'00
    };

    using HeaderCaps2Flags = std::underlying_type_t<HeaderCaps2FlagBits>;

    enum HeaderCaps3FlagBits : DDS_DWORD
    {
    };

    using HeaderCaps3Flags = std::underlying_type_t<HeaderCaps3FlagBits>;

    enum HeaderCaps4FlagBits : DDS_DWORD
    {
    };

    using HeaderCaps4Flags = std::underlying_type_t<HeaderCaps4FlagBits>;

    constexpr inline DDS_DWORD DDS_MAGIC_NUMBER = MakeFourCC('D', 'D', 'S', ' ');

    struct Header
    {
        DDS_DWORD size;
        HeaderFlags flags;
        DDS_DWORD height;
        DDS_DWORD width;
        DDS_DWORD pitch_or_linear_size;
        DDS_DWORD depth;
        DDS_DWORD mip_map_count;
        DDS_DWORD reserved1[11];
        PixelFormat pixel_format;
        HeaderCaps1Flags caps1;
        HeaderCaps2Flags caps2;
        HeaderCaps3Flags caps3;
        HeaderCaps4Flags caps4;
        DDS_DWORD reserved2;
    };

    enum class DXGIFormat : DDS_DWORD
    {
        DXGI_FORMAT_UNKNOWN = 0,
        DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
        DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
        DXGI_FORMAT_R32G32B32A32_UINT = 3,
        DXGI_FORMAT_R32G32B32A32_SINT = 4,
        DXGI_FORMAT_R32G32B32_TYPELESS = 5,
        DXGI_FORMAT_R32G32B32_FLOAT = 6,
        DXGI_FORMAT_R32G32B32_UINT = 7,
        DXGI_FORMAT_R32G32B32_SINT = 8,
        DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
        DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
        DXGI_FORMAT_R16G16B16A16_UNORM = 11,
        DXGI_FORMAT_R16G16B16A16_UINT = 12,
        DXGI_FORMAT_R16G16B16A16_SNORM = 13,
        DXGI_FORMAT_R16G16B16A16_SINT = 14,
        DXGI_FORMAT_R32G32_TYPELESS = 15,
        DXGI_FORMAT_R32G32_FLOAT = 16,
        DXGI_FORMAT_R32G32_UINT = 17,
        DXGI_FORMAT_R32G32_SINT = 18,
        DXGI_FORMAT_R32G8X24_TYPELESS = 19,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
        DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
        DXGI_FORMAT_R10G10B10A2_UNORM = 24,
        DXGI_FORMAT_R10G10B10A2_UINT = 25,
        DXGI_FORMAT_R11G11B10_FLOAT = 26,
        DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
        DXGI_FORMAT_R8G8B8A8_UNORM = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        DXGI_FORMAT_R8G8B8A8_UINT = 30,
        DXGI_FORMAT_R8G8B8A8_SNORM = 31,
        DXGI_FORMAT_R8G8B8A8_SINT = 32,
        DXGI_FORMAT_R16G16_TYPELESS = 33,
        DXGI_FORMAT_R16G16_FLOAT = 34,
        DXGI_FORMAT_R16G16_UNORM = 35,
        DXGI_FORMAT_R16G16_UINT = 36,
        DXGI_FORMAT_R16G16_SNORM = 37,
        DXGI_FORMAT_R16G16_SINT = 38,
        DXGI_FORMAT_R32_TYPELESS = 39,
        DXGI_FORMAT_D32_FLOAT = 40,
        DXGI_FORMAT_R32_FLOAT = 41,
        DXGI_FORMAT_R32_UINT = 42,
        DXGI_FORMAT_R32_SINT = 43,
        DXGI_FORMAT_R24G8_TYPELESS = 44,
        DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
        DXGI_FORMAT_R8G8_TYPELESS = 48,
        DXGI_FORMAT_R8G8_UNORM = 49,
        DXGI_FORMAT_R8G8_UINT = 50,
        DXGI_FORMAT_R8G8_SNORM = 51,
        DXGI_FORMAT_R8G8_SINT = 52,
        DXGI_FORMAT_R16_TYPELESS = 53,
        DXGI_FORMAT_R16_FLOAT = 54,
        DXGI_FORMAT_D16_UNORM = 55,
        DXGI_FORMAT_R16_UNORM = 56,
        DXGI_FORMAT_R16_UINT = 57,
        DXGI_FORMAT_R16_SNORM = 58,
        DXGI_FORMAT_R16_SINT = 59,
        DXGI_FORMAT_R8_TYPELESS = 60,
        DXGI_FORMAT_R8_UNORM = 61,
        DXGI_FORMAT_R8_UINT = 62,
        DXGI_FORMAT_R8_SNORM = 63,
        DXGI_FORMAT_R8_SINT = 64,
        DXGI_FORMAT_A8_UNORM = 65,
        DXGI_FORMAT_R1_UNORM = 66,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
        DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
        DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
        DXGI_FORMAT_BC1_TYPELESS = 70,
        DXGI_FORMAT_BC1_UNORM = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB = 72,
        DXGI_FORMAT_BC2_TYPELESS = 73,
        DXGI_FORMAT_BC2_UNORM = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB = 75,
        DXGI_FORMAT_BC3_TYPELESS = 76,
        DXGI_FORMAT_BC3_UNORM = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB = 78,
        DXGI_FORMAT_BC4_TYPELESS = 79,
        DXGI_FORMAT_BC4_UNORM = 80,
        DXGI_FORMAT_BC4_SNORM = 81,
        DXGI_FORMAT_BC5_TYPELESS = 82,
        DXGI_FORMAT_BC5_UNORM = 83,
        DXGI_FORMAT_BC5_SNORM = 84,
        DXGI_FORMAT_B5G6R5_UNORM = 85,
        DXGI_FORMAT_B5G5R5A1_UNORM = 86,
        DXGI_FORMAT_B8G8R8A8_UNORM = 87,
        DXGI_FORMAT_B8G8R8X8_UNORM = 88,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
        DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
        DXGI_FORMAT_BC6H_TYPELESS = 94,
        DXGI_FORMAT_BC6H_UF16 = 95,
        DXGI_FORMAT_BC6H_SF16 = 96,
        DXGI_FORMAT_BC7_TYPELESS = 97,
        DXGI_FORMAT_BC7_UNORM = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB = 99,
        DXGI_FORMAT_AYUV = 100,
        DXGI_FORMAT_Y410 = 101,
        DXGI_FORMAT_Y416 = 102,
        DXGI_FORMAT_NV12 = 103,
        DXGI_FORMAT_P010 = 104,
        DXGI_FORMAT_P016 = 105,
        DXGI_FORMAT_420_OPAQUE = 106,
        DXGI_FORMAT_YUY2 = 107,
        DXGI_FORMAT_Y210 = 108,
        DXGI_FORMAT_Y216 = 109,
        DXGI_FORMAT_NV11 = 110,
        DXGI_FORMAT_AI44 = 111,
        DXGI_FORMAT_IA44 = 112,
        DXGI_FORMAT_P8 = 113,
        DXGI_FORMAT_A8P8 = 114,
        DXGI_FORMAT_B4G4R4A4_UNORM = 115,
        DXGI_FORMAT_P208 = 130,
        DXGI_FORMAT_V208 = 131,
        DXGI_FORMAT_V408 = 132,
        DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
        DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
        //DXGI_FORMAT_FORCE_UINT = 0xff'ff'ff'ff
    };

    enum class ResourceDimension
    {
        //UNKNOWN = 0,//unused here
        //BUFFER = 1,
        TEXTURE1D = 2,
        TEXTURE2D = 3,
        TEXTURE3D = 4
    };

    using DDS_UINT = std::uint32_t;

    enum DXT10HeaderMiscFlag1Bits : DDS_UINT
    {
        DDS_RESOURCE_MISC_TEXTURECUBE = 0x4
    };

    using DXT10HeaderMiscFlags1 = std::underlying_type_t<DXT10HeaderMiscFlag1Bits>;

    enum DXT10HeaderMiscFlag2Bits : DDS_UINT
    {
        DDS_ALPHA_MODE_UNKNOWN = 0x0,
        DDS_ALPHA_MODE_STRAIGHT = 0x1,
        DDS_ALPHA_MODE_PREMULTIPLIED = 0x2,
        DDS_ALPHA_MODE_OPAQUE = 0x3,
        DDS_ALPHA_MODE_CUSTOM = 0x4
    };

    using DXT10HeaderMiscFlags2 = std::underlying_type_t<DXT10HeaderMiscFlag2Bits>;

    struct DXT10Header
    {
        DXGIFormat format;
        ResourceDimension resource_dimension;
        DXT10HeaderMiscFlags1 misc_flags1;
        DDS_UINT array_size;
        DXT10HeaderMiscFlags2 misc_flags2;
    };

    constexpr inline std::size_t DDS_DATA_ALIGNMENT = alignof(DDS_DWORD);

    struct ParseResult
    {
        const Header* header;
        const DXT10Header* dxt10_header;
        std::span<const std::uint8_t> image_data;
    };

    hrs::expected<ParseResult, Exception> Parse(std::span<const std::uint8_t> data);

    struct ImageResult
    {
        Render::ImageType image_type;
        Render::Format format;
        Render::Extent3D extent;
        std::uint32_t mip_levels;
        std::uint32_t array_layers;

        std::variant<DXGIFormat, PixelFormatFourCC> original_format;

        std::vector<Render::MemoryImageCopyRegion> regions;
    };

    hrs::expected<ImageResult, Exception> Resolve(const ParseResult& result);
};