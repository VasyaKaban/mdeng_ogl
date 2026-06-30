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
};