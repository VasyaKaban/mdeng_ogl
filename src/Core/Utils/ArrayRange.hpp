#pragma once

#include "Types.hpp"

template<typename T, DeviceSize N>
constexpr T* begin(T (&arr)[N]) noexcept
{
    return arr;
}

template<typename T, DeviceSize N>
constexpr T* end(T (&arr)[N]) noexcept
{
    return arr + N;
}

template<typename T, DeviceSize N>
constexpr DeviceSize size(T (&arr)[N]) noexcept
{
    return N;
}