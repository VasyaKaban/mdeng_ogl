#pragma once

#include "Traits.hpp"

#ifdef _MSC_VER
#    define CORE_ASSUME(COND, ...) __assume(COND __VA_OPT__(__VA_ARGS__));
#    define CORE_UNREACHABLE() __assume(0);
#else
#    define CORE_ASSUME(COND, ...) [[gnu::assume(COND __VA_OPT__(__VA_ARGS__))]];
#    define CORE_UNREACHABLE() __builtin_unreachable();
#endif

#define CORE_ASSUME_ALIGNED(PTR, ALIGNMENT, ...) CORE_ASSUME(reinterpret_cast<DeviceSize>(PTR) % (ALIGNMENT __VA_OPT__(__VA_ARGS__)) == 0)

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

    template<typename T>
    constexpr T* GetAddress(T& value) noexcept
    {
        static_assert(!(Complex<T> && requires { value.operator&(); }), "Complex types with overloaded operator& are not supported. Use wrapper or remove operator& overloading");

        return &value;
    }
};