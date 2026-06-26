#pragma once

#include "Traits.hpp"

namespace Core
{
    template<typename T>
    concept Range = requires(T& rng) {
        begin(rng);
        end(rng);
    };

    template<typename T>
    concept SizedRange = Range<T> && requires(T& rng) { size(rng); };

    template<Range T>
    using RangeDereferenceType = decltype(*begin(DeclareValue<T>()));
};