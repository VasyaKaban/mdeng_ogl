#pragma once

#include <utility>
#include <limits>
#include <cassert>
#include "immintrin.h"

namespace Core
{
    template<typename T, typename C>
    requires std::is_invocable_r_v<bool, C, const T&, const T&>
    constexpr const T& Clamp(const T& min, const T& max, const T& value, C&& comparator) noexcept(std::is_nothrow_invocable_r_v<bool, C, const T&, const T&>)
    {
        if(std::forward<C>(comparator)(value, min))
            return min;
        else if(std::forward<C>(comparator)(max, value))
            return max;
        else
            return value;
    }
};