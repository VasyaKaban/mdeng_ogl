#pragma once

#include <string_view>
#include <concepts>
#include "Doc.h"
#include "Core/API.h"

namespace JSON
{
    template<typename T>
    T Parse(const Doc& doc);

    template<typename I>
    I Parse(const Doc& doc)
    requires std::signed_integral<I>
    {
        if(!doc.is_number_integer())
            throw std::runtime_error("Failed to parse integer");

        Doc::number_integer_t i = doc.get<Doc::number_integer_t>();
        if(!(std::numeric_limits<I>::min() <= i && i <= std::numeric_limits<I>::max()))
            throw std::runtime_error("Failed to parse integer: out of range");

        return i;
    }

    template<typename I>
    I Parse(const Doc& doc)
    requires std::unsigned_integral<I>
    {
        if(!doc.is_number_unsigned())
            throw std::runtime_error("Failed to parse unsigned integer");

        Doc::number_unsigned_t i = doc.get<Doc::number_unsigned_t>();
        if(!(std::numeric_limits<I>::min() <= i && i <= std::numeric_limits<I>::max()))
            throw std::runtime_error("Failed to parse unsigned integer: out of range");

        return i;
    }

    template<typename F>
    F Parse(const Doc& doc)
    requires std::floating_point<F>
    {
        if(!doc.is_number())
            throw std::runtime_error("Failed to parse floating");

        Doc::number_float_t i = doc.get<Doc::number_float_t>();
        return i;
    }

    template<>
    CORE_API bool Parse(const Doc& doc);

    template<typename T>
    T ParseKey(const Doc& doc, std::string_view key)
    {
        auto value = doc[key];
        return Parse<T>(value);
    }

    template<typename T>
    std::optional<T> ParseOptionalKey(const Doc& doc, std::string_view key)
    {
        auto value = doc[key];
        if(value.is_null())
            return std::nullopt;

        return Parse<T>(value);
    }
};