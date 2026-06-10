#pragma once

#include <concepts>
#include <utility>

namespace Core
{
    template<std::invocable F>
    class ScopedCall
    {
    public:
        constexpr ScopedCall() noexcept
            : created(false)
        {}

        constexpr ScopedCall(const F& func) noexcept(std::is_nothrow_copy_constructible_v<F>)
        requires std::is_copy_constructible_v<F>
            : data{func},
              created(true)
        {}

        constexpr ScopedCall(F&& func) noexcept(std::is_nothrow_move_constructible_v<F>)
        requires std::is_move_constructible_v<F>
            : data(std::move(func)),
              created(true)
        {}

        constexpr ~ScopedCall()
        {
            if(created)
            {
                data.func();
                data.func.~F();
            }
        }

        constexpr ScopedCall(const ScopedCall& scall) noexcept(
            std::is_nothrow_copy_constructible_v<F>)
        requires std::is_copy_constructible_v<F>
            : data(scall.data),
              created(scall.created)
        {}

        constexpr ScopedCall(ScopedCall&& scall) noexcept(std::is_nothrow_move_constructible_v<F>)
        requires std::is_move_constructible_v<F>
            : data(std::move(scall.data)),
              created(std::exchange(scall.created, false))
        {}

        ScopedCall&
        operator=(const ScopedCall& scall) noexcept(std::is_nothrow_copy_assignable_v<F>)
        requires std::is_copy_assignable_v<F>
        {
            this->Drop();

            this->data = scall.data;
            this->created = scall.created;

            return *this;
        }

        ScopedCall& operator=(ScopedCall&& scall) noexcept(std::is_nothrow_move_assignable_v<F>)
        requires std::is_move_assignable_v<F>
        {
            this->Drop();

            this->data = std::move(scall.data);
            this->created = std::exchange(scall.created, false);

            return *this;
        }

        ScopedCall& operator=(const F& func) noexcept(std::is_nothrow_copy_assignable_v<F>)
        requires std::is_copy_assignable_v<F>
        {
            this->Drop();

            this->data = func;
            this->created = true;

            return *this;
        }

        ScopedCall& operator=(F&& func) noexcept(std::is_nothrow_move_assignable_v<F>)
        requires std::is_move_assignable_v<F>
        {
            this->Drop();

            this->data = std::move(func);
            this->created = true;

            return *this;
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->created;
        }

        constexpr void Drop() noexcept
        {
            if(this->created)
            {
                this->data.func.~F();
                this->created = false;
            }
        }

        constexpr void Call() const noexcept(std::is_nothrow_invocable_v<F>)
        {
            if(this->created)
                this->data.func();
        }
    private:
        union ScopedCallCallableWrapper
        {
            F func;

            ~ScopedCallCallableWrapper()
            {}
        } data;
        bool created;
    };
};