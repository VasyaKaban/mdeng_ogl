#include "Format.h"

namespace Render
{
    bool IsFormatCompressed(Format format) noexcept
    {
        return IsBCFormat(format) || IsETC2Format(format) || IsEACFormat(format);
    }

    bool IsBCFormat(Format format) noexcept
    {
        bool is_compressed;

        switch(format)
        {
            case Format::BC1_RGB_UNORM_BLOCK:
            case Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Format::BC1_RGBA_UNORM_BLOCK:
            case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Format::BC2_UNORM_BLOCK:
            case Format::BC2_UNORM_SRGB_BLOCK:
            case Format::BC3_UNORM_BLOCK:
            case Format::BC3_UNORM_SRGB_BLOCK:
            case Format::BC4_UNORM_BLOCK:
            case Format::BC4_SNORM_BLOCK:
            case Format::BC5_UNORM_BLOCK:
            case Format::BC5_SNORM_BLOCK:
            case Format::BC6H_UFLOAT_BLOCK:
            case Format::BC6H_SFLOAT_BLOCK:
            case Format::BC7_UNORM_BLOCK:
            case Format::BC7_UNORM_SRGB_BLOCK:
                is_compressed = true;
                break;
            default:
                is_compressed = false;
                break;
        }

        return is_compressed;
    }

    bool IsETC2Format(Format format) noexcept
    {
        bool is_compressed;

        switch(format)
        {
            case Format::ETC2_R8G8B8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
                is_compressed = true;
                break;
            default:
                is_compressed = false;
                break;
        }

        return is_compressed;
    }

    bool IsEACFormat(Format format) noexcept
    {
        bool is_compressed;

        switch(format)
        {
            case Format::EAC_R11_UNORM_BLOCK:
            case Format::EAC_R11_SNORM_BLOCK:
            case Format::EAC_R11G11_UNORM_BLOCK:
            case Format::EAC_R11G11_SNORM_BLOCK:
                is_compressed = true;
                break;
            default:
                is_compressed = false;
                break;
        }

        return is_compressed;
    }

    bool IsDepthFormat(Format format) noexcept
    {
        bool is_depth;
        switch(format)
        {
            case Format::D16_UNORM:
            case Format::X8_D24_UNORM_PACK32:
            case Format::D32_SFLOAT:
                is_depth = true;
                break;
            default:
                is_depth = false;
                break;
        }

        return is_depth;
    }

    bool IsStencilFormat(Format format) noexcept
    {
        bool is_stencil;
        switch(format)
        {
            case Format::S8_UINT:
                is_stencil = true;
                break;
            default:
                is_stencil = false;
                break;
        }

        return is_stencil;
    }

    bool IsDepthStencilFormat(Format format) noexcept
    {
        bool is_depth_stencil;
        switch(format)
        {
            case Format::D16_UNORM_S8_UINT:
            case Format::D24_UNORM_S8_UINT:
            case Format::D32_SFLOAT_S8_UINT:
                is_depth_stencil = true;
                break;
            default:
                is_depth_stencil = false;
                break;
        }

        return is_depth_stencil;
    }

    bool IsColorFormat(Format format) noexcept
    {
        return !(format != Format::UNDEFINED || IsDepthFormat(format) ||
                 IsDepthStencilFormat(format) || IsStencilFormat(format));
    }

    std::uint8_t GetFormatBlockSize(Format format) noexcept
    {
        std::uint8_t block_size;

        switch(format)
        {
            case Format::BC1_RGB_UNORM_BLOCK:
            case Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Format::BC1_RGBA_UNORM_BLOCK:
            case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Format::BC4_UNORM_BLOCK:
            case Format::BC4_SNORM_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Format::EAC_R11_UNORM_BLOCK:
            case Format::EAC_R11_SNORM_BLOCK:
                block_size = 8;
                break;
            case Format::BC2_UNORM_BLOCK:
            case Format::BC2_UNORM_SRGB_BLOCK:
            case Format::BC3_UNORM_BLOCK:
            case Format::BC3_UNORM_SRGB_BLOCK:
            case Format::BC5_UNORM_BLOCK:
            case Format::BC5_SNORM_BLOCK:
            case Format::BC6H_UFLOAT_BLOCK:
            case Format::BC6H_SFLOAT_BLOCK:
            case Format::BC7_UNORM_BLOCK:
            case Format::BC7_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
            case Format::EAC_R11G11_UNORM_BLOCK:
            case Format::EAC_R11G11_SNORM_BLOCK:
                block_size = 16;
                break;
            default:
                block_size = 0;
                break;
        }

        return block_size;
    }

