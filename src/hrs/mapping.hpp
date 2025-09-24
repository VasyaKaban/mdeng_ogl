#pragma once

#include <ranges>
#include <algorithm>
#include "instantiation.hpp"

namespace hrs
{
    template<std::ranges::forward_range R>
    requires hrs::type_instantiation<std::remove_cvref_t<std::ranges::range_value_t<R>>, std::pair>
    constexpr auto mapping_search(R&& mapping, auto&& from) noexcept
    {
        using From = decltype(std::declval<std::ranges::range_value_t<R>>().first);
        using To = decltype(std::declval<std::ranges::range_value_t<R>>().second);

        From s_from = std::forward<decltype(from)>(from);

        auto it = std::ranges::partition_point(std::forward<R>(mapping),
                                               [&s_from](const std::pair<From, To>& pr)
                                               {
                                                   return pr.first < s_from;
                                               });

        if(it == std::ranges::end(mapping) || it->first != s_from)
            return static_cast<decltype(&it->second)>(nullptr);

        return &it->second;
    }
};