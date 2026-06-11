#pragma once

#include <type_traits>

namespace Core
{
    template<typename T, template<typename...> typename D>
    struct IsTypeInstantiation
    {
        constexpr static bool VALUE = false;
    };

    template<typename... Args, template<typename...> typename D>
    struct IsTypeInstantiation<D<Args...>, D>
    {
        constexpr static bool VALUE = true;
    };

    template<typename T, template<typename...> typename D>
    concept TypeInstantiation = IsTypeInstantiation<T, D>::VALUE;

    template<typename T, template<auto...> typename D>
    struct IsNonTypeInstantiation
    {
        constexpr static bool VALUE = false;
    };

    template<auto... Args, template<auto...> typename D>
    struct IsNonTypeInstantiation<D<Args...>, D>
    {
        constexpr static bool VALUE = true;
    };

    template<typename T, template<auto...> typename D>
    concept NonTypeInstantiation = IsNonTypeInstantiation<T, D>::VALUE;
};
