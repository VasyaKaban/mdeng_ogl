#pragma once

#include <concepts>

namespace Core
{
    template<typename T>
    requires(!std::is_reference_v<T>)
    class Nullable
    {
    public:
        Nullable() noexcept
            : is_created(false)
        {}

        Nullable(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
        {
            new(this->data) T(value);
            this->is_created = true;
        }

        Nullable(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
        {
            new(this->data) T(std::move(value));
            this->is_created = true;
        }

        template<typename... Args>
        Nullable(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        requires std::constructible_from<T, Args...>
        {
            new(this->data) T(std::forward<Args>(args)...);
            this->is_created = true;
        }

        ~Nullable()
        {
            if(this->is_created)
                reinterpret_cast<T*>(this->data)->~T();
        }

        Nullable(const Nullable& null) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
            : is_created(null.is_created)
        {
            if(null.is_created)
                new(this->data) T(null.GetValue());
        }

        Nullable(Nullable&& null) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
            : is_created(null.is_created)
        {
            if(null.is_created)
                new(this->data) T(std::move(null).GetValue());
        }

        Nullable& operator=(const Nullable& null) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
        {
            this->is_created = null.is_created;

            if(null.is_created)
                new(this->data) T(null.GetValue());
        }

        Nullable& operator=(Nullable&& null) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
        {
            this->is_created = null.is_created;

            if(null.is_created)
                new(this->data) T(std::move(null).GetValue());
        }

        Nullable& operator=(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
        {
            this->Clear();

            new(this->data) T(value);
            this->is_created = true;

            return *this;
        }

        Nullable& operator=(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
        {
            this->Clear();

            new(this->data) T(std::move(value));
            this->is_created = true;

            return *this;
        }

        template<typename U>
        Nullable& operator=(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        requires std::constructible_from<T, U>
        {
            this->Clear();

            new(this->data) T(std::forward<U>(value));
            this->is_created = true;

            return *this;
        }

        template<typename... Args>
        void CreateInPlace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        requires std::constructible_from<T, Args...>
        {
            this->Clear();

            new(this->data) T(std::forward<Args>(args)...);
            this->is_created = true;
        }

        bool HasValue() const noexcept
        {
            return this->is_created;
        }

        void Clear() noexcept
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
            return std::move(*this).operator*();
        }

        const T&& GetValue() const&& noexcept
        {
            return std::move(*this).operator*();
        }

        explicit operator bool() const noexcept
        {
            return this->is_created;
        }

        T& operator*() & noexcept
        {
            return *reinterpret_cast<T*>(this->data);
        }

        const T& operator*() const& noexcept
        {
            return *reinterpret_cast<const T*>(this->data);
        }

        T&& operator*() && noexcept
        {
            return std::move(*reinterpret_cast<T*>(this->data));
        }

        const T&& operator*() const&& noexcept
        {
            return std::move(*reinterpret_cast<const T*>(this->data));
        }

        T* operator->() noexcept
        {
            return reinterpret_cast<T*>(this->data);
        }

        const T* operator->() const noexcept
        {
            return reinterpret_cast<const T*>(this->data);
        }
    private:
        alignas(alignof(T)) std::byte data[sizeof(T)];
        bool is_created;
    };

    template<typename T>
    requires(!std::is_volatile_v<T>)
    Nullable(T&&) -> Nullable<std::remove_reference_t<T>>;
};