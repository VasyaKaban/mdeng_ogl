#pragma once

#include <concepts>
#include "Binary.hpp"
#include "Traits.hpp"

namespace Core
{
    namespace Detail
    {
        struct ExpectedMetrics
        {
            size_t alignment;
            size_t size;
        };

        template<typename T, typename E>
        constexpr ExpectedMetrics GetExpectedMetrics() noexcept
        {
            ExpectedMetrics metrics = {.alignment = alignof(T), .size = sizeof(T)};
            if(metrics.alignment < alignof(E))
                metrics.alignment = alignof(E);

            if(metrics.size < sizeof(E))
                metrics.size = sizeof(E);

            Align(metrics.size, metrics.alignment);

            return metrics;
        }
    };

    template<typename T, typename E>
    requires(!std::is_reference_v<T> && !std::is_volatile_v<T>) && (!std::is_reference_v<E> && !std::is_volatile_v<E>)
    class Expected
    {
        constexpr static Detail::ExpectedMetrics METRICS = Detail::GetExpectedMetrics<T, E>();
    public:
        Expected() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::is_default_constructible_v<T>
        {
            new(this->data) T;
            this->is_value_created = true;
        }

        ~Expected()
        {
            if(this->is_value_created)
                reinterpret_cast<T*>(this->data)->~T();
            else
                reinterpret_cast<E*>(this->data)->~E();
        }

        Expected(const Expected& ex) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<E>)
        requires std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E>
        {
            if(ex.is_value_created)
                new(this->data) T(*reinterpret_cast<const T*>(ex.data));
            else
                new(this->data) E(*reinterpret_cast<const E*>(ex.data));

            this->is_value_created = ex.is_value_created;
        }

        Expected(Expected&& ex) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
        requires std::is_move_constructible_v<T> && std::is_move_constructible_v<E>
        {
            if(ex.is_value_created)
                new(this->data) T(std::move(*reinterpret_cast<const T*>(ex.data)));
            else
                new(this->data) E(std::move(*reinterpret_cast<const E*>(ex.data)));

            this->is_value_created = ex.is_value_created;
        }

        Expected& operator=(const Expected& ex) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<E>)
        requires std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E> && std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<E>
        {
            this->~Expected();

            if(ex.is_value_created)
                new(this->data) T(ex.GetValue());
            else
                new(this->data) E(ex.GetError());

            this->is_value_created = ex.is_value_created;

            return *this;
        }

        Expected& operator=(Expected&& ex) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
        requires std::is_move_constructible_v<T> && std::is_move_constructible_v<E> && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>
        {
            this->~Expected();

            if(ex.is_value_created)
                new(this->data) T(std::move(ex).GetValue());
            else
                new(this->data) E(std::move(ex).GetError());

            this->is_value_created = ex.is_value_created;

            return *this;
        }

        Expected(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::is_copy_constructible_v<T>
        {
            new(this->data) T(value);
            this->is_value_created = true;
        }

        Expected(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::is_move_constructible_v<T>
        {
            new(this->data) E(std::move(value));
            this->is_value_created = true;
        }

        Expected(const E& error) noexcept(std::is_nothrow_copy_constructible_v<E>)
        requires std::is_copy_constructible_v<E>
        {
            new(this->data) E(error);
            this->is_value_created = false;
        }

        Expected(E&& error) noexcept(std::is_nothrow_move_constructible_v<E>)
        requires std::is_move_constructible_v<E>
        {
            new(this->data) E(std::move(error));
            this->is_value_created = false;
        }

        template<typename U>
        requires std::constructible_from<T, U> && std::is_nothrow_constructible_v<T, U>
        Expected(InPlaceType<T>, U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            new(this->data) T(std::forward<T>(value));
            this->is_value_created = true;
        }

        template<typename U>
        requires std::constructible_from<E, U> && std::is_nothrow_constructible_v<E, U>
        Expected(InPlaceType<E>, U&& value) noexcept(std::is_nothrow_constructible_v<E, U>)
        {
            new(this->data) E(std::forward<E>(value));
            this->is_value_created = false;
        }

        Expected& operator=(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::is_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<T>
        {
            this->~Expected();

            new(this->data) T(value);

            this->is_value_created = true;

            return *this;
        }

        Expected& operator=(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::is_move_constructible_v<T> && std::is_nothrow_move_constructible_v<T>
        {
            this->~Expected();

            new(this->data) T(std::move(value));

            this->is_value_created = true;

            return *this;
        }

        Expected& operator=(const E& error) noexcept(std::is_nothrow_copy_constructible_v<E>)
        requires std::is_copy_constructible_v<E> && std::is_nothrow_copy_constructible_v<E>
        {
            this->~Expected();

            new(this->data) E(error);

            this->is_value_created = false;

            return *this;
        }

        Expected& operator=(E&& error) noexcept(std::is_nothrow_move_constructible_v<E>)
        requires std::is_move_constructible_v<E> && std::is_nothrow_move_constructible_v<E>
        {
            this->~Expected();

            new(this->data) E(std::move(error));

            this->is_value_created = false;

            return *this;
        }

        bool HasValue() const noexcept
        {
            return this->is_value_created;
        }

        bool HasError() const noexcept
        {
            return !this->is_value_created;
        }

        T& GetValue() & noexcept
        {
            return (*this).operator*();
        }

        const T& GetValue() const& noexcept
        {
            return (*this).operator*();
        }

        T&& GetValue() && noexcept
        {
            return std::move(*this).operator*();
        }

        const T&& GetValue() const&& noexcept
        {
            return std::move(*this).operator*();
        }

        E& GetError() & noexcept
        {
            assert(!this->is_value_created);

            return *reinterpret_cast<E*>(this->data);
        }

        const E& GetError() const& noexcept
        {
            assert(!this->is_value_created);

            return *reinterpret_cast<const E*>(this->data);
        }

        E&& GetError() && noexcept
        {
            assert(!this->is_value_created);

            return std::move(*reinterpret_cast<E*>(this->data));
        }

        const E&& GetError() const&& noexcept
        {
            assert(!this->is_value_created);

            return std::move(*reinterpret_cast<const E*>(this->data));
        }

        explicit operator bool() const noexcept
        {
            return this->is_value_created;
        }

        T& operator*() & noexcept
        {
            assert(this->is_value_created);

            return *reinterpret_cast<T*>(this->data);
        }

        const T& operator*() const& noexcept
        {
            assert(this->is_value_created);

            return *reinterpret_cast<const T*>(this->data);
        }

        T&& operator*() && noexcept
        {
            assert(this->is_value_created);

            return std::move(*reinterpret_cast<T*>(this->data));
        }

        const T&& operator*() const&& noexcept
        {
            assert(this->is_value_created);

            return std::move(*reinterpret_cast<const T*>(this->data));
        }

        T* operator->() noexcept
        {
            assert(this->is_value_created);

            return reinterpret_cast<T*>(this->data);
        }

        const T* operator->() const noexcept
        {
            assert(this->is_value_created);

            return reinterpret_cast<const T*>(this->data);
        }
    private:
        alignas(METRICS.alignment) char data[METRICS.size];
        bool is_value_created;
    };
};