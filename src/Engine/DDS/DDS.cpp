#include "DDS.h"
#include "Core/Render/Format.h"
#include <stdexcept>
#include <format>
#include <optional>
#include <string_view>

namespace Core
{
    namespace DDS
    {
        static std::optional<Render::Format> resolve_format(DXGIFormat format)
        {
            std::optional<Render::Format> res;
            switch(format)
            {
                case DXGIFormat::DXGI_FORMAT_R32G32B32A32_TYPELESS:
                    res = Render::Format::R32G32B32A32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32A32_FLOAT:
                    res = Render::Format::R32G32B32A32_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32A32_UINT:
                    res = Render::Format::R32G32B32A32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32A32_SINT:
                    res = Render::Format::R32G32B32A32_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32_TYPELESS:
                    res = Render::Format::R32G32B32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32_FLOAT:
                    res = Render::Format::R32G32B32_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32_UINT:
                    res = Render::Format::R32G32B32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32B32_SINT:
                    res = Render::Format::R32G32B32A32_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_TYPELESS:
                    res = Render::Format::R16G16B16A16_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_FLOAT:
                    res = Render::Format::R16G16B16A16_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_UNORM:
                    res = Render::Format::R16G16B16A16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_UINT:
                    res = Render::Format::R16G16B16A16_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_SNORM:
                    res = Render::Format::R16G16B16A16_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16B16A16_SINT:
                    res = Render::Format::R16G16B16A16_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32_TYPELESS:
                    res = Render::Format::R32G32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32_FLOAT:
                    res = Render::Format::R32G32_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32_UINT:
                    res = Render::Format::R32G32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G32_SINT:
                    res = Render::Format::R32G32_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32G8X24_TYPELESS:
                    res = Render::Format::D32_SFLOAT_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    res = Render::Format::D32_SFLOAT_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                    res = Render::Format::D32_SFLOAT_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                    res = Render::Format::D32_SFLOAT_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R10G10B10A2_TYPELESS:
                    res = Render::Format::A2B10G10R10_UNORM_PACK32;
                    break;
                case DXGIFormat::DXGI_FORMAT_R10G10B10A2_UNORM:
                    res = Render::Format::A2B10G10R10_UNORM_PACK32;
                    break;
                case DXGIFormat::DXGI_FORMAT_R10G10B10A2_UINT:
                    res = Render::Format::A2B10G10R10_UINT_PACK32;
                    break;
                case DXGIFormat::DXGI_FORMAT_R11G11B10_FLOAT:
                    res = Render::Format::B10G11R11_UFLOAT_PACK32;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_TYPELESS:
                    res = Render::Format::R8G8B8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM:
                    res = Render::Format::R8G8B8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    res = Render::Format::R8G8B8A8_UNORM_SRGB;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UINT:
                    res = Render::Format::R8G8B8A8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_SNORM:
                    res = Render::Format::R8G8B8A8_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8B8A8_SINT:
                    res = Render::Format::R8G8B8A8_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_TYPELESS:
                    res = Render::Format::R16G16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_FLOAT:
                    res = Render::Format::R16G16_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_UNORM:
                    res = Render::Format::R16G16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_UINT:
                    res = Render::Format::R16G16_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_SNORM:
                    res = Render::Format::R16G16_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16G16_SINT:
                    res = Render::Format::R16G16_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32_TYPELESS:
                    res = Render::Format::R32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_D32_FLOAT:
                    res = Render::Format::D32_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32_FLOAT:
                    res = Render::Format::R32_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32_UINT:
                    res = Render::Format::R32_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R32_SINT:
                    res = Render::Format::R32_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R24G8_TYPELESS:
                    res = Render::Format::D24_UNORM_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_D24_UNORM_S8_UINT:
                    res = Render::Format::D24_UNORM_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                    res = Render::Format::D24_UNORM_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                    res = Render::Format::D24_UNORM_S8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8_TYPELESS:
                    res = Render::Format::R8G8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8_UNORM:
                    res = Render::Format::R8G8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8_UINT:
                    res = Render::Format::R8G8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8_SNORM:
                    res = Render::Format::R8G8_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8G8_SINT:
                    res = Render::Format::R8G8_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_TYPELESS:
                    res = Render::Format::R16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_FLOAT:
                    res = Render::Format::R16_SFLOAT;
                    break;
                case DXGIFormat::DXGI_FORMAT_D16_UNORM:
                    res = Render::Format::D16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_UNORM:
                    res = Render::Format::R16_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_UINT:
                    res = Render::Format::R16_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_SNORM:
                    res = Render::Format::R16_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R16_SINT:
                    res = Render::Format::R16_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8_TYPELESS:
                    res = Render::Format::R8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8_UNORM:
                    res = Render::Format::R8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8_UINT:
                    res = Render::Format::R8_UINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8_SNORM:
                    res = Render::Format::R8_SNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R8_SINT:
                    res = Render::Format::R8_SINT;
                    break;
                case DXGIFormat::DXGI_FORMAT_A8_UNORM:
                    res = Render::Format::A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
                    res = Render::Format::E5B9G9R9_UFLOAT_PACK32;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC1_TYPELESS:
                    res = Render::Format::BC1_RGBA_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC1_UNORM:
                    res = Render::Format::BC1_RGBA_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC1_UNORM_SRGB:
                    res = Render::Format::BC1_RGBA_UNORM_SRGB_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC2_TYPELESS:
                    res = Render::Format::BC2_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC2_UNORM:
                    res = Render::Format::BC2_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC2_UNORM_SRGB:
                    res = Render::Format::BC2_UNORM_SRGB_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC3_TYPELESS:
                    res = Render::Format::BC3_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC3_UNORM:
                    res = Render::Format::BC3_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC3_UNORM_SRGB:
                    res = Render::Format::BC3_UNORM_SRGB_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC4_TYPELESS:
                    res = Render::Format::BC4_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC4_UNORM:
                    res = Render::Format::BC4_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC4_SNORM:
                    res = Render::Format::BC4_SNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC5_TYPELESS:
                    res = Render::Format::BC5_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC5_UNORM:
                    res = Render::Format::BC5_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC5_SNORM:
                    res = Render::Format::BC5_SNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_B5G6R5_UNORM:
                    res = Render::Format::R5G6B5_UNORM_PACK16;
                    break;
                case DXGIFormat::DXGI_FORMAT_B5G5R5A1_UNORM:
                    res = Render::Format::A1R5G5B5_UNORM_PACK16;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM:
                    res = Render::Format::B8G8R8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM:
                    res = Render::Format::B8G8R8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8A8_TYPELESS:
                    res = Render::Format::B8G8R8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    res = Render::Format::B8G8R8A8_UNORM_SRGB;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8X8_TYPELESS:
                    res = Render::Format::B8G8R8A8_UNORM;
                    break;
                case DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                    res = Render::Format::B8G8R8A8_UNORM_SRGB;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC6H_TYPELESS:
                    res = Render::Format::BC6H_UFLOAT_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC6H_UF16:
                    res = Render::Format::BC6H_UFLOAT_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC6H_SF16:
                    res = Render::Format::BC6H_SFLOAT_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC7_TYPELESS:
                    res = Render::Format::BC7_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC7_UNORM:
                    res = Render::Format::BC7_UNORM_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_BC7_UNORM_SRGB:
                    res = Render::Format::BC7_UNORM_SRGB_BLOCK;
                    break;
                case DXGIFormat::DXGI_FORMAT_B4G4R4A4_UNORM:
                    res = Render::Format::A4R4G4B4_UNORM_PACK16;
                    break;
                case DXGIFormat::DXGI_FORMAT_UNKNOWN:
                case DXGIFormat::DXGI_FORMAT_R1_UNORM:
                case DXGIFormat::DXGI_FORMAT_R8G8_B8G8_UNORM:
                case DXGIFormat::DXGI_FORMAT_G8R8_G8B8_UNORM:
                case DXGIFormat::DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
                case DXGIFormat::DXGI_FORMAT_AYUV:
                case DXGIFormat::DXGI_FORMAT_Y410:
                case DXGIFormat::DXGI_FORMAT_Y416:
                case DXGIFormat::DXGI_FORMAT_NV12:
                case DXGIFormat::DXGI_FORMAT_P010:
                case DXGIFormat::DXGI_FORMAT_P016:
                case DXGIFormat::DXGI_FORMAT_420_OPAQUE:
                case DXGIFormat::DXGI_FORMAT_YUY2:
                case DXGIFormat::DXGI_FORMAT_Y210:
                case DXGIFormat::DXGI_FORMAT_Y216:
                case DXGIFormat::DXGI_FORMAT_NV11:
                case DXGIFormat::DXGI_FORMAT_AI44:
                case DXGIFormat::DXGI_FORMAT_IA44:
                case DXGIFormat::DXGI_FORMAT_P8:
                case DXGIFormat::DXGI_FORMAT_A8P8:
                case DXGIFormat::DXGI_FORMAT_P208:
                case DXGIFormat::DXGI_FORMAT_V208:
                case DXGIFormat::DXGI_FORMAT_V408:
                case DXGIFormat::DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
                case DXGIFormat::DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
                    break;
                default:
                    break;
            }

            return res;
        }

