#pragma once

#include "Types.hpp"
#include "Traits.hpp"

namespace Core
{
    template<typename T>
    class Span
    {
        template<typename U>
        friend class Span;
    public:
        using Iterator = T*;

        constexpr Span() noexcept
            : data(nullptr),
              size(0)
        {}

        ~Span() = default;

        constexpr Span(T* data, DeviceSize size) noexcept
            : data(data),
              size(size)
        {}

        constexpr Span(T* begin, T* end) noexcept
            : data(begin),
              size(end - begin)
        {}

        constexpr Span(const Span&) = default;
        constexpr Span(Span&&) = default;
        constexpr Span& operator=(const Span&) = default;
        constexpr Span& operator=(Span&&) = default;

        template<typename U>
        requires Constructible<T*, U*>
        constexpr Span(U* begin, DeviceSize size) noexcept
            : data(begin),
              size(size)
        {}

        template<typename U>
        requires Constructible<T*, U*>
        constexpr Span(U* begin, U* end) noexcept
            : data(begin),
              size(end - begin)
        {}

        template<typename U>
        requires Constructible<T*, U*>
        constexpr Span(const Span<U>& sp) noexcept
            : data(sp.data),
              size(sp.size)
        {}

        template<typename U>
        requires Constructible<T*, U*>
        constexpr Span& operator=(const Span<U>& sp) noexcept
        {
            this->data = sp.data;
            this->size = sp.size;
        }

        constexpr bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        constexpr DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        constexpr T* GetData() const noexcept
        {
            return this->data;
        }

        constexpr Span SubSpan(DeviceSize offset, DeviceSize size) const noexcept
        {
            return Span{this->data + offset, size};
        }

        constexpr T& GetFirst() const noexcept
        {
            return this->data[0];
        }

        constexpr T& GetLast() const noexcept
        {
            return this->data[this->size - 1];
        }

        constexpr T& operator[](DeviceSize index) const noexcept
        {
            return this->data[index];
        }

        constexpr Iterator GetIterator() const noexcept
        {
            return this->data;
        }

        constexpr Iterator GetSentinel() const noexcept
        {
            return this->data + this->size;
        }
    private:
        T* data;
        DeviceSize size;
    };

    template<typename T>
    Span(T* data, DeviceSize size) -> Span<T>;

    template<typename T>
    Span(T* begin, T* end) -> Span<T>;

    //std compat
    template<TypeInstantiation<Span> T>
    auto begin(T&& sp) noexcept
    {
        return Forward(sp).GetIterator();
    }

    template<TypeInstantiation<Span> T>
    auto end(T&& sp) noexcept
    {
        return Forward(sp).GetSentinel();
    }

    template<TypeInstantiation<Span> T>
    auto size(T&& sp) noexcept
    {
        return Forward(sp).GetSize();
    }
};