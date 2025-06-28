#include "DDS.h"
#include <stdexcept>
#include <format>

namespace DDS
{
    static void check_dds_header(const Header* header)
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
    }

    static void check_dds_dxt10_header(const DXT10Header* header)
    {}

    DDSResult Parse(std::span<const std::uint8_t> data)
    {
        constexpr std::size_t MIN_DDS_SIZE = sizeof(DDS_MAGIC_NUMBER) + sizeof(Header);

        if(data.size() < MIN_DDS_SIZE)
            throw std::runtime_error(
                std::format("Size of data must be greater than or equal to MIN_DDS_SIZE({})",
                            MIN_DDS_SIZE));

        if(DDS_MAGIC_NUMBER != MafeFourCC(data[0], data[1], data[2], data[3]))
            throw std::runtime_error(
                std::format("Bad magic number. Must be: {}", DDS_MAGIC_NUMBER));

        DDSResult result = {};
        result.header = reinterpret_cast<const Header*>(data.data() + sizeof(DDS_MAGIC_NUMBER));

        check_dds_header(result.header);

        if(result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_FOURCC &&
           result.header->pixel_format.four_cc == PixelFormatFourCC::DX10)
        {
            constexpr std::size_t MIN_DDS_SIZE_DXT10 =
                sizeof(DDS_MAGIC_NUMBER) + sizeof(Header) + sizeof(DXT10Header);

            if(data.size() < MIN_DDS_SIZE_DXT10)
                throw std::runtime_error(std::format(
                    "Size of data must be greater than or equal to MIN_DDS_SIZE_DXT10({})",
                    MIN_DDS_SIZE_DXT10));

            result.dxt10_header = reinterpret_cast<const DXT10Header*>(
                data.data() + sizeof(DDS_MAGIC_NUMBER) + sizeof(Header));

            check_dds_dxt10_header(result.dxt10_header);

            result.image_data = data.subspan(MIN_DDS_SIZE_DXT10);
        }
        else
        {
            result.image_data = data.subspan(MIN_DDS_SIZE);
        }

        return result;
    }

    ResolveResult resolve(const DDSResult& result)
    {
        ResolveResult resolve;

        resolve.image_info.extent.width = result.header->width;
        resolve.image_info.extent.height = result.header->height;
        resolve.image_info.image_type = ImageType::Image2D;
        if(result.header->flags & HeaderFlagBits::DDSD_DEPTH ||
           result.header->caps3 & HeaderCaps2FlagBits::DDSCAPS2_VOLUME)
        {
            resolve.image_info.image_type = ImageType::Image3D;
            resolve.image_info.extent.depth = result.header->depth;
        }
        else
            resolve.image_info.extent.depth = 1;

        if((result.header->flags & HeaderFlagBits::DDSD_MIPMAPCOUNT ||
            result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_MIPMAP) &&
           result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX)
            resolve.image_info.mip_levels = result.header->mip_map_count;
        else
            resolve.image_info.mip_levels = 1;

        resolve.image_info.samples = SampleCount::SampleCount_1;
        resolve.image_info.array_layers = 1;

        resolve.is_cubemap = false;

        if(result.header->caps2 & HeaderCaps2FlagBits::DDSCAPS2_CUBEMAP &&
           result.header->caps1 & HeaderCaps1FlagBits::DDSCAPS_COMPLEX)
        {
            resolve.is_cubemap = true;
            resolve.image_info.array_layers = 6;
        }

        //format
        //array layers
        if(!result.dxt10_header)
        {
            resolve.regions.reserve(resolve.image_info.array_layers *
                                    resolve.image_info.mip_levels);

            if(result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_FOURCC) //compressed
            {
                switch(result.header->pixel_format.four_cc)
                {
                    case PixelFormatFourCC::DXT1:
                        resolve.image_info.format = Format::BC1_UNORM;
                        break;
                    case PixelFormatFourCC::DXT3:
                        resolve.image_info.format = Format::BC2_UNORM;
                        break;
                    case PixelFormatFourCC::DXT5:
                        resolve.image_info.format = Format::BC3_UNORM;
                        break;
                    default:
                        throw std::runtime_error("Unsupported FourCC type");
                        break;
                }
            }
            else //uncompressed
            {
#pragma message("ADD MORE RGBA-bit support!!!")

                if(!(result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_RGB &&
                     (result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_ALPHAPIXELS ||
                      result.header->pixel_format.flags & PixelFormatFlagBits::DDPF_ALPHA)))
                    throw std::runtime_error("Unsupported image type");

                if(result.header->pixel_format.rgb_bit_count != 32)
                    throw std::runtime_error(
                        "Unsupported image rgba bit count"); //idk how to handle 32bit+ formats

                if(std::popcount(result.header->pixel_format.red_bit_mask) != 8 ||
                   std::popcount(result.header->pixel_format.green_bit_mask) != 8 ||
                   std::popcount(result.header->pixel_format.blue_bit_mask) != 8 ||
                   std::popcount(result.header->pixel_format.alpha_bit_mask) != 8)
                {
                    throw std::runtime_error(
                        "Unsupported image rgba bit count"); //idk how to handle 32bit+ formats
                }

                resolve.image_info.format = Format::R8G8B8A8_UNORM; //32bit

                constexpr DDS_DWORD RED_MASK_POPCNT = std::popcount(0x00'00'00'ffu); //8
                constexpr DDS_DWORD GREEN_MASK_POPCNT = std::popcount(0x00'00'ff'00u); //16
                constexpr DDS_DWORD BLUE_MASK_POPCNT = std::popcount(0x00'ff'00'00u); //24
                constexpr DDS_DWORD ALPHA_MASK_POPCNT = std::popcount(0xff'00'00'00u); //32
            }

            const std::uint8_t* sub_image_data_ptr = result.image_data.data();
            for(std::size_t layer = 0; layer < resolve.image_info.array_layers; layer++)
            {
                Extent3D extent = resolve.image_info.extent;

                for(std::size_t mipmap = 0; mipmap < resolve.image_info.mip_levels; mipmap++)
                {
                    extent.width >>= mipmap;
                    extent.height >>= mipmap;
                    extent.depth >>= mipmap;

                    std::size_t sub_image_size =
                        GetFormatRegionSize(resolve.image_info.format, extent);

                    resolve.regions.push_back(ImageInitRegion{
                        .subresource_layers =
                            ImageSubresourceLayers{.mip_level = static_cast<GLuint>(mipmap),
                                                   .base_layer = static_cast<GLuint>(layer),
                                                   .layer_count = 1},
                        .extent = extent,
                        .offset = Offset3D{.x = 0, .y = 0, .z = 0},
                        .data = reinterpret_cast<const std::byte*>(sub_image_data_ptr)});

                    sub_image_data_ptr += sub_image_size;
                }
            }
        }
        else
        {
            throw std::runtime_error("UNIMPLEMENTED");
        }

        return resolve;
    }
};