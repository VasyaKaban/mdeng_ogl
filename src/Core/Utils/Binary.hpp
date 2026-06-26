#pragma once

#include <cassert>
#include "Traits.hpp"
#include "Utility.hpp"

#if defined(__x86_64__) || defined(_M_X64)
#    include <nmmintrin.h>
#endif

namespace Core
{
    template<UnsignedIntegral I>
    constexpr bool IsPowerOf2(I value) noexcept
    {
        if(value == 0)
            return false;

        //0b100 -> 0b011
        //0b101 -> 0b100
        return (value & (value - 1)) == 0;
    }

    template<UnsignedIntegral I>
    constexpr Bool Align(I& size, I alignment) noexcept
    {
        assert(alignment != 0);

        if(alignment >= size)
        {
            size = alignment;
            return true;
        }

        auto div = size / alignment;
        auto quat = size % alignment;
        if(quat == 0)
            return true;

        auto max_div = NumericLimits<I>::Max / alignment;
        if(max_div < div + 1)
            return false;

        size = (div + 1) * alignment;

        return true;
    }

    namespace Detail
    {
        template<UnsignedIntegral I>
        constexpr I CountBitsPlain(I value) noexcept
        {
            I result = 0;
            while(value)
            {
                if(value & 0b1)
                    result++;

                value >>= 1;
            }

            return result;
        }
    };

    template<UnsignedIntegral I>
    constexpr I CountBits(I value) noexcept
    {
        I result = 0;
        if(IsConstantContext())
        {
            result = Detail::CountBitsPlain(value);
        }
        else
        {
#pragma message("Yes, we use SSE4.2 popcnt instruction. Check appropriate compiler flags to make sure that we can use it safely(set x86-64 arch level)")
#if defined(__x86_64__) || defined(_M_X64)
            result = _mm_popcnt_u64(value);

#else
            result = Detail::CountBitsPlain(value);
#endif
        }

        return result;
    }
};