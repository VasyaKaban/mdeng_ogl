#include "Render.h"

namespace Render
{
    bool IsFormatCompressed(Format format) noexcept
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

    bool IsDepthStencilFormat(Format format) noexcept
    {
        bool is_depth_stencil;
        switch(format)
        {
            case Format::D16_UNORM:
            case Format::D24_UNORM_S8_UINT:
            case Format::D32_FLOAT_S8X24_UINT:
            case Format::D32_FLOAT:
                is_depth_stencil = true;
                break;
            default:
                is_depth_stencil = false;
                break;
        }

        return is_depth_stencil;
    }

    std::uint8_t GetFormatBlockSize(Format format) noexcept
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

    std::uint16_t GetFormatBitsPerPixel(Format format) noexcept
    {
        std::uint16_t bits;
        switch(format)
        {
            case Format::R32G32B32A32_FLOAT:
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
                bits = 128;
                break;
            case Format::R32G32B32_FLOAT:
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
                bits = 96;
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
                bits = 64;
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
                bits = 32;
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
                bits = 16;
                break;
            case Format::R8_UNORM:
            case Format::R8_UINT:
            case Format::R8_SNORM:
            case Format::R8_SINT:
                bits = 8;
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

    /*#error WRONG!!!
constexpr std::uint32_t GetFormatRegionSize(Format format, const Extent3D& extent) noexcept
{
    std::uint32_t region_size = 0;
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
            //default:
            //    region_size = 0;
            //    break;
    }

    return region_size;
}*/

    std::uint16_t GetFormatTexelAlignment(Format format) noexcept
    {
        std::uint16_t alignment = 0;
        switch(format)
        {
            case Format::R32G32B32A32_FLOAT:
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
            case Format::R32G32B32_FLOAT:
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
            case Format::R32G32_FLOAT:
            case Format::R32G32_UINT:
            case Format::R32G32_SINT:
            case Format::D32_FLOAT_S8X24_UINT:
            case Format::R10G10B10A2_UNORM:
            case Format::R10G10B10A2_UINT:
            case Format::R11G11B10_FLOAT:
            case Format::D32_FLOAT:
            case Format::R32_FLOAT:
            case Format::R32_UINT:
            case Format::R32_SINT:
            case Format::D24_UNORM_S8_UINT:
            case Format::R9G9B9E5_SHAREDEXP:
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
                alignment = 4;
                break;
            case Format::R16G16B16A16_FLOAT:
            case Format::R16G16B16A16_UNORM:
            case Format::R16G16B16A16_UINT:
            case Format::R16G16B16A16_SNORM:
            case Format::R16G16B16A16_SINT:
            case Format::R16G16_FLOAT:
            case Format::R16G16_UNORM:
            case Format::R16G16_UINT:
            case Format::R16G16_SNORM:
            case Format::R16G16_SINT:
            case Format::R16_FLOAT:
            case Format::D16_UNORM:
            case Format::R16_UNORM:
            case Format::R16_UINT:
            case Format::R16_SNORM:
            case Format::R16_SINT:
            case Format::B5G6R5_UNORM:
            case Format::B5G5R5A1_UNORM:
            case Format::B4G4R4A4_UNORM:
                alignment = 2;
                break;
            case Format::R8G8B8A8_UNORM:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::R8G8B8A8_UINT:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_SINT:
            case Format::R8G8_UNORM:
            case Format::R8G8_UINT:
            case Format::R8G8_SNORM:
            case Format::R8G8_SINT:
            case Format::R8_UNORM:
            case Format::R8_UINT:
            case Format::R8_SNORM:
            case Format::R8_SINT:
                alignment = 1;
                break;
        }

        return alignment;
    }

    FormatComponentsBitSize GetFormatComponentsBitSize(Format format) noexcept
    {
        FormatComponentsBitSize components;
        switch(format)
        {
            case Format::R32G32B32A32_FLOAT:
            case Format::R32G32B32A32_UINT:
            case Format::R32G32B32A32_SINT:
                components =
                    FormatComponentsBitSize{.red = 32, .green = 32, .blue = 32, .alpha = 32};
                break;
            case Format::R32G32B32_FLOAT:
            case Format::R32G32B32_UINT:
            case Format::R32G32B32_SINT:
                components =
                    FormatComponentsBitSize{.red = 32, .green = 32, .blue = 32, .alpha = 0};
                break;
            case Format::R16G16B16A16_FLOAT:
            case Format::R16G16B16A16_UNORM:
            case Format::R16G16B16A16_UINT:
            case Format::R16G16B16A16_SNORM:
            case Format::R16G16B16A16_SINT:
                components =
                    FormatComponentsBitSize{.red = 16, .green = 16, .blue = 16, .alpha = 16};
                break;
            case Format::R32G32_FLOAT:
            case Format::R32G32_UINT:
            case Format::R32G32_SINT:
                components = FormatComponentsBitSize{.red = 32, .green = 32, .blue = 0, .alpha = 0};
                break;
            case Format::R10G10B10A2_UNORM:
            case Format::R10G10B10A2_UINT:
                components =
                    FormatComponentsBitSize{.red = 10, .green = 10, .blue = 10, .alpha = 2};
                break;
            case Format::R11G11B10_FLOAT:
                components =
                    FormatComponentsBitSize{.red = 11, .green = 11, .blue = 11, .alpha = 0};
                break;
            case Format::R8G8B8A8_UNORM:
            case Format::R8G8B8A8_UNORM_SRGB:
            case Format::R8G8B8A8_UINT:
            case Format::R8G8B8A8_SNORM:
            case Format::R8G8B8A8_SINT:
                components = FormatComponentsBitSize{.red = 8, .green = 8, .blue = 8, .alpha = 8};
                break;
            case Format::R16G16_FLOAT:
            case Format::R16G16_UNORM:
            case Format::R16G16_UINT:
            case Format::R16G16_SNORM:
            case Format::R16G16_SINT:
                components = FormatComponentsBitSize{.red = 16, .green = 16, .blue = 0, .alpha = 0};
                break;
            case Format::R32_FLOAT:
            case Format::R32_UINT:
            case Format::R32_SINT:
                components = FormatComponentsBitSize{.red = 32, .green = 0, .blue = 0, .alpha = 0};
                break;
            case Format::R8G8_UNORM:
            case Format::R8G8_UINT:
            case Format::R8G8_SNORM:
            case Format::R8G8_SINT:
                components = FormatComponentsBitSize{.red = 8, .green = 8, .blue = 0, .alpha = 0};
                break;
            case Format::R16_FLOAT:
            case Format::R16_UNORM:
            case Format::R16_UINT:
            case Format::R16_SNORM:
            case Format::R16_SINT:
                components = FormatComponentsBitSize{.red = 16, .green = 0, .blue = 0, .alpha = 0};
                break;
            case Format::R8_UNORM:
            case Format::R8_UINT:
            case Format::R8_SNORM:
            case Format::R8_SINT:
                components = FormatComponentsBitSize{.red = 8, .green = 0, .blue = 0, .alpha = 0};
                break;
            case Format::R9G9B9E5_SHAREDEXP:
                components = FormatComponentsBitSize{.red = 9, .green = 9, .blue = 9, .alpha = 0};
                break;
            case Format::B5G6R5_UNORM:
                components = FormatComponentsBitSize{.red = 5, .green = 6, .blue = 5, .alpha = 0};
                break;
            case Format::B5G5R5A1_UNORM:
                components = FormatComponentsBitSize{.red = 5, .green = 5, .blue = 5, .alpha = 1};
                break;
            case Format::B4G4R4A4_UNORM:
                components = FormatComponentsBitSize{.red = 4, .green = 4, .blue = 4, .alpha = 4};
                break;
            default:
                components = FormatComponentsBitSize{.red = 0, .green = 0, .blue = 0, .alpha = 0};
                break;
        }

        return components;
    }
};