#pragma once

#include "Types.hpp"

namespace Core
{
    namespace SizeLiterals
    {
        constexpr UInt64 operator""_B(unsigned long long bytes) noexcept
        {
            return bytes;
        }

        constexpr UInt64 operator""_KiB(unsigned long long kbytes) noexcept
        {
            return kbytes * 1024;
        }

        constexpr UInt64 operator""_MiB(unsigned long long mbytes) noexcept
        {
            return mbytes * 1024 * 1024;
        }

        constexpr UInt64 operator""_GiB(unsigned long long gbytes) noexcept
        {
            return gbytes * 1024 * 1024 * 1024;
        }
    }
};