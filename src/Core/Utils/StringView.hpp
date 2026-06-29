#pragma once

#include <cassert>
#include "Core/API.h"
#include "String.hpp"

namespace Core
{
    class StringView
    {
    public:
        using Iterator = const UTF8Char*;
        using CharIterator = Detail::StringCharIterator<const UTF8Char>;
        using RangeAdaptor = Detail::StringCharIteratorRangeAdaptor<const UTF8Char>;

        StringView() noexcept
            : data(nullptr),
              size(0)
        {}

        ~StringView() = default;

        StringView(const StringView& st) = default;
        StringView(StringView&&) = default;
        StringView& operator=(const StringView&) = default;
        StringView& operator=(StringView&&) = default;

        StringView(const UTF8Char* input, DeviceSize input_size)
            : data(input),
              size(input_size)
        {
            auto res = StringEncoder::GetLength(input, input_size);
            if(res.input_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character")
        }

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<DeviceSize N>
        StringView(const UTF8Char (&input)[N])
            : StringView(input, N - 1)
        {}

        StringView(const String& str) noexcept
            : StringView(str.GetData(), str.GetSize())
        {}

        StringView(CharIterator begin, CharIterator end) noexcept
            : data(begin.GetAddress()),
              size(end.GetAddress() - begin.GetAddress())
        {}

        template<Character C>
        StringView(const Detail::StringCharIteratorRangeAdaptor<C> range)
            : StringView(range.GetIterator(), range.GetSentinel())
        {}

        template<DeviceSize N>
        StringView& operator=(const UTF8Char (&input)[N]) noexcept
        {
            StringView view(input);
            *this = view;

            return *this;
        }

        template<Character C>
        StringView& operator=(const Detail::StringCharIteratorRangeAdaptor<C> range) noexcept
        {
            StringView view(range);
            *this = view;

            return *this;
        }

        bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        const UTF8Char* GetData() const noexcept
        {
            return this->data;
        }

        const Char* GetDataAsNativeChar() const noexcept
        {
            return reinterpret_cast<const Char*>(this->data);
        }

        Iterator GetIterator() const noexcept
        {
            return Iterator(this->data);
        }

        Iterator GetSentinel() const noexcept
        {
            return Iterator(this->data + this->size);
        }

        CharIterator GetCharIterator() const noexcept
        {
            return CharIterator(this->data);
        }

        CharIterator GetCharSentinel() const noexcept
        {
            return CharIterator(this->data + this->size);
        }

        Detail::StringCharIteratorRangeAdaptor<const UTF8Char> GetCharRange() const noexcept
        {
            return RangeAdaptor{this->data, this->size};
        }

        Iterator Find(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        Iterator Find(const UTF8Char (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        Iterator Find(const StringView& str) const noexcept
        {
            return Find(str.data, str.size);
        }

        Iterator FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        Iterator FindReverse(const UTF8Char (&input)[N]) const noexcept
        {
            return FindReverse(input, N - 1);
        }

        Iterator FindReverse(const StringView& str) const noexcept
        {
            return FindReverse(str.data, str.size);
        }

        bool StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::StringStartsWith(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        bool StartsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return StartsWith(input, N - 1);
        }

        bool StartsWith(const StringView& str) const noexcept
        {
            return StartsWith(str.data, str.size);
        }

        bool EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::StringEndsWith(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        bool EndsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return EndsWith(input, N - 1);
        }

        bool EndsWith(const StringView& str) const noexcept
        {
            return EndsWith(str.data, str.size);
        }

        bool operator==(const StringView& str) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, str.data, str.size);
        }

        template<DeviceSize N>
        bool operator==(const UTF8Char (&input)[N]) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, input, N - 1);
        }

        bool operator<(const StringView& str) const noexcept
        {
            return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
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

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, StringView>
    auto size(T&& str) noexcept
    {
        return Forward(str).GetSize();
    }
};