/**
 * @file
 *
 * Represents scoped_call class
 */

#pragma once

#include <concepts>
#include <optional>
#include <utility>

namespace Core
{
    template<std::invocable F>
    class ScopedCall
    {
    public:
        constexpr ScopedCall() = default;

        constexpr ScopedCall(const F& _func) noexcept(std::is_nothrow_copy_constructible_v<F>)
        requires std::is_copy_constructible_v<F>
            : func(_func)
        {}

        constexpr ScopedCall(F&& _func) noexcept(std::is_nothrow_move_constructible_v<F>)
        requires std::is_move_constructible_v<F>
            : func(std::move(_func))
        {}

        constexpr ~ScopedCall()
        {
            if(func)
                func.value()();
        }

        constexpr ScopedCall(const ScopedCall& d_destroy) noexcept(
            std::is_nothrow_copy_constructible_v<F>)
        requires std::is_copy_constructible_v<F>
            : func(d_destroy.func)
        {}

        constexpr ScopedCall(ScopedCall&& d_destroy) noexcept(
            std::is_nothrow_move_constructible_v<F>)
        requires std::is_move_constructible_v<F>
            : func(std::move(d_destroy.func))
        {}

        ScopedCall&
        operator=(const ScopedCall& d_destroy) noexcept(std::is_nothrow_copy_assignable_v<F>)
        requires std::is_copy_assignable_v<F>
        {
            func = d_destroy.func;
            return *this;
        }

        ScopedCall& operator=(const F& _func) noexcept(std::is_nothrow_copy_assignable_v<F>)
        requires std::is_copy_assignable_v<F>
        {
            func = _func;
            return *this;
        }

        ScopedCall& operator=(F&& _func) noexcept(std::is_nothrow_move_assignable_v<F>)
        requires std::is_move_assignable_v<F>
        {
            func = std::move<F>(_func);
            return *this;
        }

        ScopedCall& operator=(ScopedCall&& d_destroy) noexcept(std::is_nothrow_move_assignable_v<F>)
        requires std::is_move_assignable_v<F>
        {
            func = std::move(d_destroy.func);
            return *this;
        }

        ScopedCall& operator=(const std::nullopt_t&) noexcept
        {
            func.reset();
            return *this;
        }

        constexpr explicit operator bool() const noexcept
        {
            return func;
        }

        constexpr void Drop() noexcept
        {
            func = std::nullopt;
        }

        constexpr void Call() const noexcept(std::is_nothrow_invocable_v<F>)
        {
            if(func)
                func.value()();
        }
    private:
        std::optional<F> func;
    };
};