    std::uint16_t GetFormatBitsPerPixel(Format format) noexcept
    {
        std::uint16_t bits;
        switch(format)
        {
            case Format::R4G4_UNORM_PACK8:
            case Format::R8_UNORM:
            case Format::R8_SNORM:
            case Format::R8_USCALED:
            case Format::R8_SSCALED:
            case Format::R8_UINT:
            case Format::R8_SINT:
            case Format::R8_UNORM_SRGB:
            case Format::S8_UINT:
            case Format::A8_UNORM:
                bits = 8;
                break;
            case Format::R4G4B4A4_UNORM_PACK16:
            case Format::B4G4R4A4_UNORM_PACK16:
            case Format::R5G6B5_UNORM_PACK16:
            case Format::B5G6R5_UNORM_PACK16:
            case Format::R5G5B5A1_UNORM_PACK16:
            case Format::B5G5R5A1_UNORM_PACK16:
            case Format::A1R5G5B5_UNORM_PACK16:
            case Format::R8G8_UNORM:
            case Format::R8G8_SNORM:
            case Format::R8G8_USCALED:
            case Format::R8G8_SSCALED:
            case Format::R8G8_UINT:
            case Format::R8G8_SINT:
            case Format::R8G8_UNORM_SRGB:
            case Format::R16_UNORM:
            case Format::R16_SNORM:
            case Format::R16_USCALED:
            case Format::R16_SSCALED:
            case Format::R16_UINT:
            case Format::R16_SINT:
            case Format::R16_SFLOAT:
            case Format::D16_UNORM:
            case Format::A4R4G4B4_UNORM_PACK16:
            case Format::A4B4G4R4_UNORM_PACK16:
            case Format::A1B5G5R5_UNORM_PACK16:
                bits = 16;
                break;
            case Format::R8G8B8_UNORM:
            case Format::R8G8B8_SNORM:
            case Format::R8G8B8_USCALED:
            case Format::R8G8B8_SSCALED:
            case Format::R8G8B8_UINT:
            case Format::R8G8B8_SINT:
            case Format::R8G8B8_UNORM_SRGB:
            case Format::B8G8R8_UNORM:
            case Format::B8G8R8_SNORM:
            case Format::B8G8R8_USCALED:
            case Format::B8G8R8_SSCALED:
            case Format::B8G8R8_UINT:
            case Format::B8G8R8_SINT:
            case Format::B8G8R8_UNORM_SRGB:
            case Format::D16_UNORM_S8_UINT:
                bits = 24;
                break;
            case Format::R8G8B8A8_UNORM:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_USCALED:
            case Format::R8G8B8A8_SSCALED:
            case Format::R8G8B8A8_UINT:
            case Format::R8G8B8A8_SINT:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::B8G8R8A8_UNORM:
            case Format::B8G8R8A8_SNORM:
            case Format::B8G8R8A8_USCALED:
            case Format::B8G8R8A8_SSCALED:
            case Format::B8G8R8A8_UINT:
            case Format::B8G8R8A8_SINT:
            case Format::B8G8R8A8_UNORM_SRGB:
            case Format::A8B8G8R8_UNORM_PACK32:
            case Format::A8B8G8R8_SNORM_PACK32:
            case Format::A8B8G8R8_USCALED_PACK32:
            case Format::A8B8G8R8_SSCALED_PACK32:
            case Format::A8B8G8R8_UINT_PACK32:
            case Format::A8B8G8R8_SINT_PACK32:
            case Format::A8B8G8R8_UNORM_SRGB_PACK32:
            case Format::A2R10G10B10_UNORM_PACK32:
            case Format::A2R10G10B10_SNORM_PACK32:
            case Format::A2R10G10B10_USCALED_PACK32:
            case Format::A2R10G10B10_SSCALED_PACK32:
            case Format::A2R10G10B10_UINT_PACK32:
            case Format::A2R10G10B10_SINT_PACK32:
            case Format::A2B10G10R10_UNORM_PACK32:
            case Format::A2B10G10R10_SNORM_PACK32:
            case Format::A2B10G10R10_USCALED_PACK32:
            case Format::A2B10G10R10_SSCALED_PACK32:
            case Format::A2B10G10R10_UINT_PACK32:
            case Format::A2B10G10R10_SINT_PACK32:
            case Format::R16G16_UNORM:
            case Format::R16G16_SNORM:
            case Format::R16G16_USCALED:
            case Format::R16G16_SSCALED:
            case Format::R16G16_UINT:
            case Format::R16G16_SINT:
            case Format::R16G16_SFLOAT:
            case Format::R32_UINT:
            case Format::R32_SINT:
            case Format::R32_SFLOAT:
            case Format::B10G11R11_UFLOAT_PACK32:
            case Format::E5B9G9R9_UFLOAT_PACK32:
            case Format::X8_D24_UNORM_PACK32:
            case Format::D32_SFLOAT:
            case Format::D24_UNORM_S8_UINT:
                bits = 32;
                break;
            case Format::R16G16B16_UNORM:
            case Format::R16G16B16_SNORM:
            case Format::R16G16B16_USCALED:
            case Format::R16G16B16_SSCALED:
            case Format::R16G16B16_UINT:
            case Format::R16G16B16_SINT:
            case Format::R16G16B16_SFLOAT:
                bits = 48;
                break;
            case Format::R16G16B16A16_UNORM:
            case Format::R16G16B16A16_SNORM:
            case Format::R16G16B16A16_USCALED:
            case Format::R16G16B16A16_SSCALED:
            case Format::R16G16B16A16_UINT:
            case Format::R16G16B16A16_SINT:
            case Format::R16G16B16A16_SFLOAT:
            case Format::R32G32_UINT:
            case Format::R32G32_SINT:
            case Format::R32G32_SFLOAT:
            case Format::R64_UINT:
            case Format::R64_SINT:
            case Format::R64_SFLOAT:
            case Format::D32_SFLOAT_S8_UINT:
                bits = 64;
                break;
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
            case Format::R32G32B32_SFLOAT:
                bits = 96;
                break;
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
            case Format::R32G32B32A32_SFLOAT:
            case Format::R64G64_UINT:
            case Format::R64G64_SINT:
            case Format::R64G64_SFLOAT:
                bits = 128;
                break;
            case Format::R64G64B64_UINT:
            case Format::R64G64B64_SINT:
            case Format::R64G64B64_SFLOAT:
                bits = 192;
                break;
            case Format::R64G64B64A64_UINT:
            case Format::R64G64B64A64_SINT:
            case Format::R64G64B64A64_SFLOAT:
                bits = 256;
                break;
            default:
                bits = 0;
                break;
        }

        return bits;
    }

    std::uint32_t GetFormatRegionSize(Format format, const BufferImageCopyRegion& reg) noexcept
    {
        //Only cares about row and height. So it must be well defined. No '0' value like in VK
        if(IsFormatCompressed(format))
        {
            //only for BC(S3TC) family
            return std::max<std::uint32_t>(1, ((reg.buffer_row_length + 3) / 4)) *
                   std::max<std::uint32_t>(1, ((reg.buffer_image_height + 3) / 4)) *
                   GetFormatBlockSize(format) * reg.subresource_layers.layer_count;
        }
        else
        {
            return reg.buffer_row_length * reg.buffer_image_height *
                   (GetFormatBitsPerPixel(format) / 8) * reg.subresource_layers.layer_count;
        }
    }

