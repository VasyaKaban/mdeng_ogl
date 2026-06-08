#pragma once

#include <type_traits>

namespace Core
{
    template<typename T, template<typename...> typename D>
    struct IsTypeInstantiation
    {
        constexpr static bool Value = false;
    };

    template<typename... Args, template<typename...> typename D>
    struct IsTypeInstantiation<D<Args...>, D>
    {
        constexpr static bool Value = true;
    };

    template<typename T, template<typename...> typename D>
    concept TypeInstantiation = IsTypeInstantiation<T, D>::Value;

    template<typename T, template<auto...> typename D>
    struct IsNonTypeInstantiation : std::false_type
    {
        constexpr static bool Value = false;
    };

    template<auto... Args, template<auto...> typename D>
    struct IsNonTypeInstantiation<D<Args...>, D> : std::true_type
    {
        constexpr static bool Value = true;
    };

    template<typename T, template<auto...> typename D>
    concept NonTypeInstantiation = IsNonTypeInstantiation<T, D>::Value;
};
