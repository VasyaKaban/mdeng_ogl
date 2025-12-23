#pragma once

#include "Render.h"

namespace Render
{
    bool IsFormatCompressed(Format format) noexcept;
    bool IsBCFormat(Format format) noexcept;
    bool IsETC2Format(Format format) noexcept;
    bool IsEACFormat(Format format) noexcept;

    bool IsDepthFormat(Format format) noexcept;
    bool IsStencilFormat(Format format) noexcept;
    bool IsDepthStencilFormat(Format format) noexcept;
    bool IsColorFormat(Format format) noexcept;

    std::uint8_t GetFormatBlockSize(Format format) noexcept;

    std::uint16_t GetFormatBitsPerPixel(Format format) noexcept;

    std::uint32_t GetFormatRegionSize(Format format, const BufferImageCopyRegion& reg) noexcept;

    std::uint32_t GetFormatRegionSize(Format format, const MemoryImageCopyRegion& reg) noexcept;

    std::uint16_t GetFormatTexelAlignment(Format format) noexcept;

    FormatType GetFormatType(Format format, ImageAspectFlagBits aspect) noexcept;

    bool IsFormatSRGB(Format format) noexcept;
};