        struct PixelFormatResult
        {
            Render::Format format;
            OriginalFormat originl_format;
        };

        static Core::Expected<PixelFormatResult, Result> resolve_format(const PixelFormat& fmt)
        {
            Core::Expected<PixelFormatResult, Result> res;
            if(fmt.flags & PixelFormatFlagBits::DDPF_FOURCC)
            {
                switch(fmt.four_cc)
                {
                    case PixelFormatFourCC::DXT1:
                        res = PixelFormatResult{.format = Render::Format::BC1_RGB_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::DXT1};
                        break;
                    case PixelFormatFourCC::DXT2:
                        res = PixelFormatResult{.format = Render::Format::BC2_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::DXT2};
                        break;
                    case PixelFormatFourCC::DXT3:
                        res = PixelFormatResult{.format = Render::Format::BC2_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::DXT3};
                        break;
                    case PixelFormatFourCC::DXT4:
                        res = PixelFormatResult{.format = Render::Format::BC3_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::DXT4};
                        break;
                    case PixelFormatFourCC::DXT5:
                        res = PixelFormatResult{.format = Render::Format::BC3_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::DXT5};
                        break;
                    case PixelFormatFourCC::BC4U:
                        res = PixelFormatResult{.format = Render::Format::BC4_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::BC4U};
                        break;
                    case PixelFormatFourCC::BC4S:
                        res = PixelFormatResult{.format = Render::Format::BC4_SNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::BC4S};
                        break;
                    case PixelFormatFourCC::BC4U_ATI1:
                        res = PixelFormatResult{.format = Render::Format::BC4_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::BC4U_ATI1};
                        break;
                    case PixelFormatFourCC::BC5U:
                        res = PixelFormatResult{.format = Render::Format::BC5_UNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::BC5U};
                        break;
                    case PixelFormatFourCC::BC5S:
                        res = PixelFormatResult{.format = Render::Format::BC5_SNORM_BLOCK,
                                                .originl_format = PixelFormatFourCC::BC5S};
                        break;
                    case PixelFormatFourCC::R16G16B16A16_U:
                        res =
                            PixelFormatResult{.format = Render::Format::R16G16B16A16_UNORM,
                                              .originl_format = PixelFormatFourCC::R16G16B16A16_U};
                        break;
                    case PixelFormatFourCC::R16G16B16A16_S:
                        res =
                            PixelFormatResult{.format = Render::Format::R16G16B16A16_SNORM,
                                              .originl_format = PixelFormatFourCC::R16G16B16A16_S};
                        break;
                    case PixelFormatFourCC::R16_F:
                        res = PixelFormatResult{.format = Render::Format::R16_SFLOAT,
                                                .originl_format = PixelFormatFourCC::R16_F};
                        break;
                    case PixelFormatFourCC::R16G16_F:
                        res = PixelFormatResult{.format = Render::Format::R16G16_SFLOAT,
                                                .originl_format = PixelFormatFourCC::R16G16_F};
                        break;
                    case PixelFormatFourCC::R16G16B16A16_F:
                        res =
                            PixelFormatResult{.format = Render::Format::R16G16B16A16_SFLOAT,
                                              .originl_format = PixelFormatFourCC::R16G16B16A16_F};
                        break;
                    case PixelFormatFourCC::R32_F:
                        res = PixelFormatResult{.format = Render::Format::R32_SFLOAT,
                                                .originl_format = PixelFormatFourCC::R32_F};
                        break;
                    case PixelFormatFourCC::R32G32_F:
                        res = PixelFormatResult{.format = Render::Format::R32G32_SFLOAT,
                                                .originl_format = PixelFormatFourCC::R32G32_F};
                        break;
                    case PixelFormatFourCC::R32G32B32A32_F:
                        res =
                            PixelFormatResult{.format = Render::Format::R32G32B32A32_SFLOAT,
                                              .originl_format = PixelFormatFourCC::R32G32B32A32_F};
                        break;
                    case PixelFormatFourCC::R8G8_B8G8_U:
                    case PixelFormatFourCC::G8R8_G8B8_U:
                    case PixelFormatFourCC::UYVY:
                    case PixelFormatFourCC::YUY2:
                    case PixelFormatFourCC::CxV8U8:
                    default:
                        res = Result::UnsupportedFourCC;
                        break;
                }
            }
            else
            {
                //we can parse only R, RG, RGB AND RGBA-like formats due to dwFlags
                //YUV - not unsupported

                //Luminance -> one channel
                //Luminance + DDPF_ALPHAPIXELS -> two channels
                //RGB -> three channels
                //RGB + DDPF_ALPHAPIXELS -> four channels
                if(fmt.flags & PixelFormatFlagBits::DDPF_LUMINANCE)
                {
                    if(!(fmt.flags & PixelFormatFlagBits::DDPF_ALPHAPIXELS))
                    {
                        //one channel
                        if(fmt.rgb_bit_count == 8 && fmt.red_bit_mask == 0xFF)
                        {
                            //DXGI_FORMAT_R8_TYPELESS = 60,
                            //DXGI_FORMAT_R8_UNORM = 61,
                            //DXGI_FORMAT_R8_UINT = 62,
                            //DXGI_FORMAT_R8_SNORM = 63,
                            //DXGI_FORMAT_R8_SINT = 64,
                            //DXGI_FORMAT_A8_UNORM = 65,
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R8_UNORM).value();
                        }
                        else if(fmt.rgb_bit_count == 16 && fmt.red_bit_mask == 0xFF'FF)
                        {
                            //DXGI_FORMAT_R16_TYPELESS = 53,
                            //DXGI_FORMAT_R16_FLOAT = 54,
                            //DXGI_FORMAT_R16_UNORM = 56,
                            //DXGI_FORMAT_R16_UINT = 57,
                            //DXGI_FORMAT_R16_SNORM = 58,
                            //DXGI_FORMAT_R16_SINT = 59,
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R16_UNORM).value();
                        }
                        else if(fmt.rgb_bit_count == 32 && fmt.red_bit_mask == 0xFF'FF'FF'FF)
                        {
                            //DXGI_FORMAT_R32_TYPELESS = 39,
                            //DXGI_FORMAT_R32_FLOAT = 41,
                            //DXGI_FORMAT_R32_UINT = 42,
                            //DXGI_FORMAT_R32_SINT = 43,
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R32_UINT).value();
                        }
                        else
                            res = Result::UnsupportedPixelFormat;
                    }
                    else
                    {
                        //two channels
                        if(fmt.rgb_bit_count == 16 && fmt.red_bit_mask == 0x00'FF &&
                           fmt.alpha_bit_mask == 0xFF'00)
                        {
                            //DXGI_FORMAT_R8G8_TYPELESS = 48,
                            //DXGI_FORMAT_R8G8_UNORM = 49,
                            //DXGI_FORMAT_R8G8_UINT = 50,
                            //DXGI_FORMAT_R8G8_SNORM = 51,
                            //DXGI_FORMAT_R8G8_SINT = 52,
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R8G8_UNORM).value();
                        }
                        else if(fmt.rgb_bit_count == 32 && fmt.red_bit_mask == 0xFF'FF &&
                                fmt.alpha_bit_mask == 0xFF'FF'00'00)
                        {
                            //DXGI_FORMAT_R16G16_TYPELESS = 33,
                            //DXGI_FORMAT_R16G16_FLOAT = 34,
                            //DXGI_FORMAT_R16G16_UNORM = 35,
                            //DXGI_FORMAT_R16G16_UINT = 36,
                            //DXGI_FORMAT_R16G16_SNORM = 37,
                            //DXGI_FORMAT_R16G16_SINT = 38,
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R16G16_UNORM).value();
                        }
                        else
                            res = Result::UnsupportedPixelFormat;
                    }
                }
                else if(fmt.flags & PixelFormatFlagBits::DDPF_RGB)
                {
                    if(!(fmt.flags & PixelFormatFlagBits::DDPF_ALPHAPIXELS))
                    {
                        //three channels
                        if(fmt.rgb_bit_count == 16)
                        {
                            //DXGI_FORMAT_B5G6R5_UNORM = 85

                            if(!(fmt.red_bit_mask == 0xF8'00 && fmt.green_bit_mask == 0x7E0 &&
                                 fmt.blue_bit_mask == 0x1F))
                                res = Result::UnsupportedPixelFormat;

                            res = resolve_format(DXGIFormat::DXGI_FORMAT_B5G6R5_UNORM).value();
                        }
                        else if(fmt.rgb_bit_count == 32)
                        {
                            //DXGI_FORMAT_R11G11B10_FLOAT = 26

                            //DXGI_FORMAT_B8G8R8X8_UNORM = 88,
                            //DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
                            //DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,

                            //DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,

                            if(fmt.red_bit_mask == 0x7FF && fmt.green_bit_mask == 0x3F'F8'00 &&
                               fmt.blue_bit_mask == 0xFF'C0'00'00)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_R11G11B10_FLOAT).value();
                            else if(fmt.red_bit_mask == 0xFF'00'00 &&
                                    fmt.green_bit_mask == 0xFF'00 && fmt.blue_bit_mask == 0xFF)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM).value();
                            else if(fmt.red_bit_mask == 0x1FF && fmt.green_bit_mask == 0x3'FE'00 &&
                                    fmt.blue_bit_mask == 0x7'FC'00'00)
                                res = resolve_format(DXGIFormat::DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
                                          .value();
                            else
                                res = Result::UnsupportedPixelFormat;
                        }
                        else
                            res = Result::UnsupportedPixelFormat;
                    }
                    else
                    {
                        //four channels
                        if(fmt.rgb_bit_count == 16)
                        {
                            //DXGI_FORMAT_B5G5R5A1_UNORM = 86

                            //DXGI_FORMAT_B4G4R4A4_UNORM = 115

                            if(fmt.red_bit_mask == 0x7C'00 && fmt.green_bit_mask == 0x3E0 &&
                               fmt.blue_bit_mask == 0x1F)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_B5G5R5A1_UNORM).value();
                            else if(fmt.red_bit_mask == 0xF00 && fmt.green_bit_mask == 0xF0 &&
                                    fmt.blue_bit_mask == 0xF && fmt.alpha_bit_mask == 0xF0'00)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_B4G4R4A4_UNORM).value();
                            else
                                res = Result::UnsupportedPixelFormat;
                        }
                        else if(fmt.rgb_bit_count == 32)
                        {
                            //DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
                            //DXGI_FORMAT_R10G10B10A2_UNORM = 24,
                            //DXGI_FORMAT_R10G10B10A2_UINT = 25,

                            //DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
                            //DXGI_FORMAT_R8G8B8A8_UNORM = 28,
                            //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
                            //DXGI_FORMAT_R8G8B8A8_UINT = 30,
                            //DXGI_FORMAT_R8G8B8A8_SNORM = 31,
                            //DXGI_FORMAT_R8G8B8A8_SINT = 32,

                            //DXGI_FORMAT_B8G8R8A8_UNORM = 87,
                            //DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
                            //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,

                            if(fmt.red_bit_mask == 0x3FF && fmt.green_bit_mask == 0xF'FC'00 &&
                               fmt.blue_bit_mask == 0x3F'F0'00'00)
                                res = resolve_format(DXGIFormat::DXGI_FORMAT_R10G10B10A2_UNORM)
                                          .value();
                            else if(fmt.red_bit_mask == 0xFF && fmt.green_bit_mask == 0xFF'00 &&
                                    fmt.blue_bit_mask == 0xFF'00'00 &&
                                    fmt.alpha_bit_mask == 0xFF'00'00'00)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM).value();
                            else if(fmt.red_bit_mask == 0xFF'00'00 &&
                                    fmt.green_bit_mask == 0xFF'00 && fmt.blue_bit_mask == 0xFF &&
                                    fmt.alpha_bit_mask == 0xFF'00'00'00)
                                res =
                                    resolve_format(DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM).value();
                            else
                                res = Result::UnsupportedPixelFormat;
                        }
                        else
                            res = Result::UnsupportedPixelFormat;
                    }
                }
                else
                    res = Result::UnsupportedPixelFormat;
            }
            return res;
        }

#pragma message("TODO!!!")
        /*static void check_dds_header(const Header* header)
    {
        constexpr std::size_t HEADER_STRUCT_SIZE = 124;
        if(header->size != 124)
            throw std::runtime_error(
                std::format("header->size must be equal to {}", HEADER_STRUCT_SIZE));

        if(!((header->flags & HeaderFlagBits::DDSD_CAPS) &&
             (header->flags & HeaderFlagBits::DDSD_HEIGHT) &&
             (header->flags & HeaderFlagBits::DDSD_WIDTH) &&
             (header->flags & HeaderFlagBits::DDSD_PIXELFORMAT)))
        {
            throw std::runtime_error(
                "header->flags must has all required flags: DDSD_CAPS | DDSD_HEIGHT | "
                "DDSD_WIDTH | DDSD_PIXELFORMAT");
        }

        constexpr std::size_t PIXEL_FORMAT_STRUCT_SIZE = 32;
        if(header->pixel_format.size != PIXEL_FORMAT_STRUCT_SIZE)
            throw std::runtime_error(std::format("header->pixel_format.size must be equal to {}",
                                                 PIXEL_FORMAT_STRUCT_SIZE));
    }*/

        //static void check_dds_dxt10_header(const DXT10Header* header)
        //{}

        Exception::Exception(Result _result, std::string_view _message)
            : result(_result),
              message(_message)
        {}

        Exception::Exception(Result _result, std::string&& _message) noexcept
            : result(_result),
              message(std::move(_message))
        {}

        const char* Exception::what() const noexcept
        {
            return message.c_str();
        }

        Result Exception::GetResult() const noexcept
        {
            return result;
        }

        const std::string& Exception::GetMessage() const noexcept
        {
            return message;
        }

        Core::Expected<ParseResult, Exception> Parse(std::span<const std::uint8_t> data)
        {
            constexpr std::size_t MIN_DDS_SIZE = sizeof(DDS_MAGIC_NUMBER) + sizeof(Header);

            if(data.size() < MIN_DDS_SIZE)
                return Exception(
                    Result::BadData,
                    std::format("Size of data must be greater than or equal to MIN_DDS_SIZE({})",
                                MIN_DDS_SIZE));

            if(DDS_MAGIC_NUMBER != MakeFourCC(data[0], data[1], data[2], data[3]))
                return Exception(Result::BadData,
                                 std::format("Bad magic number. Must be: {}", DDS_MAGIC_NUMBER));

            ParseResult result = {};
            result.header = reinterpret_cast<const Header*>(data.data() + sizeof(DDS_MAGIC_NUMBER));

            //check_dds_header(result.header);

            if(result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_FOURCC &&
               result.header->pixel_format.four_cc == PixelFormatFourCC::DX10)
            {
                constexpr std::size_t MIN_DDS_SIZE_DXT10 =
                    sizeof(DDS_MAGIC_NUMBER) + sizeof(Header) + sizeof(DXT10Header);

                if(data.size() < MIN_DDS_SIZE_DXT10)
                    return Exception(
                        Result::BadData,
                        (std::format(
                            "Size of data must be greater than or equal to MIN_DDS_SIZE_DXT10({})",
                            MIN_DDS_SIZE_DXT10)));

                result.dxt10_header = reinterpret_cast<const DXT10Header*>(
                    data.data() + sizeof(DDS_MAGIC_NUMBER) + sizeof(Header));

                //check_dds_dxt10_header(result.dxt10_header);

                result.image_data = data.subspan(MIN_DDS_SIZE_DXT10);
            }
            else
            {
                result.image_data = data.subspan(MIN_DDS_SIZE);
            }

            return result;
        }

        Core::Expected<ImageResult, Exception> Resolve(const ParseResult& result)
        {
            //#error CORRECT HANDLING FOR CUBEMAPS AND THEIR ARRAY LAYER COUNT!

            ImageResult resolve;

            resolve.extent.width = result.header->width;
            resolve.extent.height = result.header->height;
            resolve.extent.depth = 1;
            resolve.image_type = Render::ImageType::Image2D;

            if(result.header->flags & HeaderFlagBits::DDSD_DEPTH ||
               result.header->caps3 & HeaderCaps2FlagBits::DDSCAPS2_VOLUME ||
               (result.dxt10_header &&
                result.dxt10_header->resource_dimension == ResourceDimension::TEXTURE3D))
            {
                resolve.image_type = Render::ImageType::Image3D;
                resolve.extent.depth = result.header->depth;
            }
            else if(result.dxt10_header)
            {
                if(result.dxt10_header->resource_dimension == ResourceDimension::TEXTURE1D)
                    resolve.image_type = Render::ImageType::Image1D;
            }

            if((result.header->flags & HeaderFlagBits::DDSD_MIPMAPCOUNT ||
                result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_MIPMAP) &&
               result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX)
                resolve.mip_levels = result.header->mip_map_count;
            else
                resolve.mip_levels = 1;

            resolve.array_layers = 1;

            resolve.cubemap_compatible = false;
            if((result.header->caps2 & HeaderCaps2FlagBits::DDSCAPS2_CUBEMAP &&
                result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX) ||
               (result.dxt10_header && result.dxt10_header->misc_flags1 &
                                           DXT10HeaderMiscFlag1Bits::DDS_RESOURCE_MISC_TEXTURECUBE))
            {
                resolve.cubemap_compatible = true;
                resolve.array_layers = 6;
            }

            if(result.dxt10_header)
                resolve.array_layers *= result.dxt10_header->array_size;

            //format
            //array layers

            if(result.dxt10_header)
            {
                auto format_opt = resolve_format(result.dxt10_header->format);
                if(!format_opt.has_value())
                    return Exception(
                        Result::UnsupportedDXGIFormat,
                        std::format("Unsupported DXGI format: {}",
                                    static_cast<DDS_DWORD>(result.dxt10_header->format)));

                resolve.format = format_opt.value();
                resolve.original_format = result.dxt10_header->format;
            }
            else
            {
                auto format_exp = resolve_format(result.header->pixel_format);
                if(!format_exp.HasValue())
                {
                    std::string error_message;
                    switch(format_exp.Error())
                    {
                        case Result::UnsupportedFourCC:
                        {
                            auto sparsed = SparseFourCC(
                                static_cast<DDS_DWORD>(result.header->pixel_format.four_cc));
                            error_message = std::format("Unsupported FourCC: {}{}{}{}",
                                                        +sparsed[0],
                                                        +sparsed[1],
                                                        +sparsed[2],
                                                        +sparsed[3]);
                        }
                        break;
                        case Result::UnsupportedPixelFormat:
                            error_message = "Unsupported pixel format";
                            break;
                        default:
                            error_message = "Unknown error";
                            break;
                    }

                    return Exception(format_exp.Error(), std::move(error_message));
                }

                resolve.format = format_exp->format;
                resolve.original_format = format_exp->originl_format;
            }

            resolve.regions.reserve(resolve.array_layers * resolve.mip_levels);

            bool is_compressed = Render::IsFormatCompressed(resolve.format);

            const std::uint8_t* sub_image_data_ptr = result.image_data.data();
            for(std::size_t layer = 0; layer < resolve.array_layers; layer++)
            {
                Render::Extent3D extent = resolve.extent;

                for(std::size_t mipmap = 0; mipmap < resolve.mip_levels; mipmap++)
                {
                    if(mipmap != 0)
                    {
                        extent.width >>= static_cast<int>(extent.width != 1);
                        extent.height >>= static_cast<int>(extent.height != 1);
                        extent.depth >>= static_cast<int>(extent.depth != 1);
                    }

                    //we do not have R8G8_B8G8, G8R8_G8B8, legacy UYVY-packed, and legacy YUY2-packed formats
                    //so we do not care about them
                    DDS_DWORD sub_image_size;
                    if(is_compressed)
                    {
                        //sub_image_size = std::max<DDS_DWORD>(1, ((extent.width + 3) / 4)) *
                        //                 extent.height * extent.depth *
                        //                 GetFormatBlockSize(resolve.image_info.format) / 4;

                        sub_image_size = std::max<DDS_DWORD>(1, ((extent.width + 3) / 4)) *
                                         std::max<DDS_DWORD>(1, ((extent.height + 3) / 4)) *
                                         std::max<DDS_DWORD>(1, ((extent.depth + 3) / 4)) *
                                         GetFormatBlockSize(resolve.format);
                    }
                    else
                    {
                        DDS_DWORD bits_per_pixel = GetFormatBitsPerPixel(resolve.format);

                        sub_image_size = ((extent.width * bits_per_pixel + 7) / 8) * extent.height *
                                         extent.depth;
                    }

                    resolve.regions.push_back(Render::MemoryImageCopyRegion{
                        .data = sub_image_data_ptr,
                        .buffer_row_length = extent.width,
                        .buffer_image_height = extent.height,
                        .subresource_layers =
                            Render::ImageSubresourceLayers{
                                .mip_level = static_cast<std::uint32_t>(mipmap),
                                .base_layer = static_cast<std::uint32_t>(layer),
                                .layer_count = 1},
                        .offset = Render::Offset3D{.x = 0, .y = 0, .z = 0},
                        .extent = extent});

                    sub_image_data_ptr += sub_image_size;
                }
            }

            return resolve;
        }
    };
};