    std::uint32_t GetFormatRegionSize(Format format, const MemoryImageCopyRegion& reg) noexcept
    {
        //Only cares about row and height. So it must be well defined. No '0' value like in VK
        if(IsFormatCompressed(format))
        {
            //only for BC(S3TC) family
            return std::max<std::uint32_t>(1, ((reg.buffer_row_length + 3) / 4)) *
                   std::max<std::uint32_t>(1, ((reg.buffer_image_height + 3) / 4)) *
                   GetFormatBlockSize(format) * reg.subresource_layers.layer_count;
        }
        else
        {
            return reg.buffer_row_length * reg.buffer_image_height *
                   (GetFormatBitsPerPixel(format) / 8) * reg.subresource_layers.layer_count;
        }
    }

    std::uint16_t GetFormatTexelAlignment(Format format) noexcept
    {
        std::uint16_t alignment = 0;
        switch(format)
        {
            case Format::R4G4_UNORM_PACK8:
            case Format::R8_UNORM:
            case Format::R8_SNORM:
            case Format::R8_USCALED:
            case Format::R8_SSCALED:
            case Format::R8_UINT:
            case Format::R8_SINT:
            case Format::R8_UNORM_SRGB:
            case Format::S8_UINT:
            case Format::A8_UNORM:
                alignment = 1;
                break;
            case Format::R8G8_UNORM:
            case Format::R8G8_SNORM:
            case Format::R8G8_USCALED:
            case Format::R8G8_SSCALED:
            case Format::R8G8_UINT:
            case Format::R8G8_SINT:
            case Format::R8G8_UNORM_SRGB:
            case Format::R4G4B4A4_UNORM_PACK16:
            case Format::B4G4R4A4_UNORM_PACK16:
            case Format::R5G6B5_UNORM_PACK16:
            case Format::B5G6R5_UNORM_PACK16:
            case Format::R5G5B5A1_UNORM_PACK16:
            case Format::B5G5R5A1_UNORM_PACK16:
            case Format::A1R5G5B5_UNORM_PACK16:
            case Format::R16_UNORM:
            case Format::R16_SNORM:
            case Format::R16_USCALED:
            case Format::R16_SSCALED:
            case Format::R16_UINT:
            case Format::R16_SINT:
            case Format::R16_SFLOAT:
            case Format::D16_UNORM:
            case Format::A4R4G4B4_UNORM_PACK16:
            case Format::A4B4G4R4_UNORM_PACK16:
            case Format::A1B5G5R5_UNORM_PACK16:
                alignment = 2;
                break;
            case Format::R8G8B8_UNORM:
            case Format::R8G8B8_SNORM:
            case Format::R8G8B8_USCALED:
            case Format::R8G8B8_SSCALED:
            case Format::R8G8B8_UINT:
            case Format::R8G8B8_SINT:
            case Format::R8G8B8_UNORM_SRGB:
            case Format::B8G8R8_UNORM:
            case Format::B8G8R8_SNORM:
            case Format::B8G8R8_USCALED:
            case Format::B8G8R8_SSCALED:
            case Format::B8G8R8_UINT:
            case Format::B8G8R8_SINT:
            case Format::B8G8R8_UNORM_SRGB:
            case Format::D16_UNORM_S8_UINT:
                alignment = 3;
                break;
            case Format::R8G8B8A8_UNORM:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_USCALED:
            case Format::R8G8B8A8_SSCALED:
            case Format::R8G8B8A8_UINT:
            case Format::R8G8B8A8_SINT:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::B8G8R8A8_UNORM:
            case Format::B8G8R8A8_SNORM:
            case Format::B8G8R8A8_USCALED:
            case Format::B8G8R8A8_SSCALED:
            case Format::B8G8R8A8_UINT:
            case Format::B8G8R8A8_SINT:
            case Format::B8G8R8A8_UNORM_SRGB:
            case Format::R16G16_UNORM:
            case Format::R16G16_SNORM:
            case Format::R16G16_USCALED:
            case Format::R16G16_SSCALED:
            case Format::R16G16_UINT:
            case Format::R16G16_SINT:
            case Format::R16G16_SFLOAT:
            case Format::A8B8G8R8_UNORM_PACK32:
            case Format::A8B8G8R8_SNORM_PACK32:
            case Format::A8B8G8R8_USCALED_PACK32:
            case Format::A8B8G8R8_SSCALED_PACK32:
            case Format::A8B8G8R8_UINT_PACK32:
            case Format::A8B8G8R8_SINT_PACK32:
            case Format::A8B8G8R8_UNORM_SRGB_PACK32:
            case Format::A2R10G10B10_UNORM_PACK32:
            case Format::A2R10G10B10_SNORM_PACK32:
            case Format::A2R10G10B10_USCALED_PACK32:
            case Format::A2R10G10B10_SSCALED_PACK32:
            case Format::A2R10G10B10_UINT_PACK32:
            case Format::A2R10G10B10_SINT_PACK32:
            case Format::A2B10G10R10_UNORM_PACK32:
            case Format::A2B10G10R10_SNORM_PACK32:
            case Format::A2B10G10R10_USCALED_PACK32:
            case Format::A2B10G10R10_SSCALED_PACK32:
            case Format::A2B10G10R10_UINT_PACK32:
            case Format::A2B10G10R10_SINT_PACK32:
            case Format::R32_UINT:
            case Format::R32_SINT:
            case Format::R32_SFLOAT:
            case Format::B10G11R11_UFLOAT_PACK32:
            case Format::E5B9G9R9_UFLOAT_PACK32:
            case Format::X8_D24_UNORM_PACK32:
            case Format::D32_SFLOAT:
            case Format::D24_UNORM_S8_UINT:
                alignment = 4;
                break;
            case Format::
                D32_SFLOAT_S8_UINT: //we do not use depth-stencil formats as transferable -> so do not care + check FLOAT_32_UNSIGNED_INT_24_8_REV
                alignment = 5;
                break;
            case Format::R16G16B16_UNORM:
            case Format::R16G16B16_SNORM:
            case Format::R16G16B16_USCALED:
            case Format::R16G16B16_SSCALED:
            case Format::R16G16B16_UINT:
            case Format::R16G16B16_SINT:
            case Format::R16G16B16_SFLOAT:
                alignment = 6;
                break;
            case Format::R16G16B16A16_UNORM:
            case Format::R16G16B16A16_SNORM:
            case Format::R16G16B16A16_USCALED:
            case Format::R16G16B16A16_SSCALED:
            case Format::R16G16B16A16_UINT:
            case Format::R16G16B16A16_SINT:
            case Format::R16G16B16A16_SFLOAT:
            case Format::R32G32_UINT:
            case Format::R32G32_SINT:
            case Format::R32G32_SFLOAT:
            case Format::R64_UINT:
            case Format::R64_SINT:
            case Format::R64_SFLOAT:
            case Format::BC1_RGB_UNORM_BLOCK:
            case Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Format::BC1_RGBA_UNORM_BLOCK:
            case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Format::BC4_UNORM_BLOCK:
            case Format::BC4_SNORM_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Format::EAC_R11_UNORM_BLOCK:
            case Format::EAC_R11_SNORM_BLOCK:
                alignment = 8;
                break;
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
            case Format::R32G32B32_SFLOAT:
                alignment = 12;
                break;
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
            case Format::R32G32B32A32_SFLOAT:
            case Format::R64G64_UINT:
            case Format::R64G64_SINT:
            case Format::R64G64_SFLOAT:
            case Format::BC2_UNORM_BLOCK:
            case Format::BC2_UNORM_SRGB_BLOCK:
            case Format::BC3_UNORM_BLOCK:
            case Format::BC3_UNORM_SRGB_BLOCK:
            case Format::BC5_UNORM_BLOCK:
            case Format::BC5_SNORM_BLOCK:
            case Format::BC6H_UFLOAT_BLOCK:
            case Format::BC6H_SFLOAT_BLOCK:
            case Format::BC7_UNORM_BLOCK:
            case Format::BC7_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
            case Format::EAC_R11G11_UNORM_BLOCK:
            case Format::EAC_R11G11_SNORM_BLOCK:
                alignment = 16;
                break;
            case Format::R64G64B64_UINT:
            case Format::R64G64B64_SINT:
            case Format::R64G64B64_SFLOAT:
                alignment = 24;
                break;
            case Format::R64G64B64A64_UINT:
            case Format::R64G64B64A64_SINT:
            case Format::R64G64B64A64_SFLOAT:
                alignment = 32;
                break;
            default:
                alignment = 0;
                break;
        }

        return alignment;
    }

