#pragma once

#include "Traits.hpp"

namespace Core
{
    constexpr Bool IsConstantContext() noexcept
    {
        return __builtin_is_constant_evaluated();
    }

    template<typename T>
    constexpr DropReference<T>&& Move(T&& value) noexcept
    {
        return static_cast<DropReference<T>&&>(value);
    }

    template<typename T>
    constexpr T&& Forward(T&& value) noexcept
    {
        if constexpr(LValueReference<T>)
            return value;
        else
            return static_cast<DropReference<T>&&>(value);
    }

    template<typename T, typename U>
    requires MoveConstructible<T> && Assignable<T, U>
    constexpr T Exchange(T& old_value, U&& new_value) noexcept(NoexceptMoveConstructible<T> && NoexceptAssignable<T, U>)
    {
        T to_return(Move(old_value));
        old_value = Forward(new_value);
        return to_return;
    }

    template<typename T>
    requires MoveConstructible<T> && MoveAssignable<T>
    constexpr void Swap(T& value1, T& value2) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T>)
    {
        T tmp_value(Move(value1));
        value1 = Move(value2);
        value2 = Move(tmp_value);
    }
};