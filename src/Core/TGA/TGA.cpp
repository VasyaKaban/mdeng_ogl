#include "TGA.h"
#include <fstream>
#include <format>

namespace TGA
{
    std::optional<std::runtime_error> Write(const WriteInfo& info)
    {
        Header hdr;
        hdr.id_length = (info.id.length() > 255 ? 255 : info.id.length());

        if(info.data_type != DataType::UncompressedRGB)
            return std::runtime_error("DataType is not supported");

        hdr.color_map_type = ColorMapType::NoColorMapPresent;

        if(info.width == 0 && info.height == 0)
        {
            hdr.image_type = ImageType::NoImageData;
            hdr.image_spec.image_descriptor.alpha_depth = 0;
        }
        else if(info.width == 0 || info.height == 0)
        {
            return std::runtime_error("Width or height can't be 0");
        }
        else
        {
            if(!(info.pixel_depth == PixelDepth::Bits24 || info.pixel_depth == PixelDepth::Bits32))
                return std::runtime_error("PixelDepth is not supported");

            hdr.image_type = ImageType::UncompressedTrueColor;

            if(info.pixel_depth == PixelDepth::Bits24)
                hdr.image_spec.image_descriptor.alpha_depth = 0;
            else
                hdr.image_spec.image_descriptor.alpha_depth = 8;
        }

        std::fill_n(hdr.color_map, 5, 0);

        hdr.image_spec.x_origin = 0, hdr.image_spec.y_origin = 0;
        hdr.image_spec.width = info.width;
        hdr.image_spec.height = info.height;
        hdr.image_spec.pixel_depth = info.pixel_depth;

        hdr.image_spec.image_descriptor.right_to_left = false;
        hdr.image_spec.image_descriptor.top_to_bottom = info.top_to_bottom;
        hdr.image_spec.image_descriptor.interleaving = InterleavingFlag::None;

        std::ofstream out;
        out.open(info.filename, std::ios::out | std::ios::binary);
        if(!out.is_open())
            return std::runtime_error(std::format("Failed to open file: {}", info.filename));

        out.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
        if(!info.id.empty())
            out.write(info.id.data(), hdr.id_length);

        out.write(reinterpret_cast<const char*>(info.data),
                  info.width * info.height * (static_cast<std::uint8_t>(info.pixel_depth) / 8));

        return std::nullopt;
    }
};