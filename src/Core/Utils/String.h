#pragma once

#include <cassert>
#include "Memory.h"
#include "Utility.hpp"
#include "Impl/StringCommon.h"

namespace Core
{
    //Char
    //WideChar
    //UTF8Char
    //UTF16Char
    //UTF32Char
    class String
    {
    public:
        using Iterator = Detail::StringCharIterator<UTF8Char>;
        using ConstIterator = Detail::StringCharIterator<const UTF8Char>;

        String(Allocator allocator = GetGlobalAllocator()) noexcept;
        String(DeviceSize reserve, Allocator allocator = GetGlobalAllocator()) noexcept;
        ~String();
        String(const String& str);
        String(String&& str) noexcept;
        String& operator=(const String& str);
        String& operator=(String&& str) noexcept;
        String(ConstIterator begin, ConstIterator end, Allocator allocator = GetGlobalAllocator());

        String(const Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        String(const WideChar* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        String(const UTF8Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        String(const UTF16Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        String(const UTF32Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());

        Bool IsEmpty() const noexcept;
        DeviceSize GetSize() const noexcept;
        DeviceSize GetCapacity() const noexcept;
        Allocator GetAllocator() const noexcept;
        UTF8Char* GetData() noexcept;
        const UTF8Char* GetData() const noexcept;
        Char* GetDataAsNativeChar() noexcept;
        const Char* GetDataAsNativeChar() const noexcept;

        Void Reserve(DeviceSize reserve);
        Void Clear() noexcept;
        Bool FlushUnusedReserve() noexcept;

        Void Prepend(const String& str);
        Void Prepend(const Char* input, DeviceSize input_size);
        Void Prepend(const WideChar* input, DeviceSize input_size);
        Void Prepend(const UTF8Char* input, DeviceSize input_size);
        Void Prepend(const UTF16Char* input, DeviceSize input_size);
        Void Prepend(const UTF32Char* input, DeviceSize input_size);

        Void Append(const String& str);
        Void Append(const Char* input, DeviceSize input_size);
        Void Append(const WideChar* input, DeviceSize input_size);
        Void Append(const UTF8Char* input, DeviceSize input_size);
        Void Append(const UTF16Char* input, DeviceSize input_size);
        Void Append(const UTF32Char* input, DeviceSize input_size);

        Void Insert(ConstIterator before_it, const String& str);
        Void Insert(ConstIterator before_it, const Char* input, DeviceSize input_size);
        Void Insert(ConstIterator before_it, const WideChar* input, DeviceSize input_size);
        Void Insert(ConstIterator before_it, const UTF8Char* input, DeviceSize input_size);
        Void Insert(ConstIterator before_it, const UTF16Char* input, DeviceSize input_size);
        Void Insert(ConstIterator before_it, const UTF32Char* input, DeviceSize input_size);

        Void EraseFirst(ConstIterator end_it) noexcept;
        Void EraseLast(ConstIterator first_it) noexcept;
        Void Erase(ConstIterator begin, ConstIterator end) noexcept;

        Iterator GetIterator() noexcept;
        ConstIterator GetIterator() const noexcept;
        Iterator GetSentinel() noexcept;
        ConstIterator GetSentinel() const noexcept;

        String operator+(const String& str);
        String& operator+=(const String& str);

        ConstIterator Find(const UTF8Char* input, DeviceSize input_size) const noexcept;
        ConstIterator Find(const String& str) const noexcept;

        ConstIterator FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept;
        ConstIterator FindReverse(const String& str) const noexcept;

        Bool StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Bool StartsWith(const String& str) const noexcept;

        Bool EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept;
        Bool EndsWith(const String& str) const noexcept;

        Bool operator==(const String& str) const noexcept;

        Bool operator<(const String& str) const noexcept;

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept;

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<Character C, DeviceSize N>
        String(const C (&input)[N], Allocator allocator = GetGlobalAllocator())
            : String(input, N - 1, allocator)
        {}

        template<Character C, DeviceSize N>
        String& operator=(const C (&input)[N]) noexcept
        {
            *this = String(this->allocator);

            this->Append(input, N - 1);

            return *this;
        }

        template<Character C, DeviceSize N>
        Void Prepend(const C (&input)[N])
        {
            Prepend(input, N - 1);
        }

        template<Character C, DeviceSize N>
        Void Append(const C (&input)[N])
        {
            Append(input, N - 1);
        }

        template<Character C, DeviceSize N>
        Void Insert(ConstIterator before_it, const C (&input)[N])
        {
            Insert(before_it, input, N - 1);
        }

        template<Character C, DeviceSize N>
        String operator+(const C (&input)[N])
        {
            String out(this->allocator);
            out.Reserve(this->capacity + (N - 1));

            out.Append(*this);
            out.Append(input, N - 1);

            return out;
        }

        template<Character C, DeviceSize N>
        String& operator+=(const C (&input)[N])
        {
            this->Append(input, N - 1);

            return *this;
        }

        template<DeviceSize N>
        ConstIterator Find(const UTF8Char (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        template<DeviceSize N>
        ConstIterator FindReverse(const UTF8Char (&input)[N]) const noexcept
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
        UTF8Char* data;
        DeviceSize size;
        DeviceSize capacity;
        Allocator allocator;
    };

    //std compat
    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, String>
    auto begin(T&& str) noexcept
    {
        return Forward(str).GetIterator();
    }

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, String>
    auto end(T&& str) noexcept
    {
        return Forward(str).GetSentinel();
    }

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, String>
    auto size(T&& str) noexcept
    {
        return Forward(str).GetSize();
    }
};