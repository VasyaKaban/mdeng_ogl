#pragma once

#include "Binary.hpp"
#include "Traits.hpp"

namespace Core
{
    namespace Detail
    {
        struct ExpectedMetrics
        {
            DeviceSize alignment;
            DeviceSize size;
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
    requires(!Reference<T> && !Volatile<T>) && (!Reference<E> && !Volatile<E>)
    class Expected
    {
        constexpr static Detail::ExpectedMetrics METRICS = Detail::GetExpectedMetrics<T, E>();
    public:
        Expected() noexcept(NoexceptDefaultConstructible<T>)
        requires DefaultConstructible<T>
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

        Expected(const Expected& ex) noexcept(NoexceptCopyConstructible<T> && NoexceptCopyConstructible<E>)
        requires CopyConstructible<T> && CopyConstructible<E>
        {
            if(ex.is_value_created)
                new(this->data) T(*reinterpret_cast<const T*>(ex.data));
            else
                new(this->data) E(*reinterpret_cast<const E*>(ex.data));

            this->is_value_created = ex.is_value_created;
        }

        Expected(Expected&& ex) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveConstructible<E>)
        requires MoveConstructible<T> && MoveConstructible<E>
        {
            if(ex.is_value_created)
                new(this->data) T(Move(*reinterpret_cast<const T*>(ex.data)));
            else
                new(this->data) E(Move(*reinterpret_cast<const E*>(ex.data)));

            this->is_value_created = ex.is_value_created;
        }

        Expected& operator=(const Expected& ex) noexcept(NoexceptCopyConstructible<T> && NoexceptCopyConstructible<E>)
        requires CopyConstructible<T> && CopyConstructible<E> && NoexceptCopyConstructible<T> && NoexceptCopyConstructible<E>
        {
            this->~Expected();

            if(ex.is_value_created)
                new(this->data) T(ex.GetValue());
            else
                new(this->data) E(ex.GetError());

            this->is_value_created = ex.is_value_created;

            return *this;
        }

        Expected& operator=(Expected&& ex) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveConstructible<E>)
        requires MoveConstructible<T> && MoveConstructible<E> && NoexceptMoveConstructible<T> && NoexceptMoveConstructible<E>
        {
            this->~Expected();

            if(ex.is_value_created)
                new(this->data) T(Move(ex).GetValue());
            else
                new(this->data) E(Move(ex).GetError());

            this->is_value_created = ex.is_value_created;

            return *this;
        }

        Expected(const T& value) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T>
        {
            new(this->data) T(value);
            this->is_value_created = true;
        }

        Expected(T&& value) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T>
        {
            new(this->data) E(Move(value));
            this->is_value_created = true;
        }

        Expected(const E& error) noexcept(NoexceptCopyConstructible<E>)
        requires CopyConstructible<E>
        {
            new(this->data) E(error);
            this->is_value_created = false;
        }

        Expected(E&& error) noexcept(NoexceptMoveConstructible<E>)
        requires MoveConstructible<E>
        {
            new(this->data) E(Move(error));
            this->is_value_created = false;
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Expected(InPlaceType<T>, Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
        {
            new(this->data) T(Forward(args)...);
            this->is_value_created = true;
        }

        template<typename... Args>
        requires Constructible<E, Args...>
        Expected(InPlaceType<E>, Args&&... args) noexcept(NoexceptConstructible<E, Args...>)
        {
            new(this->data) E(Forward(args)...);
            this->is_value_created = false;
        }

        Expected& operator=(const T& value) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T> && NoexceptCopyConstructible<T>
        {
            this->~Expected();

            new(this->data) T(value);

            this->is_value_created = true;

            return *this;
        }

        Expected& operator=(T&& value) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T> && NoexceptMoveConstructible<T>
        {
            this->~Expected();

            new(this->data) T(Move(value));

            this->is_value_created = true;

            return *this;
        }

        Expected& operator=(const E& error) noexcept(NoexceptCopyConstructible<E>)
        requires CopyConstructible<E> && NoexceptCopyConstructible<E>
        {
            this->~Expected();

            new(this->data) E(error);

            this->is_value_created = false;

            return *this;
        }

        Expected& operator=(E&& error) noexcept(NoexceptMoveConstructible<E>)
        requires MoveConstructible<E> && NoexceptMoveConstructible<E>
        {
            this->~Expected();

            new(this->data) E(Move(error));

            this->is_value_created = false;

            return *this;
        }

        Bool HasValue() const noexcept
        {
            return this->is_value_created;
        }

        Bool HasError() const noexcept
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
            return Move(*this).operator*();
        }

        const T&& GetValue() const&& noexcept
        {
            return Move(*this).operator*();
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

            return Move(*reinterpret_cast<E*>(this->data));
        }

        const E&& GetError() const&& noexcept
        {
            assert(!this->is_value_created);

            return Move(*reinterpret_cast<const E*>(this->data));
        }

        explicit operator Bool() const noexcept
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

            return Move(*reinterpret_cast<T*>(this->data));
        }

        const T&& operator*() const&& noexcept
        {
            assert(this->is_value_created);

            return Move(*reinterpret_cast<const T*>(this->data));
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
        alignas(METRICS.alignment) UInt8 data[METRICS.size];
        Bool is_value_created;
    };
};