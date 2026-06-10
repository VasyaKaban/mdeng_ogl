#pragma once

namespace Core
{
    template<auto Val>
    struct NonTypeArgument
    {
        constexpr NonTypeArgument() = default;
    };

    template<auto Val>
    constexpr inline NonTypeArgument<Val> NonType{};
};