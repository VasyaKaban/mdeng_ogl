#pragma once

#include "Types.hpp"
#include "Traits.hpp"
#include "Utility.hpp"

namespace Core
{
    template<typename T, typename C>
    requires InvocableWithResult<Bool, C, const T&, const T&>
    constexpr const T& Clamp(const T& min, const T& max, const T& value, C&& comparator) noexcept(NoexceptInvocableWithResult<Bool, C, const T&, const T&>)
    {
        if(Forward(comparator)(value, min))
            return min;
        else if(Forward(comparator)(max, value))
            return max;
        else
            return value;
    }

    //true -> no-saturating happened
    template<UnsignedIntegral I>
    constexpr Bool SaturatingAdd(I value1, I value2, I& result) noexcept
    {
        if(NumericLimits<I>::Max - value2 >= value1)
        {
            result = value1 + value2;
            return true;
        }

        result = NumericLimits<I>::Max;
        return false;
    }

    template<UnsignedIntegral I>
    constexpr Bool SaturatingSub(I value1, I value2, I& result) noexcept
    {
        if(value1 >= value2)
        {
            result = value1 - value2;
            return true;
        }

        result = 0;
        return false;
    }

    template<UnsignedIntegral I>
    constexpr Bool SaturatingMul(I value1, I value2, I& result) noexcept
    {
        if(value1 == 0 || value2 == 0)
        {
            result = 0;
            return true;
        }

        I rem = NumericLimits<I>::Max / value2;

        if(rem >= value1)
        {
            result = value1 * value2;
            return true;
        }

        result = NumericLimits<I>::Max;
        return false;
    }
};