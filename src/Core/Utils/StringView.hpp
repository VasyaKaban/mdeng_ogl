#pragma once

#include <cassert>
#include "Core/API.h"
#include "StringImpl/StringCommon.h"

namespace Core
{
    class StringView
    {
    public:
        using Iterator = const char8_t*;
        using CharIterator = Detail::StringCharIterator<const char8_t>;
        using RangeAdaptor = Detail::StringCharIteratorRangeAdaptor<const char8_t>;

        StringView() noexcept
            : data(nullptr),
              size(0)
        {}

        ~StringView() = default;

        StringView(const StringView& st) = default;
        StringView(StringView&&) = default;
        StringView& operator=(const StringView&) = default;
        StringView& operator=(StringView&&) = default;

        StringView(const char8_t* input, size_t input_size)
            : data(input),
              size(input_size)
        {
            auto res = StringEncoder::GetLength(input, input_size);
            if(res.input_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character")
        }

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<size_t N>
        StringView(const char8_t (&input)[N])
            : StringView(input, N - 1)
        {}

        StringView(CharIterator begin, CharIterator end) noexcept
            : data(begin.GetAddress()),
              size(end.GetAddress() - begin.GetAddress())
        {}

        template<Character C>
        StringView(const Detail::StringCharIteratorRangeAdaptor<C> range)
            : StringView(range.Begin(), range.End())
        {}

        template<size_t N>
        StringView& operator=(const char8_t (&input)[N]) noexcept
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

        size_t GetSize() const noexcept
        {
            return this->size;
        }

        const char8_t* GetData() const noexcept
        {
            return this->data;
        }

        const char* GetDataAsNativeChar() const noexcept
        {
            return reinterpret_cast<const char*>(this->data);
        }

        Iterator Begin() const noexcept
        {
            return Iterator(this->data);
        }

        Iterator End() const noexcept
        {
            return Iterator(this->data + this->size);
        }

        CharIterator CharBegin() const noexcept
        {
            return CharIterator(this->data);
        }

        CharIterator CharEnd() const noexcept
        {
            return CharIterator(this->data + this->size);
        }

        Detail::StringCharIteratorRangeAdaptor<const char8_t> GetCharRange() const noexcept
        {
            return RangeAdaptor{this->data, this->size};
        }

        Iterator Find(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        }

        template<size_t N>
        Iterator Find(const char8_t (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        Iterator Find(const StringView& str) const noexcept
        {
            return Find(str.data, str.size);
        }

        Iterator FindReverse(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        }

        template<size_t N>
        Iterator FindReverse(const char8_t (&input)[N]) const noexcept
        {
            return FindReverse(input, N - 1);
        }

        Iterator FindReverse(const StringView& str) const noexcept
        {
            return FindReverse(str.data, str.size);
        }

        bool StartsWith(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::StringStartsWith(this->data, this->size, input, input_size);
        }

        template<size_t N>
        bool StartsWith(const char8_t (&input)[N]) const noexcept
        {
            return StartsWith(input, N - 1);
        }

        bool StartsWith(const StringView& str) const noexcept
        {
            return StartsWith(str.data, str.size);
        }

        bool EndsWith(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::StringEndsWith(this->data, this->size, input, input_size);
        }

        template<size_t N>
        bool EndsWith(const char8_t (&input)[N]) const noexcept
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

        template<size_t N>
        bool operator==(const char8_t (&input)[N]) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, input, N - 1);
        }

        bool operator<(const StringView& str) const noexcept
        {
            return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
        }
    private:
        const char8_t* data;
        size_t size;
    };

    //std compat
    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, StringView>
    auto begin(T&& str) noexcept
    {
        return std::forward<T>(str).Begin();
    }

    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, StringView>
    auto end(T&& str) noexcept
    {
        return std::forward<T>(str).End();
    }

    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, StringView>
    auto size(T&& str) noexcept
    {
        return std::forward<T>(str).GetSize();
    }
};