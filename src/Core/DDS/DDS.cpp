#include "DDS.h"
#include <stdexcept>
#include <format>
#include <string_view>

namespace DDS
{
    struct format_resolve_result
    {
        Render::Format ctx_format;
    };

    static hrs::expected<format_resolve_result, std::runtime_error> resolve_format(DXGIFormat fmt)
    {
        format_resolve_result res;
        switch(fmt)
        {
            case DXGIFormat::DXGI_FORMAT_R32G32B32A32_TYPELESS:
                res.ctx_format = Render::Format::R32G32B32A32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32A32_FLOAT:
                res.ctx_format = Render::Format::R32G32B32A32_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32A32_UINT:
                res.ctx_format = Render::Format::R32G32B32A32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32A32_SINT:
                res.ctx_format = Render::Format::R32G32B32A32_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32_TYPELESS:
                res.ctx_format = Render::Format::R32G32B32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32_FLOAT:
                res.ctx_format = Render::Format::R32G32B32_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32_UINT:
                res.ctx_format = Render::Format::R32G32B32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32B32_SINT:
                res.ctx_format = Render::Format::R32G32B32A32_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_TYPELESS:
                res.ctx_format = Render::Format::R16G16B16A16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_FLOAT:
                res.ctx_format = Render::Format::R16G16B16A16_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_UNORM:
                res.ctx_format = Render::Format::R16G16B16A16_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_UINT:
                res.ctx_format = Render::Format::R16G16B16A16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_SNORM:
                res.ctx_format = Render::Format::R16G16B16A16_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16B16A16_SINT:
                res.ctx_format = Render::Format::R16G16B16A16_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32_TYPELESS:
                res.ctx_format = Render::Format::R32G32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32_FLOAT:
                res.ctx_format = Render::Format::R32G32_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32_UINT:
                res.ctx_format = Render::Format::R32G32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G32_SINT:
                res.ctx_format = Render::Format::R32G32_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32G8X24_TYPELESS:
                res.ctx_format = Render::Format::D32_FLOAT_S8X24_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                res.ctx_format = Render::Format::D32_FLOAT_S8X24_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
                res.ctx_format = Render::Format::D32_FLOAT_S8X24_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                res.ctx_format = Render::Format::D32_FLOAT_S8X24_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R10G10B10A2_TYPELESS:
                res.ctx_format = Render::Format::D32_FLOAT_S8X24_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R10G10B10A2_UNORM:
                res.ctx_format = Render::Format::R10G10B10A2_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R10G10B10A2_UINT:
                res.ctx_format = Render::Format::R10G10B10A2_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R11G11B10_FLOAT:
                res.ctx_format = Render::Format::R11G11B10_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_TYPELESS:
                res.ctx_format = Render::Format::R8G8B8A8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_UINT:
                res.ctx_format = Render::Format::R8G8B8A8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_SNORM:
                res.ctx_format = Render::Format::R8G8B8A8_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8B8A8_SINT:
                res.ctx_format = Render::Format::R8G8B8A8_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_TYPELESS:
                res.ctx_format = Render::Format::R16G16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_FLOAT:
                res.ctx_format = Render::Format::R16G16_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_UNORM:
                res.ctx_format = Render::Format::R16G16_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_UINT:
                res.ctx_format = Render::Format::R16G16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_SNORM:
                res.ctx_format = Render::Format::R16G16_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16G16_SINT:
                res.ctx_format = Render::Format::R16G16_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32_TYPELESS:
                res.ctx_format = Render::Format::R32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_D32_FLOAT:
                res.ctx_format = Render::Format::D32_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32_FLOAT:
                res.ctx_format = Render::Format::R32_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32_UINT:
                res.ctx_format = Render::Format::R32_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R32_SINT:
                res.ctx_format = Render::Format::R32_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R24G8_TYPELESS:
                res.ctx_format = Render::Format::D24_UNORM_S8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_D24_UNORM_S8_UINT:
                res.ctx_format = Render::Format::D24_UNORM_S8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
                res.ctx_format = Render::Format::D24_UNORM_S8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                res.ctx_format = Render::Format::D24_UNORM_S8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8_TYPELESS:
                res.ctx_format = Render::Format::R8G8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8_UNORM:
                res.ctx_format = Render::Format::R8G8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8_UINT:
                res.ctx_format = Render::Format::R8G8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8_SNORM:
                res.ctx_format = Render::Format::R8G8_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8G8_SINT:
                res.ctx_format = Render::Format::R8G8_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_TYPELESS:
                res.ctx_format = Render::Format::R16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_FLOAT:
                res.ctx_format = Render::Format::R16_FLOAT;
                break;
            case DXGIFormat::DXGI_FORMAT_D16_UNORM:
                res.ctx_format = Render::Format::D16_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_UNORM:
                res.ctx_format = Render::Format::R16_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_UINT:
                res.ctx_format = Render::Format::R16_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_SNORM:
                res.ctx_format = Render::Format::R16_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R16_SINT:
                res.ctx_format = Render::Format::R16_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8_TYPELESS:
                res.ctx_format = Render::Format::R8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8_UNORM:
                res.ctx_format = Render::Format::R8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8_UINT:
                res.ctx_format = Render::Format::R8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R8_SNORM:
                res.ctx_format = Render::Format::R8_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_R8_SINT:
                res.ctx_format = Render::Format::R8_SINT;
                break;
            case DXGIFormat::DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
                res.ctx_format = Render::Format::R9G9B9E5_SHAREDEXP;
                break;
            case DXGIFormat::DXGI_FORMAT_BC1_TYPELESS:
                res.ctx_format = Render::Format::BC1_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC1_UNORM:
                res.ctx_format = Render::Format::BC1_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC1_UNORM_SRGB:
                res.ctx_format = Render::Format::BC1_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_BC2_TYPELESS:
                res.ctx_format = Render::Format::BC2_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC2_UNORM:
                res.ctx_format = Render::Format::BC2_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC2_UNORM_SRGB:
                res.ctx_format = Render::Format::BC2_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_BC3_TYPELESS:
                res.ctx_format = Render::Format::BC3_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC3_UNORM:
                res.ctx_format = Render::Format::BC3_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC3_UNORM_SRGB:
                res.ctx_format = Render::Format::BC3_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_BC4_TYPELESS:
                res.ctx_format = Render::Format::BC4_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC4_UNORM:
                res.ctx_format = Render::Format::BC4_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC4_SNORM:
                res.ctx_format = Render::Format::BC4_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC5_TYPELESS:
                res.ctx_format = Render::Format::BC5_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC5_UNORM:
                res.ctx_format = Render::Format::BC5_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC5_SNORM:
                res.ctx_format = Render::Format::BC5_SNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_B5G6R5_UNORM:
                res.ctx_format = Render::Format::B5G6R5_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_B5G5R5A1_UNORM:
                res.ctx_format = Render::Format::B5G5R5A1_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8A8_TYPELESS:
                res.ctx_format = Render::Format::R8G8B8A8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8X8_TYPELESS:
                res.ctx_format = Render::Format::R8G8B8A8_UINT;
                break;
            case DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                res.ctx_format = Render::Format::R8G8B8A8_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_BC6H_TYPELESS:
                res.ctx_format = Render::Format::BC6H_UF16;
                break;
            case DXGIFormat::DXGI_FORMAT_BC6H_UF16:
                res.ctx_format = Render::Format::BC6H_UF16;
                break;
            case DXGIFormat::DXGI_FORMAT_BC6H_SF16:
                res.ctx_format = Render::Format::BC6H_SF16;
                break;
            case DXGIFormat::DXGI_FORMAT_BC7_TYPELESS:
                res.ctx_format = Render::Format::BC7_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC7_UNORM:
                res.ctx_format = Render::Format::BC7_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_BC7_UNORM_SRGB:
                res.ctx_format = Render::Format::BC7_UNORM_SRGB;
                break;
            case DXGIFormat::DXGI_FORMAT_B4G4R4A4_UNORM:
                res.ctx_format = Render::Format::B4G4R4A4_UNORM;
                break;
            case DXGIFormat::DXGI_FORMAT_UNKNOWN:
            case DXGIFormat::DXGI_FORMAT_A8_UNORM:
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
            default:
                return std::runtime_error(
                    std::format("DXGI format: {} is not supported in graphics context",
                                static_cast<std::underlying_type_t<DXGIFormat>>(fmt)));
                break;
        }

        return res;
    }

