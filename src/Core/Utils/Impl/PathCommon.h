#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"
#include "../StringView.h"

namespace Core
{
    namespace Detail
    {
        class CORE_API PathPartIterator
        {
        public:
            explicit PathPartIterator(StringView data, StringView::Iterator begin) noexcept;

            PathPartIterator() = default;
            ~PathPartIterator() = default;
            PathPartIterator(const PathPartIterator&) = default;
            PathPartIterator(PathPartIterator&&) = default;
            PathPartIterator& operator=(const PathPartIterator&) = default;
            PathPartIterator& operator=(PathPartIterator&&) = default;

            PathPartIterator operator++(int) noexcept;
            PathPartIterator& operator++() noexcept;

            Bool operator==(const PathPartIterator& it) const noexcept;

            StringView operator*() const noexcept;
        private:
            StringView data;
            StringView::Iterator begin;
            StringView::Iterator end;
        };
    };
};