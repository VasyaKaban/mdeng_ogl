#pragma once

#include <cstddef>
#include <utility>

namespace Core
{
    namespace Detail
    {
        template<std::size_t N, typename T, typename... Args>
        requires(N < sizeof...(Args) + 1)
        struct nth
        {
            using type = nth<N - 1, Args...>::type;
        };

        template<typename T, typename... Args>
        struct nth<0, T, Args...>
        {
            using type = T;
        };

        template<std::size_t N, typename T, typename... Args>
        requires(N < sizeof...(Args) + 1)
        using nth_t = nth<N, T, Args...>;
    };

    template<typename... Args>
    struct Variadic
    {
        constexpr static std::size_t COUNT = sizeof...(Args);

        template<std::size_t Index>
        requires(Index < COUNT)
        using Nth = Detail::nth<Index, Args...>::type;
    };

    template<std::size_t Index, typename A, typename... Args>
    constexpr auto& NthArgument(A&& arg, Args&&... args) noexcept
    {
        if constexpr(Index == 0)
            return arg;
        else
            return NthArgument<Index - 1>(std::forward<Args>(args)...);
    }
};
