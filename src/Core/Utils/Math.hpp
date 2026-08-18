#pragma once

#include <math.h>
#include "Types.hpp"
#include "Traits.hpp"
#include "Utility.hpp"

namespace Core
{
    constexpr inline Float64 Pi = 3.1415926535897932384626433832795;

    template<FloatingPoint F>
    constexpr F DegreesToRadians(F deg) noexcept
    {
        return (deg * static_cast<F>(Pi)) / static_cast<F>(180.0);
    }

    template<FloatingPoint F>
    constexpr F RadiansToDegrees(F rad) noexcept
    {
        return (rad * static_cast<F>(180.0)) / static_cast<F>(Pi);
    }

    template<FloatingPoint F>
    F Sine(F rads) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return sinf(rads);
        else
            return sin(rads);
    }

    template<FloatingPoint F>
    F Cosine(F rads) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return cosf(rads);
        else
            return cos(rads);
    }

    template<FloatingPoint F>
    F Tangent(F rads) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return tanf(rads);
        else
            return tan(rads);
    }

    template<FloatingPoint F>
    F ArcSine(F sin) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return asinf(sin);
        else
            return asin(sin);
    }

    template<FloatingPoint F>
    F ArcCosine(F cos) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return acosf(cos);
        else
            return acos(cos);
    }

    template<FloatingPoint F>
    F ArcTangent(F tan) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return atanf(tan);
        else
            return atan(tan);
    }

    template<FloatingPoint F>
    F SquareRoot(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return sqrtf(value);
        else
            return sqrt(value);
    }

    template<FloatingPoint F>
    F Absolute(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return fabsf(value);
        else
            return fabs(value);
    }

    template<FloatingPoint F>
    F Power(F base, F exponent) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return powf(base, exponent);
        else
            return pow(base, exponent);
    }

    template<FloatingPoint F>
    F Ceil(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return ceilf(value);
        else
            return ceil(value);
    }

    template<FloatingPoint F>
    F Floor(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return floorf(value);
        else
            return floor(value);
    }

    template<FloatingPoint F>
    F Round(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return roundf(value);
        else
            return round(value);
    }

    template<FloatingPoint F>
    F Truncate(F value) noexcept
    {
        if constexpr(SameAs<F, Float32>)
            return truncf(value);
        else
            return trunc(value);
    }

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