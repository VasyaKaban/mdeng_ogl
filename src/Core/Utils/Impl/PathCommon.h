#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"
#include "../StringView.h"

namespace Core
{
    namespace Detail
    {
        //if we have path like this: /folder1/folder2/file
        //then this path will be iterated in the next order:
        // /
        // folder1/
        // folder2/
        // file
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

            StringView::Iterator GetDataIterator() const noexcept;
            StringView::Iterator GetDataSentinel() const noexcept;
        private:
            StringView data;

            //lazy data
            mutable StringView::Iterator begin;
            mutable StringView::Iterator end;
        };
    };
};