#pragma once

#include <optional>
#include <string_view>
#include <stdexcept>

namespace TGA
{
    enum class DataType
    {
        UncompressedRGB
    };

    enum class ColorMapType : std::uint8_t
    {
        NoColorMapPresent = 0,
        ColorMapPresent = 1
    };

    enum class ImageType : std::uint8_t
    {
        NoImageData = 0,
        UncompressedColorMapped = 1,
        UncompressedTrueColor = 2,
        UncompressedGrayScale = 3,
        RLEColorMapped = 9,
        RLETrueColor = 10,
        RLEGrayScale = 11
    };

    enum class PixelDepth : std::uint8_t
    {
        Bits16 = 16,
        Bits24 = 24,
        Bits32 = 32
    };

    enum class InterleavingFlag : std::uint8_t
    {
        None = 0b00,
        TwoWay = 0b01,
        FourWay = 0b10,
        //Reserved = 0b11
    };

    struct ImageDescriptor
    {
        std::uint8_t alpha_depth: 4;
        bool right_to_left: 1;
        bool top_to_bottom: 1;
        InterleavingFlag interleaving: 2;
    };

    struct ImageSpecification
    {
        std::uint16_t x_origin;
        std::uint16_t y_origin;
        std::uint16_t width;
        std::uint16_t height;
        PixelDepth pixel_depth;
        ImageDescriptor image_descriptor;
    };

    struct Header
    {
        std::uint8_t id_length;
        ColorMapType color_map_type;
        ImageType image_type;
        std::uint8_t color_map[5];
        ImageSpecification image_spec;
    };

    struct WriteInfo
    {
        const char* filename;
        std::uint16_t width;
        std::uint16_t height;
        DataType data_type;
        PixelDepth pixel_depth;
        bool top_to_bottom;
        const std::byte* data;
        std::string_view id;
    };

    std::optional<std::runtime_error> Write(const WriteInfo& info);
};