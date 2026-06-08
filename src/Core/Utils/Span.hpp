#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include "Instantiation.hpp"

namespace Core
{
    template<typename T>
    class Span
    {
    public:
        using Iterator = T*;

        constexpr Span() noexcept
            : data(nullptr),
              size(0)
        {}

        ~Span() = default;

        constexpr Span(T* data, size_t size) noexcept
            : data(data),
              size(size)
        {}

        template<std::contiguous_iterator It>
        requires std::convertible_to<std::remove_reference_t<std::iter_reference_t<It>>, T>
        constexpr Span(It begin, size_t size) noexcept
            : data(std::to_address(begin)),
              size(size)
        {}

        template<std::contiguous_iterator It, std::sentinel_for<It> Se>
        requires std::convertible_to<std::remove_reference_t<std::iter_reference_t<It>>, T>
        constexpr Span(It begin, Se end) noexcept
            : data(std::to_address(begin)),
              size(end - begin)
        {}

        constexpr Span(const Span&) = default;
        constexpr Span(Span&&) = default;
        constexpr Span& operator=(const Span&) = default;
        constexpr Span& operator=(Span&&) = default;

        constexpr bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        constexpr size_t GetSize() const noexcept
        {
            return this->size;
        }

        constexpr T* GetData() const noexcept
        {
            return this->data;
        }

        constexpr Span SubSpan(size_t offset, size_t size) const noexcept
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

        constexpr T& operator[](size_t index) const noexcept
        {
            return this->data[index];
        }

        constexpr Iterator Begin() const noexcept
        {
            return this->data;
        }

        constexpr Iterator End() const noexcept
        {
            return this->data + this->size;
        }
    private:
        T* data;
        size_t size;
    };

    template<typename T>
    Span(T* data, size_t size) -> Span<T>;

    template<std::contiguous_iterator It>
    Span(It begin, size_t size) -> Span<std::remove_reference_t<std::iter_reference_t<It>>>;

    template<std::contiguous_iterator It, std::sentinel_for<It> Se>
    Span(It begin, Se end) -> Span<std::remove_reference_t<std::iter_reference_t<It>>>;

    //std compat
    template<TypeInstantiation<Span> T>
    auto begin(T&& sp) noexcept
    {
        return std::forward<T>(sp).Begin();
    }

    template<TypeInstantiation<Span> T>
    auto end(T&& sp) noexcept
    {
        return std::forward<T>(sp).End();
    }

    template<TypeInstantiation<Span> T>
    auto size(T&& sp) noexcept
    {
        return std::forward<T>(sp).GetSize();
    }
};

constexpr int arr[5] = {1, 2, 3, 4, 5};
constexpr auto a = std::to_address(arr);

using t = std::iter_value_t<decltype(arr)>;

constexpr Core::Span sp(arr, 5);
constexpr Core::Span sp2(arr, arr + 5);