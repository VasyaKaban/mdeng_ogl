#pragma once

#include "Types.hpp"
#include "Traits.hpp"

namespace Core
{
    namespace Detail
    {
        template<DeviceSize TargetIndex, DeviceSize CurrentIndex, typename T, typename... Types>
        struct TypeOfIndex
        {
            using Type = TypeOfIndex<TargetIndex, CurrentIndex + 1, Types...>::Type;
        };

        template<DeviceSize CurrentIndex, typename T, typename... Types>
        struct TypeOfIndex<CurrentIndex, CurrentIndex, T, Types...>
        {
            using Type = T;
        };

        template<typename T, DeviceSize CurrentIndex, typename C, typename... Types>
        struct IndexOfType
        {
            constexpr static DeviceSize Index = IndexOfType<T, CurrentIndex + 1, Types...>::Index;
        };

        template<DeviceSize CurrentIndex, typename C, typename... Types>
        struct IndexOfType<C, CurrentIndex, C, Types...>
        {
            constexpr static DeviceSize Index = CurrentIndex;
        };
    };

    template<typename... Types>
    struct Variadic
    {
        constexpr static DeviceSize Count = sizeof...(Types);

        template<DeviceSize Index>
        requires(Index < Count)
        using TypeOfIndex = Detail::TypeOfIndex<Index, 0, Types...>::Type;

        template<typename T>
        requires(SameAs<T, Types> || ...)
        constexpr static DeviceSize IndexOfType = Detail::IndexOfType<T, 0, Types...>::Index;
    };
};