    static hrs::expected<format_resolve_result, std::runtime_error>
    resolve_format(const PixelFormat& fmt)
    {
        format_resolve_result res;
        if(fmt.flags & PixelFormatFlagBits::DDPF_FOURCC)
        {
            switch(fmt.four_cc)
            {
                case DDS::PixelFormatFourCC::DXT1:
                    res.ctx_format = Render::Format::BC1_UNORM;
                    break;
                case DDS::PixelFormatFourCC::DXT2:
                    res.ctx_format = Render::Format::BC2_UNORM;
                    break;
                case DDS::PixelFormatFourCC::DXT3:
                    res.ctx_format = Render::Format::BC2_UNORM;
                    break;
                case DDS::PixelFormatFourCC::DXT4:
                    res.ctx_format = Render::Format::BC3_UNORM;
                    break;
                case DDS::PixelFormatFourCC::DXT5:
                    res.ctx_format = Render::Format::BC3_UNORM;
                    break;
                case DDS::PixelFormatFourCC::BC4U:
                    res.ctx_format = Render::Format::BC4_UNORM;
                    break;
                case DDS::PixelFormatFourCC::BC4S:
                    res.ctx_format = Render::Format::BC4_SNORM;
                    break;
                case DDS::PixelFormatFourCC::BC4U_ATI1:
                    res.ctx_format = Render::Format::BC4_UNORM;
                    break;
                case DDS::PixelFormatFourCC::BC5U:
                    res.ctx_format = Render::Format::BC5_UNORM;
                    break;
                case DDS::PixelFormatFourCC::BC5S:
                    res.ctx_format = Render::Format::BC5_SNORM;
                    break;
                case DDS::PixelFormatFourCC::R16G16B16A16_U:
                    res.ctx_format = Render::Format::R16G16B16A16_UNORM;
                    break;
                case DDS::PixelFormatFourCC::R16G16B16A16_S:
                    res.ctx_format = Render::Format::R16G16B16A16_SNORM;
                    break;
                case DDS::PixelFormatFourCC::R16_F:
                    res.ctx_format = Render::Format::R16_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R16G16_F:
                    res.ctx_format = Render::Format::R16G16_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R16G16B16A16_F:
                    res.ctx_format = Render::Format::R16G16B16A16_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R32_F:
                    res.ctx_format = Render::Format::R32_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R32G32_F:
                    res.ctx_format = Render::Format::R32G32_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R32G32B32A32_F:
                    res.ctx_format = Render::Format::R32G32B32A32_FLOAT;
                    break;
                case DDS::PixelFormatFourCC::R8G8_B8G8_U:
                case DDS::PixelFormatFourCC::G8R8_G8B8_U:
                case DDS::PixelFormatFourCC::UYVY:
                case DDS::PixelFormatFourCC::YUY2:
                case DDS::PixelFormatFourCC::CxV8U8:
                default:
                {
                    auto four_cc = ToDwordFourCC(fmt.four_cc);
                    return std::runtime_error(std::format(
                        "FourCC : {} -> {} is not supported in graphics context",
                        std::string_view(reinterpret_cast<const char*>(four_cc.data()),
                                         four_cc.size()),
                        static_cast<std::underlying_type_t<PixelFormatFourCC>>(fmt.four_cc)));
                }
                break;
            }
        }
        else
        {
            //we can parse only R, RG, RGB AND RGBA-like formats due to dwFlags
            //YUV and LUMINANCE - are unsupported

            //Luminance -> one channel
            //Luminance + DDPF_ALPHAPIXELS -> two channels
            //RGB -> three channels
            //RGB + DDPF_ALPHAPIXELS -> four channels
            if(fmt.flags & PixelFormatFlagBits::DDPF_LUMINANCE)
            {
                if(!(fmt.flags & PixelFormatFlagBits::DDPF_ALPHAPIXELS))
                {
                    //one channel
                    if(fmt.rgb_bit_count == 8)
                    {
                        if(fmt.red_bit_mask != 0xFF)
                            return std::runtime_error("Unsupported pixel format");

                        //DXGI_FORMAT_R8_TYPELESS = 60,
                        //DXGI_FORMAT_R8_UNORM = 61,
                        //DXGI_FORMAT_R8_UINT = 62,
                        //DXGI_FORMAT_R8_SNORM = 63,
                        //DXGI_FORMAT_R8_SINT = 64,
                        //DXGI_FORMAT_A8_UNORM = 65,
                        res = resolve_format(DXGIFormat::DXGI_FORMAT_R8_UNORM).value();
                    }
                    else if(fmt.rgb_bit_count == 16)
                    {
                        //DXGI_FORMAT_R16_TYPELESS = 53,
                        //DXGI_FORMAT_R16_FLOAT = 54,
                        //DXGI_FORMAT_R16_UNORM = 56,
                        //DXGI_FORMAT_R16_UINT = 57,
                        //DXGI_FORMAT_R16_SNORM = 58,
                        //DXGI_FORMAT_R16_SINT = 59,

                        if(fmt.red_bit_mask != 0xFF'FF)
                            return std::runtime_error("Unsupported pixel format");

                        res = resolve_format(DXGIFormat::DXGI_FORMAT_R16_UNORM).value();
                    }
                    else if(fmt.rgb_bit_count == 32)
                    {
                        //DXGI_FORMAT_R32_TYPELESS = 39,
                        //DXGI_FORMAT_R32_FLOAT = 41,
                        //DXGI_FORMAT_R32_UINT = 42,
                        //DXGI_FORMAT_R32_SINT = 43,

                        if(fmt.red_bit_mask != 0xFF'FF'FF'FF)
                            return std::runtime_error("Unsupported pixel format");

                        res = resolve_format(DXGIFormat::DXGI_FORMAT_R32_UINT).value();
                    }
                    else
                        return std::runtime_error("Unsupported pixel format");
                }
                else
                {
                    //two channels
                    if(fmt.rgb_bit_count == 16)
                    {
                        //DXGI_FORMAT_R8G8_TYPELESS = 48,
                        //DXGI_FORMAT_R8G8_UNORM = 49,
                        //DXGI_FORMAT_R8G8_UINT = 50,
                        //DXGI_FORMAT_R8G8_SNORM = 51,
                        //DXGI_FORMAT_R8G8_SINT = 52,

                        if(!(fmt.red_bit_mask == 0x00'FF && fmt.alpha_bit_mask == 0xFF'00))
                            return std::runtime_error("Unsupported pixel format");

                        res = resolve_format(DXGIFormat::DXGI_FORMAT_R8G8_UNORM).value();
                    }
                    else if(fmt.rgb_bit_count == 32)
                    {
                        //DXGI_FORMAT_R16G16_TYPELESS = 33,
                        //DXGI_FORMAT_R16G16_FLOAT = 34,
                        //DXGI_FORMAT_R16G16_UNORM = 35,
                        //DXGI_FORMAT_R16G16_UINT = 36,
                        //DXGI_FORMAT_R16G16_SNORM = 37,
                        //DXGI_FORMAT_R16G16_SINT = 38,

                        if(!(fmt.red_bit_mask == 0xFF'FF && fmt.alpha_bit_mask == 0xFF'FF'00'00))
                            return std::runtime_error("Unsupported pixel format");

                        res = resolve_format(DXGIFormat::DXGI_FORMAT_R16G16_UNORM).value();
                    }
                    else
                        return std::runtime_error("Unsupported pixel format");
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
                            return std::runtime_error("Unsupported pixel format");

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
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R11G11B10_FLOAT).value();
                        else if(fmt.red_bit_mask == 0xFF'00'00 && fmt.green_bit_mask == 0xFF'00 &&
                                fmt.blue_bit_mask == 0xFF)
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_B8G8R8X8_UNORM).value();
                        else if(fmt.red_bit_mask == 0x1FF && fmt.green_bit_mask == 0x3'FE'00 &&
                                fmt.blue_bit_mask == 0x7'FC'00'00)
                            res =
                                resolve_format(DXGIFormat::DXGI_FORMAT_R9G9B9E5_SHAREDEXP).value();
                        else
                            return std::runtime_error("Unsupported pixel format");
                    }
                    else
                        return std::runtime_error("Unsupported pixel format");
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
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_B5G5R5A1_UNORM).value();
                        else if(fmt.red_bit_mask == 0xF00 && fmt.green_bit_mask == 0xF0 &&
                                fmt.blue_bit_mask == 0xF && fmt.alpha_bit_mask == 0xF0'00)
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_B4G4R4A4_UNORM).value();
                        else
                            return std::runtime_error("Unsupported pixel format");
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
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R10G10B10A2_UNORM).value();
                        else if(fmt.red_bit_mask == 0xFF && fmt.green_bit_mask == 0xFF'00 &&
                                fmt.blue_bit_mask == 0xFF'00'00 &&
                                fmt.alpha_bit_mask == 0xFF'00'00'00)
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_R8G8B8A8_UNORM).value();
                        else if(fmt.red_bit_mask == 0xFF'00'00 && fmt.green_bit_mask == 0xFF'00 &&
                                fmt.blue_bit_mask == 0xFF && fmt.alpha_bit_mask == 0xFF'00'00'00)
                            res = resolve_format(DXGIFormat::DXGI_FORMAT_B8G8R8A8_UNORM).value();
                        else
                            return std::runtime_error("Unsupported pixel format");
                    }
                    else
                        return std::runtime_error("Unsupported pixel format");
                }
            }
            else
                return std::runtime_error("Unsupported pixel format");
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

