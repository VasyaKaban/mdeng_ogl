#pragma once

#include "Types.hpp"
#include "Traits.hpp"

namespace Core
{
    template<typename T>
    concept Hashable = requires(const T& obj) {
        { Hash(obj) } -> SameAs<DeviceSize>;
    };
};