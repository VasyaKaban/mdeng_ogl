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
    concept SizedRange = Range<T> && requires(T& rng) {
        { size(rng) } -> Integral;
    };

    template<Range T>
    using RangeDereferenceType = decltype(*begin(DeclareValue<T>()));

    template<Range T>
    using RangeIterator = DropReference<decltype(begin(DeclareValue<T>()))>;

    template<Range T>
    using RangeSentinel = DropReference<decltype(end(DeclareValue<T>()))>;

    template<typename T>
    concept ForwardIterator = requires(T iter) {
        { iter++ } -> SameAs<T>;
        { ++iter } -> SameAs<T&>;
        { iter == iter } -> SameAs<Bool>;
    };

    template<typename T>
    concept BidirectionalIterator = ForwardIterator<T> && requires(T iter) {
        { iter-- } -> SameAs<T>;
        { --iter } -> SameAs<T&>;
    };

    template<typename T, typename I>
    concept Sentinel = requires(I iter, T sent) {
        { sent != iter } -> SameAs<Bool>;
    };
};