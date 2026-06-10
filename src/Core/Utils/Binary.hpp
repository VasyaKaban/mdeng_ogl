#pragma once

#include <cassert>
#include <concepts>
#include <limits>

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

    template<std::unsigned_integral I>
    constexpr bool Align(I& size, I alignment) noexcept
    {
        assert(alignment != 0 && size != 0);

        if(alignment >= size)
        {
            size = alignment;
            return true;
        }

        auto div = size / alignment;
        if(div == size)
            return true;

        auto max_div = std::numeric_limits<I>::max() / alignment;
        if(max_div < div + 1)
            return false;

        size = (div + 1) * alignment;

        return true;
    }
};