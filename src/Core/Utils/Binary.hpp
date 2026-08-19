#pragma once

#include <cassert>
#include "Traits.hpp"
#include "Math.hpp"
#include "Platform.h"

#if CORE_ARCH_CURRENT == CORE_ARCH_AMD64
#    include <nmmintrin.h>
#endif

namespace Core
{
    template<UnsignedIntegral I>
    constexpr Bool IsPowerOf2(I value) noexcept
    {
        if(value == 0)
            return false;

        //0b100 -> 0b011
        //0b101 -> 0b100
        return (value & (value - 1)) == 0;
    }

    template<UnsignedIntegral I>
    constexpr I GetPowerOf2(I value) noexcept
    {
        if(IsPowerOf2(value))
            return value;

        I output = 0;
        while(value != 0)
        {
            output <<= 1;
            output |= 0b1;

            value >>= 1;
        }

        return output + 1;
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

        if(size % alignment == 0)
            return true;

        auto div = size / alignment;
        if(!SaturatingAdd(div, static_cast<decltype(div)>(1), div))
            return false;

        return SaturatingMul(div, alignment, size);
    }

    template<UnsignedIntegral I>
    constexpr Bool AlignAddress(UInt8*& address, I buffer_size, I required_alignment) noexcept
    {
        assert(required_alignment != 0);

        DeviceSize int_address = *reinterpret_cast<DeviceSize*>(address);
        auto rem = int_address % required_alignment;
        if(rem > buffer_size)
            return false;

        int_address += rem;
        address = reinterpret_cast<UInt8*>(int_address);

        return true;
    }

    template<UnsignedIntegral I>
    constexpr Bool IsAlignedAddress(UInt8* address, I alignment) noexcept
    {
        assert(alignment != 0);

        DeviceSize int_address = *reinterpret_cast<DeviceSize*>(address);

        return int_address % alignment == 0;
    }

    namespace Detail
    {
        template<UnsignedIntegral I>
        constexpr DeviceSize CountBitsImpl(I value) noexcept
        {
            DeviceSize out = 0;

            for(; value != 0; value >>= 1)
            {
                if(value & 0b1)
                    out++;
            }

            return out;
        }
    };

    template<UnsignedIntegral I>
    constexpr DeviceSize CountBits(I value) noexcept
    {
        if constexpr(IsConstantContext())
            return Detail::CountBitsImpl(value);
        else
        {
#if CORE_ARCH_CURRENT == CORE_ARCH_AMD64
            return _mm_popcnt_u64(value);
#else
            return Detail::CountBitsImpl(value);
#endif
        }
    }
};