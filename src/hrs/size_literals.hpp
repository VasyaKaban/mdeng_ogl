#pragma once

#include <cstdint>

namespace hrs
{
    namespace size_literals
    {
        constexpr std::uint64_t operator""_B(unsigned long long bytes) noexcept
        {
            return bytes;
        }

        constexpr std::uint64_t operator""_KiB(unsigned long long kbytes) noexcept
        {
            return kbytes * 1024;
        }

        constexpr std::uint64_t operator""_MiB(unsigned long long mbytes) noexcept
        {
            return mbytes * 1024 * 1024;
        }

        constexpr std::uint64_t operator""_GiB(unsigned long long gbytes) noexcept
        {
            return gbytes * 1024 * 1024 * 1024;
        }
    }
};