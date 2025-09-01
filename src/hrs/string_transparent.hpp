#pragma once

#include <string>
#include <string_view>
#include "instantiation.hpp"

namespace hrs
{
    template<hrs::type_instantiation<std::basic_string> S>
    struct transparent_string_less_comparator
    {
        using is_transparent = void;
        using char_t = S::value_type;
        using traits_t = S::traits_type;

        bool operator()(const S& str1, const S& str2) const noexcept
        {
            return str1 < str2;
        }

        bool operator()(const S& str1, std::basic_string_view<char_t, traits_t> str2) const noexcept
        {
            return str1 < str2;
        }

        bool operator()(std::basic_string_view<char_t, traits_t> str1, const S& str2) const noexcept
        {
            return str1 < str2;
        }
    };

    template<hrs::type_instantiation<std::basic_string> S>
    struct transparent_string_equal_comparator
    {
        using is_transparent = void;
        using char_t = S::value_type;
        using traits_t = S::traits_type;

        bool operator()(const S& str1, const S& str2) const noexcept
        {
            return str1 == str2;
        }

        bool operator()(const S& str1, std::basic_string_view<char_t, traits_t> str2) const noexcept
        {
            return str1 == str2;
        }

        bool operator()(std::basic_string_view<char_t, traits_t> str1, const S& str2) const noexcept
        {
            return str1 == str2;
        }
    };

    template<hrs::type_instantiation<std::basic_string> S>
    struct transparent_string_hasher
    {
        using is_transparent = void;
        using char_t = S::value_type;
        using traits_t = S::traits_type;
        using string_view_t = std::basic_string_view<char_t, traits_t>;

        std::size_t operator()(const S& str) const noexcept
        {
            return std::hash<string_view_t>{}(str);
        }

        std::size_t operator()(std::basic_string_view<char_t, traits_t> str) const noexcept
        {
            return std::hash<string_view_t>{}(str);
        }
    };
};