    FormatType GetFormatType(Format format, ImageAspectFlagBits aspect) noexcept
    {
        FormatType type;

        if(aspect == ImageAspectFlagBits::AspectColorBit)
        {
            switch(format)
            {
                case Format::R4G4_UNORM_PACK8:
                case Format::R4G4B4A4_UNORM_PACK16:
                case Format::B4G4R4A4_UNORM_PACK16:
                case Format::R5G6B5_UNORM_PACK16:
                case Format::B5G6R5_UNORM_PACK16:
                case Format::R5G5B5A1_UNORM_PACK16:
                case Format::B5G5R5A1_UNORM_PACK16:
                case Format::A1R5G5B5_UNORM_PACK16:
                case Format::R8_UNORM:
                case Format::R8_UNORM_SRGB:
                case Format::R8G8_UNORM:
                case Format::R8G8_UNORM_SRGB:
                case Format::R8G8B8_UNORM:
                case Format::R8G8B8_UNORM_SRGB:
                case Format::B8G8R8_UNORM:
                case Format::B8G8R8_UNORM_SRGB:
                case Format::R8G8B8A8_UNORM:
                case Format::R8G8B8A8_UNORM_SRGB:
                case Format::B8G8R8A8_UNORM:
                case Format::B8G8R8A8_UNORM_SRGB:
                case Format::A8B8G8R8_UNORM_PACK32:
                case Format::A8B8G8R8_UNORM_SRGB_PACK32:
                case Format::A2R10G10B10_UNORM_PACK32:
                case Format::A2B10G10R10_UNORM_PACK32:
                case Format::R16_UNORM:
                case Format::R16G16_UNORM:
                case Format::R16G16B16_UNORM:
                case Format::R16G16B16A16_UNORM:
                case Format::BC1_RGB_UNORM_BLOCK:
                case Format::BC1_RGB_UNORM_SRGB_BLOCK:
                case Format::BC1_RGBA_UNORM_BLOCK:
                case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
                case Format::BC2_UNORM_BLOCK:
                case Format::BC2_UNORM_SRGB_BLOCK:
                case Format::BC3_UNORM_BLOCK:
                case Format::BC3_UNORM_SRGB_BLOCK:
                case Format::BC4_UNORM_BLOCK:
                case Format::BC5_UNORM_BLOCK:
                case Format::BC7_UNORM_BLOCK:
                case Format::BC7_UNORM_SRGB_BLOCK:
                case Format::ETC2_R8G8B8_UNORM_BLOCK:
                case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
                case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
                case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
                case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
                case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
                case Format::EAC_R11_UNORM_BLOCK:
                case Format::EAC_R11G11_UNORM_BLOCK:
                case Format::A4R4G4B4_UNORM_PACK16:
                case Format::A4B4G4R4_UNORM_PACK16:
                case Format::A1B5G5R5_UNORM_PACK16:
                case Format::A8_UNORM:
                    type = FormatType::UNORM;
                    break;
                case Format::R8_SNORM:
                case Format::R8G8_SNORM:
                case Format::R8G8B8_SNORM:
                case Format::B8G8R8_SNORM:
                case Format::R8G8B8A8_SNORM:
                case Format::B8G8R8A8_SNORM:
                case Format::A8B8G8R8_SNORM_PACK32:
                case Format::A2R10G10B10_SNORM_PACK32:
                case Format::A2B10G10R10_SNORM_PACK32:
                case Format::R16_SNORM:
                case Format::R16G16_SNORM:
                case Format::R16G16B16_SNORM:
                case Format::R16G16B16A16_SNORM:
                case Format::BC4_SNORM_BLOCK:
                case Format::BC5_SNORM_BLOCK:
                case Format::EAC_R11_SNORM_BLOCK:
                case Format::EAC_R11G11_SNORM_BLOCK:
                    type = FormatType::SNORM;
                    break;
                case Format::R8_USCALED:
                case Format::R8G8_USCALED:
                case Format::R8G8B8_USCALED:
                case Format::B8G8R8_USCALED:
                case Format::R8G8B8A8_USCALED:
                case Format::B8G8R8A8_USCALED:
                case Format::A8B8G8R8_USCALED_PACK32:
                case Format::A2R10G10B10_USCALED_PACK32:
                case Format::A2B10G10R10_USCALED_PACK32:
                case Format::R16_USCALED:
                case Format::R16G16_USCALED:
                case Format::R16G16B16_USCALED:
                case Format::R16G16B16A16_USCALED:
                    type = FormatType::USCALED;
                    break;
                case Format::R8_SSCALED:
                case Format::R8G8_SSCALED:
                case Format::R8G8B8_SSCALED:
                case Format::B8G8R8_SSCALED:
                case Format::R8G8B8A8_SSCALED:
                case Format::B8G8R8A8_SSCALED:
                case Format::A8B8G8R8_SSCALED_PACK32:
                case Format::A2R10G10B10_SSCALED_PACK32:
                case Format::A2B10G10R10_SSCALED_PACK32:
                case Format::R16_SSCALED:
                case Format::R16G16_SSCALED:
                case Format::R16G16B16_SSCALED:
                case Format::R16G16B16A16_SSCALED:
                    type = FormatType::SSCALED;
                    break;
                case Format::R8_UINT:
                case Format::R8G8_UINT:
                case Format::R8G8B8_UINT:
                case Format::B8G8R8_UINT:
                case Format::R8G8B8A8_UINT:
                case Format::B8G8R8A8_UINT:
                case Format::A8B8G8R8_UINT_PACK32:
                case Format::A2R10G10B10_UINT_PACK32:
                case Format::A2B10G10R10_UINT_PACK32:
                case Format::R16_UINT:
                case Format::R16G16_UINT:
                case Format::R16G16B16_UINT:
                case Format::R16G16B16A16_UINT:
                case Format::R32_UINT:
                case Format::R32G32_UINT:
                case Format::R32G32B32_UINT:
                case Format::R32G32B32A32_UINT:
                case Format::R64_UINT:
                case Format::R64G64_UINT:
                case Format::R64G64B64_UINT:
                case Format::R64G64B64A64_UINT:
                    type = FormatType::UINT;
                    break;
                case Format::R8_SINT:
                case Format::R8G8_SINT:
                case Format::R8G8B8_SINT:
                case Format::B8G8R8_SINT:
                case Format::R8G8B8A8_SINT:
                case Format::B8G8R8A8_SINT:
                case Format::A8B8G8R8_SINT_PACK32:
                case Format::A2R10G10B10_SINT_PACK32:
                case Format::A2B10G10R10_SINT_PACK32:
                case Format::R16_SINT:
                case Format::R16G16_SINT:
                case Format::R16G16B16_SINT:
                case Format::R16G16B16A16_SINT:
                case Format::R32_SINT:
                case Format::R32G32_SINT:
                case Format::R32G32B32_SINT:
                case Format::R32G32B32A32_SINT:
                case Format::R64_SINT:
                case Format::R64G64_SINT:
                case Format::R64G64B64_SINT:
                case Format::R64G64B64A64_SINT:
                    type = FormatType::SINT;
                    break;
                case Format::R16_SFLOAT:
                case Format::R16G16_SFLOAT:
                case Format::R16G16B16_SFLOAT:
                case Format::R16G16B16A16_SFLOAT:
                case Format::R32_SFLOAT:
                case Format::R32G32_SFLOAT:
                case Format::R32G32B32_SFLOAT:
                case Format::R32G32B32A32_SFLOAT:
                case Format::R64_SFLOAT:
                case Format::R64G64_SFLOAT:
                case Format::R64G64B64_SFLOAT:
                case Format::R64G64B64A64_SFLOAT:
                case Format::BC6H_SFLOAT_BLOCK:
                    type = FormatType::SFLOAT;
                    break;
                case Format::B10G11R11_UFLOAT_PACK32:
                case Format::E5B9G9R9_UFLOAT_PACK32:
                case Format::BC6H_UFLOAT_BLOCK:
                    type = FormatType::UFLOAT;
                    break;
                default:
                    break;
            };
        }
        else
        {
            switch(format)
            {
                case Format::D16_UNORM:
                    type = FormatType::UNORM;
                    break;
                case Format::X8_D24_UNORM_PACK32:
                    type = FormatType::UNORM;
                    break;
                case Format::D32_SFLOAT:
                    type = FormatType::SFLOAT;
                    break;
                case Format::D16_UNORM_S8_UINT:
                    if(aspect == ImageAspectFlagBits::AspectDepthBit)
                        type = FormatType::UNORM;
                    else
                        type = FormatType::UINT;
                case Format::D24_UNORM_S8_UINT:
                    if(aspect == ImageAspectFlagBits::AspectDepthBit)
                        type = FormatType::UNORM;
                    else
                        type = FormatType::UINT;
                case Format::D32_SFLOAT_S8_UINT:
                    if(aspect == ImageAspectFlagBits::AspectDepthBit)
                        type = FormatType::SFLOAT;
                    else
                        type = FormatType::UINT;
                case Format::S8_UINT:
                    type = FormatType::UINT;
                    break;
                default:
                    break;
            }
        }

        return type;
    }

