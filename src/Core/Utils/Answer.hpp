#pragma once

#include <cassert>
#include "Traits.hpp"

namespace Core
{
    template<typename T>
    requires(!Reference<T> && !Volatile<T>)
    class Answer
    {
    public:
        Answer() noexcept
            : is_created(false)
        {}

        Answer(const T& value) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T>
        {
            new(this->data) T(value);
            this->is_created = true;
        }

        Answer(T&& value) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T>
        {
            new(this->data) T(Move(value));
            this->is_created = true;
        }

        template<typename... Args>
        Answer(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
        requires Constructible<T, Args...>
        {
            new(this->data) T(Forward(args)...);
            this->is_created = true;
        }

        ~Answer()
        {
            if(this->is_created)
                reinterpret_cast<T*>(this->data)->~T();
        }

        Answer(const Answer& null) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T>
            : is_created(null.is_created)
        {
            if(null.is_created)
                new(this->data) T(null.GetValue());
        }

        Answer(Answer&& null) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T>
            : is_created(null.is_created)
        {
            if(null.is_created)
                new(this->data) T(Move(null).GetValue());
        }

        Answer& operator=(const Answer& null) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T>
        {
            this->is_created = null.is_created;

            if(null.is_created)
                new(this->data) T(null.GetValue());
        }

        Answer& operator=(Answer&& null) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T>
        {
            this->is_created = null.is_created;

            if(null.is_created)
                new(this->data) T(Move(null).GetValue());
        }

        Answer& operator=(const T& value) noexcept(NoexceptCopyConstructible<T>)
        requires CopyConstructible<T>
        {
            this->Clear();

            new(this->data) T(value);
            this->is_created = true;

            return *this;
        }

        Answer& operator=(T&& value) noexcept(NoexceptMoveConstructible<T>)
        requires MoveConstructible<T>
        {
            this->Clear();

            new(this->data) T(Move(value));
            this->is_created = true;

            return *this;
        }

        template<typename U>
        Answer& operator=(U&& value) noexcept(NoexceptConstructible<T, U>)
        requires Constructible<T, U>
        {
            this->Clear();

            new(this->data) T(Forward(value));
            this->is_created = true;

            return *this;
        }

        template<typename... Args>
        Void CreateInPlace(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
        requires Constructible<T, Args...>
        {
            this->Clear();

            new(this->data) T(Forward(args)...);
            this->is_created = true;
        }

        Bool HasValue() const noexcept
        {
            return this->is_created;
        }

        Void Clear() noexcept
        {
            if(this->is_created)
            {
                reinterpret_cast<T*>(this->data)->~T();

                this->is_created = false;
            }
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

        explicit operator Bool() const noexcept
        {
            return this->is_created;
        }

        T& operator*() & noexcept
        {
            assert(this->is_created);

            return *reinterpret_cast<T*>(this->data);
        }

        const T& operator*() const& noexcept
        {
            assert(this->is_created);

            return *reinterpret_cast<const T*>(this->data);
        }

        T&& operator*() && noexcept
        {
            assert(this->is_created);

            return Move(*reinterpret_cast<T*>(this->data));
        }

        const T&& operator*() const&& noexcept
        {
            assert(this->is_created);

            return Move(*reinterpret_cast<const T*>(this->data));
        }

        T* operator->() noexcept
        {
            assert(this->is_created);

            return reinterpret_cast<T*>(this->data);
        }

        const T* operator->() const noexcept
        {
            assert(this->is_created);

            return reinterpret_cast<const T*>(this->data);
        }
    private:
        alignas(alignof(T)) UInt8 data[sizeof(T)];
        Bool is_created;
    };

    template<typename T>
    Answer(T&&) -> Answer<DropReference<T>>;
};