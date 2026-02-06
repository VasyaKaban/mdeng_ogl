#pragma once

#include "Render.h"

namespace Render
{
    CORE_API bool IsFormatCompressed(Format format) noexcept;
    CORE_API bool IsBCFormat(Format format) noexcept;
    CORE_API bool IsETC2Format(Format format) noexcept;
    CORE_API bool IsEACFormat(Format format) noexcept;

    CORE_API bool IsDepthFormat(Format format) noexcept;
    CORE_API bool IsStencilFormat(Format format) noexcept;
    CORE_API bool IsDepthStencilFormat(Format format) noexcept;
    CORE_API bool IsColorFormat(Format format) noexcept;

    CORE_API std::uint8_t GetFormatBlockSize(Format format) noexcept;

    CORE_API std::uint16_t GetFormatBitsPerPixel(Format format) noexcept;

    CORE_API std::uint32_t GetFormatRegionSize(Format format,
                                               const BufferImageCopyRegion& reg) noexcept;

    CORE_API std::uint32_t GetFormatRegionSize(Format format,
                                               const MemoryImageCopyRegion& reg) noexcept;

    CORE_API std::uint16_t GetFormatTexelAlignment(Format format) noexcept;

    CORE_API FormatType GetFormatType(Format format, ImageAspectFlagBits aspect) noexcept;

    CORE_API bool IsFormatSRGB(Format format) noexcept;
};