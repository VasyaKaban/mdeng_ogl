#pragma once

#include <concepts>

namespace Core
{
    template<std::unsigned_integral I>
    constexpr bool IsPowerOf2(I value) noexcept
    {
        if(value == 0)
            return false;

        //0b100 -> 0b011
        //0b101 -> 0b100
        return (value & (value - 1)) == 0;
    }
};