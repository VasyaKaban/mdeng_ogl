
#pragma once
#include <concepts>
#include <utility>

namespace Core
{
    struct Unexpected
    {};

    constexpr inline Unexpected Unexpected;

    template<typename T, typename E>
    class Expected
    {
    public:
        constexpr Expected() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::is_default_constructible_v<T>
            : data{T{}},
              is_error(false)
        {}

        template<typename U = T>
        requires std::constructible_from<T, U>
        constexpr Expected(U&& val) noexcept(std::is_nothrow_constructible_v<T, U>)
            : data(std::forward<U>(val)),
              is_error(false)
        {}

        template<typename U = E>
        requires std::constructible_from<E, U> && (!std::constructible_from<T, U>)
        constexpr Expected(U&& err) noexcept(std::is_nothrow_constructible_v<E, U>)
            : data(std::forward<U>(err)),
              is_error(true)
        {}

        template<typename U = E>
        requires std::assignable_from<E&, U>
        constexpr Expected(U&& err,
                           struct Unexpected _) noexcept(std::is_nothrow_assignable_v<E&, U>)
        {
            is_error = true;
            data.error = std::forward<U>(err);
        }

        constexpr ~Expected()
        {
            if(is_error)
                data.error.~E();
            else
                data.value.~T();
        }

        constexpr Expected(const Expected& ex) noexcept(std::is_nothrow_copy_assignable_v<T> &&
                                                        std::is_nothrow_copy_assignable_v<E>)
        {
            is_error = ex.is_error;
            if(ex.is_error)
                data.error = ex.data.error;
            else
                data.value = ex.data.value;
        }

        constexpr Expected(Expected&& ex) noexcept(std::is_nothrow_move_assignable_v<T> &&
                                                   std::is_nothrow_move_assignable_v<E>)
        {
            is_error = ex.is_error;
            if(ex.is_error)
                data.error = std::move(ex.data.error);
            else
                data.value = std::move(ex.data.value);
        }

        constexpr Expected&
        operator=(const Expected& ex) noexcept(std::is_nothrow_copy_assignable_v<T> &&
                                               std::is_nothrow_copy_assignable_v<E>)
        {
            this->~Expected();
            is_error = ex.is_error;
            if(ex.is_error)
                data.error = ex.data.error;
            else
                data.value = ex.data.value;
            return *this;
        }

        constexpr Expected&
        operator=(Expected&& ex) noexcept(std::is_nothrow_move_assignable_v<T> &&
                                          std::is_nothrow_move_assignable_v<E>)
        {
            this->~Expected();
            is_error = ex.is_error;
            if(ex.is_error)
                data.error = std::move(ex.data.error);
            else
                data.value = std::move(ex.data.value);
            return *this;
        }

        template<typename U = T>
        requires std::assignable_from<T&, U>
        constexpr Expected& operator=(U&& value) noexcept(std::is_nothrow_assignable_v<T&, U>)
        {
            this->~Expected();
            is_error = false;
            data.value = std::forward<U>(value);
            return *this;
        }

        template<typename U = E>
        requires std::assignable_from<E&, U> && (!std::assignable_from<T&, U>)
        constexpr Expected& operator=(U&& error) noexcept(std::is_nothrow_assignable_v<E&, U>)
        {
            this->~Expected();
            is_error = true;
            data.error = std::forward<U>(error);
            return *this;
        }

        constexpr explicit operator bool() const noexcept
        {
            return !is_error;
        }

        constexpr E& Error() & noexcept
        {
            return data.error;
        }

        constexpr const E& Error() const& noexcept
        {
            return data.error;
        }

        constexpr E&& Error() && noexcept
        {
            return std::move(data.error);
        }

        constexpr const E&& Error() const&& noexcept
        {
            return std::move(data.error);
        }

        constexpr T& Value() & noexcept
        {
            return data.value;
        }

        constexpr const T& Value() const& noexcept
        {
            return data.value;
        }

        constexpr T&& Value() && noexcept
        {
            return std::move(data.value);
        }

        constexpr const T&& Value() const&& noexcept
        {
            return std::move(data.value);
        }

        constexpr bool HasValue() const noexcept
        {
            return !is_error;
        }

        constexpr T* operator->() noexcept
        {
            return &Value();
        }

        constexpr const T* operator->() const noexcept
        {
            return &Value();
        }

        constexpr T& operator*() & noexcept
        {
            return Value();
        }
        constexpr const T& operator*() const& noexcept
        {
            return Value();
        }

        constexpr T&& operator*() && noexcept
        {
            return Value();
        }

        constexpr const T&& operator*() const&& noexcept
        {
            return Value();
        }

        template<typename U>
        constexpr T ValueOr(U&& other) const& noexcept(std::is_nothrow_convertible_v<U, T>)
        {
            if(HasValue())
                return **this;
            return std::forward<U>(other);
        }

        template<typename U>
        constexpr T ValueOr(U&& other) const&& noexcept(std::is_nothrow_convertible_v<U, T>)
        {
            if(HasValue())
                return **this;
            return std::forward<U>(other);
        }

        template<typename F>
        constexpr Expected<T, E> AndThen(F&& func) &
        {
            if(HasValue())
                return std::forward<F>(func)(**this);
            else
                return Error();
        }

        template<typename F>
        constexpr Expected<T, E> AndThen(F&& func) const&
        {
            if(HasValue())
                return std::forward<F>(func)(**this);
            else
                return Error();
        }

        template<typename F>
        constexpr Expected<T, E> AndThen(F&& func) &&
        {
            if(HasValue())
                return std::forward<F>(func)(std::move(**this));
            else
                return Error();
        }

        template<typename F>
        constexpr Expected<T, E> AndThen(F&& func) const&&
        {
            if(HasValue())
                return std::forward<F>(func)(std::move(**this));
            else
                return Error();
        }

        template<typename F>
        constexpr Expected<T, E> OrElse(F&& func) &
        {
            if(!HasValue())
                return std::forward<F>(func)(Error());
            else
                return Value();
        }

        template<typename F>
        constexpr Expected<T, E> OrElse(F&& func) const&
        {
            if(!HasValue())
                return std::forward<F>(func)(Error());
            else
                return Value();
        }

        template<typename F>
        constexpr Expected<T, E> OrElse(F&& func) &&
        {
            if(!HasValue())
                return std::forward<F>(func)(std::move(Error()));
            else
                return Value();
        }

        template<typename F>
        constexpr Expected<T, E> OrElse(F&& func) const&&
        {
            if(!HasValue())
                return std::forward<F>(func)(std::move(Error()));
            else
                return Value();
        }
    private:
        union ExpectedData
        {
            T value;
            E error;

            constexpr ExpectedData() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires std::is_default_constructible_v<T>
                : value{}
            {}

            constexpr ~ExpectedData()
            {}

            template<typename U = T>
            requires std::constructible_from<T, U>
            constexpr ExpectedData(U&& val) noexcept(std::is_nothrow_constructible_v<T, U>)
                : value(std::forward<U>(val))
            {}

            template<typename U = E>
            requires std::constructible_from<E, U> && (!std::constructible_from<T, U>)
            constexpr ExpectedData(U&& err) noexcept(std::is_nothrow_constructible_v<E, U>)
                : error(std::forward<U>(err))
            {}

        } data;
        bool is_error;
    };
}