    bool IsFormatSRGB(Format format) noexcept
    {
        bool is_srgb;

        switch(format)
        {
            case Format::R8_UNORM_SRGB:
            case Format::R8G8_UNORM_SRGB:
            case Format::R8G8B8_UNORM_SRGB:
            case Format::B8G8R8_UNORM_SRGB:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::B8G8R8A8_UNORM_SRGB:
            case Format::A8B8G8R8_UNORM_SRGB_PACK32:
            case Format::BC1_RGB_UNORM_SRGB_BLOCK:
            case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
            case Format::BC2_UNORM_SRGB_BLOCK:
            case Format::BC3_UNORM_SRGB_BLOCK:
            case Format::BC7_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
            case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
                is_srgb = true;
                break;
            default:
                is_srgb = false;
                break;
        }

        return is_srgb;
    }

    std::string_view FormatToString(Format format) noexcept
    {
        std::string_view out;
        switch(format)
        {
            case Format::UNDEFINED:
                out = "UNDEFINED";
                break;
            case Format::R4G4_UNORM_PACK8:
                out = "R4G4_UNORM_PACK8";
                break;
            case Format::R4G4B4A4_UNORM_PACK16:
                out = "R4G4B4A4_UNORM_PACK16";
                break;
            case Format::B4G4R4A4_UNORM_PACK16:
                out = "B4G4R4A4_UNORM_PACK16";
                break;
            case Format::R5G6B5_UNORM_PACK16:
                out = "R5G6B5_UNORM_PACK16";
                break;
            case Format::B5G6R5_UNORM_PACK16:
                out = "B5G6R5_UNORM_PACK16";
                break;
            case Format::R5G5B5A1_UNORM_PACK16:
                out = "R5G5B5A1_UNORM_PACK16";
                break;
            case Format::B5G5R5A1_UNORM_PACK16:
                out = "B5G5R5A1_UNORM_PACK16";
                break;
            case Format::A1R5G5B5_UNORM_PACK16:
                out = "A1R5G5B5_UNORM_PACK16";
                break;
            case Format::R8_UNORM:
                out = "R8_UNORM";
                break;
            case Format::R8_SNORM:
                out = "R8_SNORM";
                break;
            case Format::R8_USCALED:
                out = "R8_USCALED";
                break;
            case Format::R8_SSCALED:
                out = "R8_SSCALED";
                break;
            case Format::R8_UINT:
                out = "R8_UINT";
                break;
            case Format::R8_SINT:
                out = "R8_SINT";
                break;
            case Format::R8_UNORM_SRGB:
                out = "R8_UNORM_SRGB";
                break;
            case Format::R8G8_UNORM:
                out = "R8G8_UNORM";
                break;
            case Format::R8G8_SNORM:
                out = "R8G8_SNORM";
                break;
            case Format::R8G8_USCALED:
                out = "R8G8_USCALED";
                break;
            case Format::R8G8_SSCALED:
                out = "R8G8_SSCALED";
                break;
            case Format::R8G8_UINT:
                out = "R8G8_UINT";
                break;
            case Format::R8G8_SINT:
                out = "R8G8_SINT";
                break;
            case Format::R8G8_UNORM_SRGB:
                out = "R8G8_UNORM_SRGB";
                break;
            case Format::R8G8B8_UNORM:
                out = "R8G8B8_UNORM";
                break;
            case Format::R8G8B8_SNORM:
                out = "R8G8B8_SNORM";
                break;
            case Format::R8G8B8_USCALED:
                out = "R8G8B8_USCALED";
                break;
            case Format::R8G8B8_SSCALED:
                out = "R8G8B8_SSCALED";
                break;
            case Format::R8G8B8_UINT:
                out = "R8G8B8_UINT";
                break;
            case Format::R8G8B8_SINT:
                out = "R8G8B8_SINT";
                break;
            case Format::R8G8B8_UNORM_SRGB:
                out = "R8G8B8_UNORM_SRGB";
                break;
            case Format::B8G8R8_UNORM:
                out = "B8G8R8_UNORM";
                break;
            case Format::B8G8R8_SNORM:
                out = "B8G8R8_SNORM";
                break;
            case Format::B8G8R8_USCALED:
                out = "B8G8R8_USCALED";
                break;
            case Format::B8G8R8_SSCALED:
                out = "B8G8R8_SSCALED";
                break;
            case Format::B8G8R8_UINT:
                out = "B8G8R8_UINT";
                break;
            case Format::B8G8R8_SINT:
                out = "B8G8R8_SINT";
                break;
            case Format::B8G8R8_UNORM_SRGB:
                out = "B8G8R8_UNORM_SRGB";
                break;
            case Format::R8G8B8A8_UNORM:
                out = "R8G8B8A8_UNORM";
                break;
            case Format::R8G8B8A8_SNORM:
                out = "R8G8B8A8_SNORM";
                break;
            case Format::R8G8B8A8_USCALED:
                out = "R8G8B8A8_USCALED";
                break;
            case Format::R8G8B8A8_SSCALED:
                out = "R8G8B8A8_SSCALED";
                break;
            case Format::R8G8B8A8_UINT:
                out = "R8G8B8A8_UINT";
                break;
            case Format::R8G8B8A8_SINT:
                out = "R8G8B8A8_SINT";
                break;
            case Format::R8G8B8A8_UNORM_SRGB:
                out = "R8G8B8A8_UNORM_SRGB";
                break;
            case Format::B8G8R8A8_UNORM:
                out = "B8G8R8A8_UNORM";
                break;
            case Format::B8G8R8A8_SNORM:
                out = "B8G8R8A8_SNORM";
                break;
            case Format::B8G8R8A8_USCALED:
                out = "B8G8R8A8_USCALED";
                break;
            case Format::B8G8R8A8_SSCALED:
                out = "B8G8R8A8_SSCALED";
                break;
            case Format::B8G8R8A8_UINT:
                out = "B8G8R8A8_UINT";
                break;
            case Format::B8G8R8A8_SINT:
                out = "B8G8R8A8_SINT";
                break;
            case Format::B8G8R8A8_UNORM_SRGB:
                out = "B8G8R8A8_UNORM_SRGB";
                break;
            case Format::A8B8G8R8_UNORM_PACK32:
                out = "A8B8G8R8_UNORM_PACK32";
                break;
            case Format::A8B8G8R8_SNORM_PACK32:
                out = "A8B8G8R8_SNORM_PACK32";
                break;
            case Format::A8B8G8R8_USCALED_PACK32:
                out = "A8B8G8R8_USCALED_PACK32";
                break;
            case Format::A8B8G8R8_SSCALED_PACK32:
                out = "A8B8G8R8_SSCALED_PACK32";
                break;
            case Format::A8B8G8R8_UINT_PACK32:
                out = "A8B8G8R8_UINT_PACK32";
                break;
            case Format::A8B8G8R8_SINT_PACK32:
                out = "A8B8G8R8_SINT_PACK32";
                break;
            case Format::A8B8G8R8_UNORM_SRGB_PACK32:
                out = "A8B8G8R8_UNORM_SRGB_PACK32";
                break;
            case Format::A2R10G10B10_UNORM_PACK32:
                out = "A2R10G10B10_UNORM_PACK32";
                break;
            case Format::A2R10G10B10_SNORM_PACK32:
                out = "A2R10G10B10_SNORM_PACK32";
                break;
            case Format::A2R10G10B10_USCALED_PACK32:
                out = "A2R10G10B10_USCALED_PACK32";
                break;
            case Format::A2R10G10B10_SSCALED_PACK32:
                out = "A2R10G10B10_SSCALED_PACK32";
                break;
            case Format::A2R10G10B10_UINT_PACK32:
                out = "A2R10G10B10_UINT_PACK32";
                break;
            case Format::A2R10G10B10_SINT_PACK32:
                out = "A2R10G10B10_SINT_PACK32";
                break;
            case Format::A2B10G10R10_UNORM_PACK32:
                out = "A2B10G10R10_UNORM_PACK32";
                break;
            case Format::A2B10G10R10_SNORM_PACK32:
                out = "A2B10G10R10_SNORM_PACK32";
                break;
            case Format::A2B10G10R10_USCALED_PACK32:
                out = "A2B10G10R10_USCALED_PACK32";
                break;
            case Format::A2B10G10R10_SSCALED_PACK32:
                out = "A2B10G10R10_SSCALED_PACK32";
                break;
            case Format::A2B10G10R10_UINT_PACK32:
                out = "A2B10G10R10_UINT_PACK32";
                break;
            case Format::A2B10G10R10_SINT_PACK32:
                out = "A2B10G10R10_SINT_PACK32";
                break;
            case Format::R16_UNORM:
                out = "R16_UNORM";
                break;
            case Format::R16_SNORM:
                out = "R16_SNORM";
                break;
            case Format::R16_USCALED:
                out = "R16_USCALED";
                break;
            case Format::R16_SSCALED:
                out = "R16_SSCALED";
                break;
            case Format::R16_UINT:
                out = "R16_UINT";
                break;
            case Format::R16_SINT:
                out = "R16_SINT";
                break;
            case Format::R16_SFLOAT:
                out = "R16_SFLOAT";
                break;
            case Format::R16G16_UNORM:
                out = "R16G16_UNORM";
                break;
            case Format::R16G16_SNORM:
                out = "R16G16_SNORM";
                break;
            case Format::R16G16_USCALED:
                out = "R16G16_USCALED";
                break;
            case Format::R16G16_SSCALED:
                out = "R16G16_SSCALED";
                break;
            case Format::R16G16_UINT:
                out = "R16G16_UINT";
                break;
            case Format::R16G16_SINT:
                out = "R16G16_SINT";
                break;
            case Format::R16G16_SFLOAT:
                out = "R16G16_SFLOAT";
                break;
            case Format::R16G16B16_UNORM:
                out = "R16G16B16_UNORM";
                break;
            case Format::R16G16B16_SNORM:
                out = "R16G16B16_SNORM";
                break;
            case Format::R16G16B16_USCALED:
                out = "R16G16B16_USCALED";
                break;
            case Format::R16G16B16_SSCALED:
                out = "R16G16B16_SSCALED";
                break;
            case Format::R16G16B16_UINT:
                out = "R16G16B16_UINT";
                break;
            case Format::R16G16B16_SINT:
                out = "R16G16B16_SINT";
                break;
            case Format::R16G16B16_SFLOAT:
                out = "R16G16B16_SFLOAT";
                break;
            case Format::R16G16B16A16_UNORM:
                out = "R16G16B16A16_UNORM";
                break;
            case Format::R16G16B16A16_SNORM:
                out = "R16G16B16A16_SNORM";
                break;
            case Format::R16G16B16A16_USCALED:
                out = "R16G16B16A16_USCALED";
                break;
            case Format::R16G16B16A16_SSCALED:
                out = "R16G16B16A16_SSCALED";
                break;
            case Format::R16G16B16A16_UINT:
                out = "R16G16B16A16_UINT";
                break;
            case Format::R16G16B16A16_SINT:
                out = "R16G16B16A16_SINT";
                break;
            case Format::R16G16B16A16_SFLOAT:
                out = "R16G16B16A16_SFLOAT";
                break;
            case Format::R32_UINT:
                out = "R32_UINT";
                break;
            case Format::R32_SINT:
                out = "R32_SINT";
                break;
            case Format::R32_SFLOAT:
                out = "R32_SFLOAT";
                break;
            case Format::R32G32_UINT:
                out = "R32G32_UINT";
                break;
            case Format::R32G32_SINT:
                out = "R32G32_SINT";
                break;
            case Format::R32G32_SFLOAT:
                out = "R32G32_SFLOAT";
                break;
            case Format::R32G32B32_UINT:
                out = "R32G32B32_UINT";
                break;
            case Format::R32G32B32_SINT:
                out = "R32G32B32_SINT";
                break;
            case Format::R32G32B32_SFLOAT:
                out = "R32G32B32_SFLOAT";
                break;
            case Format::R32G32B32A32_UINT:
                out = "R32G32B32A32_UINT";
                break;
            case Format::R32G32B32A32_SINT:
                out = "R32G32B32A32_SINT";
                break;
            case Format::R32G32B32A32_SFLOAT:
                out = "R32G32B32A32_SFLOAT";
                break;
            case Format::R64_UINT:
                out = "R64_UINT";
                break;
            case Format::R64_SINT:
                out = "R64_SINT";
                break;
            case Format::R64_SFLOAT:
                out = "R64_SFLOAT";
                break;
            case Format::R64G64_UINT:
                out = "R64G64_UINT";
                break;
            case Format::R64G64_SINT:
                out = "R64G64_SINT";
                break;
            case Format::R64G64_SFLOAT:
                out = "R64G64_SFLOAT";
                break;
            case Format::R64G64B64_UINT:
                out = "R64G64B64_UINT";
                break;
            case Format::R64G64B64_SINT:
                out = "R64G64B64_SINT";
                break;
            case Format::R64G64B64_SFLOAT:
                out = "R64G64B64_SFLOAT";
                break;
            case Format::R64G64B64A64_UINT:
                out = "R64G64B64A64_UINT";
                break;
            case Format::R64G64B64A64_SINT:
                out = "R64G64B64A64_SINT";
                break;
            case Format::R64G64B64A64_SFLOAT:
                out = "R64G64B64A64_SFLOAT";
                break;
            case Format::B10G11R11_UFLOAT_PACK32:
                out = "B10G11R11_UFLOAT_PACK32";
                break;
            case Format::E5B9G9R9_UFLOAT_PACK32:
                out = "E5B9G9R9_UFLOAT_PACK32";
                break;
            case Format::D16_UNORM:
                out = "D16_UNORM";
                break;
            case Format::X8_D24_UNORM_PACK32:
                out = "X8_D24_UNORM_PACK32";
                break;
            case Format::D32_SFLOAT:
                out = "D32_SFLOAT";
                break;
            case Format::S8_UINT:
                out = "S8_UINT";
                break;
            case Format::D16_UNORM_S8_UINT:
                out = "D16_UNORM_S8_UINT";
                break;
            case Format::D24_UNORM_S8_UINT:
                out = "D24_UNORM_S8_UINT";
                break;
            case Format::D32_SFLOAT_S8_UINT:
                out = "D32_SFLOAT_S8_UINT";
                break;
            case Format::BC1_RGB_UNORM_BLOCK:
                out = "BC1_RGB_UNORM_BLOCK";
                break;
            case Format::BC1_RGB_UNORM_SRGB_BLOCK:
                out = "BC1_RGB_UNORM_SRGB_BLOCK";
                break;
            case Format::BC1_RGBA_UNORM_BLOCK:
                out = "BC1_RGBA_UNORM_BLOCK";
                break;
            case Format::BC1_RGBA_UNORM_SRGB_BLOCK:
                out = "BC1_RGBA_UNORM_SRGB_BLOCK";
                break;
            case Format::BC2_UNORM_BLOCK:
                out = "BC2_UNORM_BLOCK";
                break;
            case Format::BC2_UNORM_SRGB_BLOCK:
                out = "BC2_UNORM_SRGB_BLOCK";
                break;
            case Format::BC3_UNORM_BLOCK:
                out = "BC3_UNORM_BLOCK";
                break;
            case Format::BC3_UNORM_SRGB_BLOCK:
                out = "BC3_UNORM_SRGB_BLOCK";
                break;
            case Format::BC4_UNORM_BLOCK:
                out = "BC4_UNORM_BLOCK";
                break;
            case Format::BC4_SNORM_BLOCK:
                out = "BC4_SNORM_BLOCK";
                break;
            case Format::BC5_UNORM_BLOCK:
                out = "BC5_UNORM_BLOCK";
                break;
            case Format::BC5_SNORM_BLOCK:
                out = "BC5_SNORM_BLOCK";
                break;
            case Format::BC6H_UFLOAT_BLOCK:
                out = "BC6H_UFLOAT_BLOCK";
                break;
            case Format::BC6H_SFLOAT_BLOCK:
                out = "BC6H_SFLOAT_BLOCK";
                break;
            case Format::BC7_UNORM_BLOCK:
                out = "BC7_UNORM_BLOCK";
                break;
            case Format::BC7_UNORM_SRGB_BLOCK:
                out = "BC7_UNORM_SRGB_BLOCK";
                break;
            case Format::ETC2_R8G8B8_UNORM_BLOCK:
                out = "ETC2_R8G8B8_UNORM_BLOCK";
                break;
            case Format::ETC2_R8G8B8_UNORM_SRGB_BLOCK:
                out = "ETC2_R8G8B8_UNORM_SRGB_BLOCK";
                break;
            case Format::ETC2_R8G8B8A1_UNORM_BLOCK:
                out = "ETC2_R8G8B8A1_UNORM_BLOCK";
                break;
            case Format::ETC2_R8G8B8A1_UNORM_SRGB_BLOCK:
                out = "ETC2_R8G8B8A1_UNORM_SRGB_BLOCK";
                break;
            case Format::ETC2_R8G8B8A8_UNORM_BLOCK:
                out = "ETC2_R8G8B8A8_UNORM_BLOCK";
                break;
            case Format::ETC2_R8G8B8A8_UNORM_SRGB_BLOCK:
                out = "ETC2_R8G8B8A8_UNORM_SRGB_BLOCK";
                break;
            case Format::EAC_R11_UNORM_BLOCK:
                out = "EAC_R11_UNORM_BLOCK";
                break;
            case Format::EAC_R11_SNORM_BLOCK:
                out = "EAC_R11_SNORM_BLOCK";
                break;
            case Format::EAC_R11G11_UNORM_BLOCK:
                out = "EAC_R11G11_UNORM_BLOCK";
                break;
            case Format::EAC_R11G11_SNORM_BLOCK:
                out = "EAC_R11G11_SNORM_BLOCK";
                break;
            case Format::A4R4G4B4_UNORM_PACK16:
                out = "A4R4G4B4_UNORM_PACK16";
                break;
            case Format::A4B4G4R4_UNORM_PACK16:
                out = "A4B4G4R4_UNORM_PACK16";
                break;
            case Format::A1B5G5R5_UNORM_PACK16:
                out = "A1B5G5R5_UNORM_PACK16";
                break;
            case Format::A8_UNORM:
                out = "A8_UNORM";
                break;
        }

        return out;
    }
};