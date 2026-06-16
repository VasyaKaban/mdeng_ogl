#pragma once

#include <cstddef>
#include <utility>
#include <concepts>

namespace Core
{
    namespace Detail
    {
        template<size_t TargetIndex, size_t CurrentIndex, typename T, typename... Types>
        struct TypeOfIndex
        {
            using Type = TypeOfIndex<TargetIndex, CurrentIndex + 1, Types...>::Type;
        };

        template<size_t CurrentIndex, typename T, typename... Types>
        struct TypeOfIndex<CurrentIndex, CurrentIndex, T, Types...>
        {
            using Type = T;
        };

        template<typename T, size_t CurrentIndex, typename C, typename... Types>
        struct IndexOfType
        {
            constexpr static size_t INDEX = IndexOfType<T, CurrentIndex + 1, Types...>::INDEX;
        };

        template<size_t CurrentIndex, typename C, typename... Types>
        struct IndexOfType<C, CurrentIndex, C, Types...>
        {
            constexpr static size_t INDEX = CurrentIndex;
        };
    };

    template<typename... Types>
    struct Variadic
    {
        constexpr static std::size_t COUNT = sizeof...(Types);

        template<std::size_t Index>
        requires(Index < COUNT)
        using TypeOfIndex = Detail::TypeOfIndex<Index, 0, Types...>::Type;

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        constexpr static size_t IndexOfType = Detail::IndexOfType<T, 0, Types...>::INDEX;
    };
};
