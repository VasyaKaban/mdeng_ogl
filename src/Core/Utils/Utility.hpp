#pragma once

#include "Traits.hpp"
#include <cstring>

namespace Core
{
    template<typename It>
    struct InsertResult
    {
        It it;
        Bool inserted;
    };

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
    constexpr Void Swap(T& value1, T& value2) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T>)
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

    template<typename T, typename C>
    requires InvocableWithResult<C, Bool, const T&, const T&>
    constexpr const T& Select(const T& value1, const T& value2, C&& comparator) noexcept(NoexceptInvocableWithResult<C, Bool, T&&, T&&>)
    {
        if(Forward(comparator)(value1, value2))
            return value1;

        return value2;
    }

    template<typename T>
    requires LessComparable<T, T>
    constexpr const T& Min(const T& value1, const T& value2) noexcept
    {
        if(value1 < value2)
            return value1;

        return value2;
    }

    template<typename T, DeviceSize N>
    requires LessComparable<T, T> && (N > 0)
    constexpr const T& Min(const T (&values)[N]) noexcept
    {
        if constexpr(N == 1)
            return values[0];
        else
        {
            DeviceSize target_index = 0;
            for(DeviceSize i = 1; i < N; i++)
            {
                if(values[target_index] > values[i])
                    target_index = i;
            }

            return values[target_index];
        }
    }

    template<typename T>
    requires LessComparable<T, T>
    constexpr const T& Max(const T& value1, const T& value2) noexcept
    {
        if(value1 < value2)
            return value2;

        return value1;
    }

    template<typename T, DeviceSize N>
    requires LessComparable<T, T> && (N > 0)
    constexpr const T& Max(const T (&values)[N]) noexcept
    {
        if constexpr(N == 1)
            return values[0];
        else
        {
            DeviceSize target_index = 0;
            for(DeviceSize i = 1; i < N; i++)
            {
                if(values[target_index] < values[i])
                    target_index = i;
            }

            return values[target_index];
        }
    }

    inline Void CopyNonOverlappedMemory(const Void* src, Void* dst, DeviceSize size) noexcept
    {
        memcpy(dst, src, size);
    }

    inline Void CopyOverlappedMemory(const Void* src, Void* dst, DeviceSize size) noexcept
    {
        memmove(dst, src, size);
    }
};