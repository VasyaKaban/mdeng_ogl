#pragma once

#include <cassert>
#include "Core/API.h"
#include "Impl/StringCommon.h"
#include "RangeTraits.hpp"

namespace Core
{
    class String;

    class StringView
    {
    public:
        using Iterator = Detail::StringCharIterator<const UTF8Char>;

        StringView() noexcept;
        ~StringView() = default;
        StringView(const StringView& st) = default;
        StringView(StringView&&) = default;
        StringView& operator=(const StringView&) = default;
        StringView& operator=(StringView&&) = default;

        StringView(const UTF8Char* input, DeviceSize input_size);
        StringView(const String& str) noexcept;
        StringView(Iterator begin, Iterator end) noexcept;

        Bool IsEmpty() const noexcept;
        DeviceSize GetSize() const noexcept;
        const UTF8Char* GetData() const noexcept;
        const Char* GetDataAsNativeChar() const noexcept;

        Iterator GetIterator() const noexcept;
        Iterator GetSentinel() const noexcept;

        Iterator Find(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Iterator Find(const StringView& str) const noexcept;

        Iterator FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Iterator FindReverse(const StringView& str) const noexcept;

        Bool StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Bool StartsWith(const StringView& str) const noexcept;

        Bool EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Bool EndsWith(const StringView& str) const noexcept;

        Bool operator==(const StringView& str) const noexcept;

        Bool operator<(const StringView& str) const noexcept;

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<DeviceSize N>
        StringView(const UTF8Char (&input)[N])
            : StringView(input, N - 1)
        {}

        template<DeviceSize N>
        StringView& operator=(const UTF8Char (&input)[N]) noexcept
        {
            StringView view(input);
            *this = view;

            return *this;
        }

        template<Range R>
        requires Constructible<StringView::Iterator, RangeIterator<R>>
        StringView(R&& rng) noexcept
            : StringView(Forward(rng).GetIterator(), Forward(rng).GetSentinel())
        {}

        template<Range R>
        requires Constructible<StringView::Iterator, RangeIterator<R>>
        StringView& operator=(R&& rng) noexcept
        {
            StringView view(Forward(rng).GetIterator(), Forward(rng).GetSentinel());
            *this = view;

            return *this;
        }

        template<DeviceSize N>
        Iterator Find(const UTF8Char (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        template<DeviceSize N>
        Iterator FindReverse(const UTF8Char (&input)[N]) const noexcept
        {
            return FindReverse(input, N - 1);
        }

        template<DeviceSize N>
        Bool StartsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return StartsWith(input, N - 1);
        }

        template<DeviceSize N>
        Bool EndsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return EndsWith(input, N - 1);
        }

        template<DeviceSize N>
        Bool operator==(const UTF8Char (&input)[N]) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, input, N - 1);
        }
    private:
        const UTF8Char* data;
        DeviceSize size;
    };

    //std compat
    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, StringView>
    auto begin(T&& str) noexcept
    {
        return Forward(str).GetIterator();
    }

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, StringView>
    auto end(T&& str) noexcept
    {
        return Forward(str).GetSentinel();
    }
};