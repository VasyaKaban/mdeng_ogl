#pragma once

#include <string>
#include "Instantiation.hpp"

namespace Core
{
    template<Core::TypeInstantiation<std::basic_string> S>
    struct TransparentStringHasher
    {
        using is_transparent = void;

        using CharType = S::value_type;
        using TraitsType = S::traits_type;
        using StringViewType = std::basic_string_view<CharType, TraitsType>;

        std::size_t operator()(const S& str) const noexcept
        {
            return std::hash<StringViewType>{}(str);
        }

        std::size_t operator()(const StringViewType& str) const noexcept
        {
            return std::hash<StringViewType>{}(str);
        }

        std::size_t operator()(const CharType* null_str) const noexcept
        {
            return std::hash<StringViewType>{}(null_str);
        }
    };
};