    hrs::expected<DDSResult, std::runtime_error> Parse(std::span<const std::uint8_t> data)
    {
        constexpr std::size_t MIN_DDS_SIZE = sizeof(DDS_MAGIC_NUMBER) + sizeof(Header);

        if(data.size() < MIN_DDS_SIZE)
            return std::runtime_error(
                std::format("Size of data must be greater than or equal to MIN_DDS_SIZE({})",
                            MIN_DDS_SIZE));

        if(DDS_MAGIC_NUMBER != MafeFourCC(data[0], data[1], data[2], data[3]))
            return std::runtime_error(
                std::format("Bad magic number. Must be: {}", DDS_MAGIC_NUMBER));

        DDSResult result = {};
        result.header = reinterpret_cast<const Header*>(data.data() + sizeof(DDS_MAGIC_NUMBER));

        //check_dds_header(result.header);

        if(result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_FOURCC &&
           result.header->pixel_format.four_cc == PixelFormatFourCC::DX10)
        {
            constexpr std::size_t MIN_DDS_SIZE_DXT10 =
                sizeof(DDS_MAGIC_NUMBER) + sizeof(Header) + sizeof(DXT10Header);

            if(data.size() < MIN_DDS_SIZE_DXT10)
                return std::runtime_error(std::format(
                    "Size of data must be greater than or equal to MIN_DDS_SIZE_DXT10({})",
                    MIN_DDS_SIZE_DXT10));

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

    hrs::expected<ResolveResult, std::runtime_error> Resolve(const DDSResult& result)
    {
        ResolveResult resolve;

        resolve.image_info.extent.width = result.header->width;
        resolve.image_info.extent.height = result.header->height;
        resolve.image_info.extent.depth = 1;
        resolve.image_info.image_type = Render::ImageType::Image2D;

        if(result.header->flags & HeaderFlagBits::DDSD_DEPTH ||
           result.header->caps3 & HeaderCaps2FlagBits::DDSCAPS2_VOLUME ||
           (result.dxt10_header &&
            result.dxt10_header->resource_dimension == ResourceDimension::TEXTURE3D))
        {
            resolve.image_info.image_type = Render::ImageType::Image3D;
            resolve.image_info.extent.depth = result.header->depth;
        }
        else if(result.dxt10_header)
        {
            if(result.dxt10_header->resource_dimension == ResourceDimension::TEXTURE1D)
                resolve.image_info.image_type = Render::ImageType::Image1D;
        }

        if((result.header->flags & HeaderFlagBits::DDSD_MIPMAPCOUNT ||
            result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_MIPMAP) &&
           result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX)
            resolve.image_info.mip_levels = result.header->mip_map_count;
        else
            resolve.image_info.mip_levels = 1;

        resolve.image_info.samples = Render::SampleCount::SampleCount_1;
        resolve.image_info.array_layers = 1;

        resolve.is_cubemap = false;

        if((result.header->caps2 & HeaderCaps2FlagBits::DDSCAPS2_CUBEMAP &&
            result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX) ||
           (result.dxt10_header && result.dxt10_header->misc_flags1 &
                                       DXT10HeaderMiscFlag1Bits::DDS_RESOURCE_MISC_TEXTURECUBE))
        {
            resolve.is_cubemap = true;
            resolve.image_info.array_layers = 6;
        }

        if(result.dxt10_header)
            resolve.image_info.array_layers *= result.dxt10_header->array_size;

        //format
        //array layers

        auto resolved_format_exp =
            (result.dxt10_header ? resolve_format(result.dxt10_header->format) :
                                   resolve_format(result.header->pixel_format));

        if(!resolved_format_exp)
            return resolved_format_exp.error();

        auto& resolved_format = *resolved_format_exp;

        resolve.regions.reserve(resolve.image_info.array_layers * resolve.image_info.mip_levels);

        resolve.image_info.format = resolved_format.ctx_format;

        bool is_compressed = IsFormatCompressed(resolve.image_info.format);

        const std::uint8_t* sub_image_data_ptr = result.image_data.data();
        for(std::size_t layer = 0; layer < resolve.image_info.array_layers; layer++)
        {
            Render::Extent3D extent = resolve.image_info.extent;

            for(std::size_t mipmap = 0; mipmap < resolve.image_info.mip_levels; mipmap++)
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
                if(IsFormatCompressed(resolve.image_info.format))
                {
                    //sub_image_size = std::max<DDS_DWORD>(1, ((extent.width + 3) / 4)) *
                    //                 extent.height * extent.depth *
                    //                 GetFormatBlockSize(resolve.image_info.format) / 4;

                    sub_image_size = std::max<DDS_DWORD>(1, ((extent.width + 3) / 4)) *
                                     std::max<DDS_DWORD>(1, ((extent.height + 3) / 4)) *
                                     std::max<DDS_DWORD>(1, ((extent.depth + 3) / 4)) *
                                     GetFormatBlockSize(resolve.image_info.format);
                }
                else
                {
                    DDS_DWORD bits_per_pixel = GetFormatBitsPerPixel(resolve.image_info.format);

                    sub_image_size =
                        ((extent.width * bits_per_pixel + 7) / 8) * extent.height * extent.depth;
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