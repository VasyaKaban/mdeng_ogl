#pragma once

#include <type_traits>

namespace Core
{
    template<typename T, template<typename...> typename D>
    struct IsTypeInstantiation : std::false_type
    {};

    template<typename... Args, template<typename...> typename D>
    struct IsTypeInstantiation<D<Args...>, D> : std::true_type
    {};

    template<typename T, template<typename...> typename D>
    concept TypeInstantiation = IsTypeInstantiation<T, D>::value;

    template<typename T, template<auto...> typename D>
    struct IsNonTypeInstantiation : std::false_type
    {};

    template<auto... Args, template<auto...> typename D>
    struct IsNonTypeInstantiation<D<Args...>, D> : std::true_type
    {};

    template<typename T, template<auto...> typename D>
    concept NonTypeInstantiation = IsNonTypeInstantiation<T, D>::value;
};
