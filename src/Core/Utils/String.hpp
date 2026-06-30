#pragma once

#include <cassert>
#include "Memory.h"
#include "Utility.hpp"
#include "StringImpl/StringCommon.h"

namespace Core
{
    class String
    {
    public:
        using Iterator = UTF8Char*;
        using ConstIterator = const UTF8Char*;

        using CharIterator = Detail::StringCharIterator<UTF8Char>;
        using ConstCharIterator = Detail::StringCharIterator<const UTF8Char>;

        using RangeAdaptor = Detail::StringCharIteratorRangeAdaptor<UTF8Char>;
        using ConstRangeAdaptor = Detail::StringCharIteratorRangeAdaptor<const UTF8Char>;

        String(Allocator allocator = GetGlobalAllocator()) noexcept
            : data(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {}

        ~String()
        {
            if(this->data)
                this->allocator.Deallocate(this->data);
        }

        String(const String& str)
        {
            String tmp_str(str.data, str.size, str.allocator);

            *this = Move(tmp_str);
        }

        String(String&& str) noexcept
            : data(Exchange(str.data, nullptr)),
              size(Exchange(str.size, 0)),
              capacity(Exchange(str.capacity, 0)),
              allocator(str.allocator)
        {}

        String& operator=(const String& str)
        {
            *this = String(this->allocator);

            String tmp_str(str.data, str.size, str.allocator);

            *this = Move(tmp_str);

            return *this;
        }

        String& operator=(String&& str) noexcept
        {
            this->~String();

            this->data = Exchange(str.data, nullptr);
            this->size = Exchange(str.size, 0);
            this->capacity = Exchange(str.capacity, 0);
            this->allocator = str.allocator;

            return *this;
        }

        template<Character C>
        String(const C* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator())
            : String(allocator)
        {
            this->Append(input, input_size);
        }

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<Character C, DeviceSize N>
        String(const C (&input)[N], Allocator allocator = GetGlobalAllocator())
            : String(allocator)
        {
            this->Append(input, N - 1);
        }

        String(ConstCharIterator begin, ConstCharIterator end, Allocator allocator = GetGlobalAllocator())
            : String(begin.GetAddress(), end.GetAddress() - begin.GetAddress(), allocator)
        {}

        template<Character C>
        String(const Detail::StringCharIteratorRangeAdaptor<C> range, Allocator allocator = GetGlobalAllocator())
            : String(range.GetIterator().GetAddress(), range.GetSentinel().GetAddress() - range.GetIterator().GetAddress(), allocator)
        {}

        template<Character C, DeviceSize N>
        String& operator=(const C (&input)[N]) noexcept
        {
            *this = String(this->allocator);

            this->Append(input, N - 1);

            return *this;
        }

        template<Character C>
        String& operator=(const Detail::StringCharIteratorRangeAdaptor<C> range) noexcept
        {
            *this = String(this->allocator);

            this->Append(range.GetIterator().GetAddress(), range.GetSentinel().GetAddress() - range.GetIterator().GetAddress());

            return *this;
        }

        Bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        DeviceSize GetCapacity() const noexcept
        {
            return this->capacity;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        UTF8Char* GetData() noexcept
        {
            return this->data;
        }

        const UTF8Char* GetData() const noexcept
        {
            return this->data;
        }

        Char* GetDataAsNativeChar() noexcept
        {
            return reinterpret_cast<Char*>(this->data);
        }

        const Char* GetDataAsNativeChar() const noexcept
        {
            return reinterpret_cast<const Char*>(this->data);
        }

        Void Reserve(DeviceSize reserve)
        {
            if(this->capacity >= reserve)
                return;

            if(this->data != nullptr && this->allocator.Grow(this->data, reserve)) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));

                memcpy(new_memory, this->data, this->size);

                if(this->data != nullptr)
                    this->allocator.Deallocate(this->data);

                this->data = new_memory;
                this->capacity = reserve;
            }
        }

        Void Clear() noexcept
        {
            this->size = 0;
        }

        Bool FlushUnusedReserve() noexcept
        {
            Bool res = false;

            if(!this->data)
                res = true;
            else if(this->size == 0 && this->capacity != 0)
            {
                this->allocator.Deallocate(this->data);

                this->data = nullptr;
                this->capacity = 0;

                res = true;
            }
            else if(this->capacity > this->size)
            {
                res = allocator.Trim(this->data, this->size);
                if(res)
                    this->capacity = this->size;
            }

            return res;
        }

        template<Character C>
        Void Prepend(const C* input, DeviceSize input_size)
        {
            if(input_size == 0)
                return;

            auto length_res = StringEncoder::GetLength(input, input_size);
            if(length_res.input_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character");

            DeviceSize new_size = this->size + length_res.output_size;

            if(this->capacity >= new_size)
            {
                memmove(this->data + length_res.output_size, this->data, this->size); //move old data
                StringEncoder::Convert(input, input_size, this->data); //copy new data
            }
            else if(this->data != nullptr && this->allocator.Grow(this->data, new_size))
            {
                memmove(this->data + length_res.output_size, this->data, this->size); //move old data
                StringEncoder::Convert(input, input_size, this->data); //copy new data

                this->capacity = new_size;
            }
            else
            {
                UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(this->allocator.Allocate(GetMemoryRequirements(new_size)));

                StringEncoder::Convert(input, input_size, new_memory); //copy new data
                memcpy(new_memory + length_res.output_size, this->data, this->size);

                this->capacity = new_size;

                if(this->data != nullptr)
                    this->allocator.Deallocate(this->data);

                this->data = new_memory;
            }

            this->size = new_size;
        }

        template<Character C, DeviceSize N>
        Void Prepend(const C (&input)[N])
        {
            Prepend(input, N - 1);
        }

        Void Prepend(const String& str)
        {
            Prepend(str.data, str.size);
        }

        template<Character C>
        Void Append(const C* input, DeviceSize input_size)
        {
            if(input_size == 0)
                return;

            auto res = StringEncoder::GetLength(input, input_size);
            if(res.input_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character");

            this->Reserve(this->size + res.output_size);

            StringEncoder::Convert(input, input_size, this->data + this->size);

            this->size += res.output_size;
        }

        template<Character C, DeviceSize N>
        Void Append(const C (&input)[N])
        {
            Append(input, N - 1);
        }

        Void Append(const String& str)
        {
            Append(str.data, str.size);
        }

        template<Character C>
        Void Insert(ConstCharIterator before_it, const C* input, DeviceSize input_size)
        {
            if(before_it == GetCharIterator())
            {
                Prepend(input, input_size);
            }
            else if(before_it == GetCharSentinel())
            {
                Append(input, input_size);
            }
            else
            {
                if(input_size == 0)
                    return;

                auto length_res = StringEncoder::GetLength(input, input_size);
                if(length_res.input_offset != input_size)
                    CORE_THROW_EXCEPTION_MOCK("Bad character");

                DeviceSize new_size = this->size + length_res.output_size;

                UTF8Char* second_part_start_ptr = const_cast<UTF8Char*>(before_it.GetAddress());
                UTF8Char* second_part_final_ptr = second_part_start_ptr + length_res.output_size;
                DeviceSize first_part_size = second_part_start_ptr - this->data;
                DeviceSize second_part_size = this->size - first_part_size;

                if(this->capacity >= new_size)
                {
                    memmove(second_part_final_ptr, second_part_start_ptr, second_part_size); //move second part
                    StringEncoder::Convert(input, input_size, second_part_start_ptr); //copy new data
                }
                else if(this->data != nullptr && this->allocator.Grow(this->data, new_size))
                {
                    memmove(second_part_final_ptr, second_part_start_ptr, second_part_size); //move second part
                    StringEncoder::Convert(input, input_size, second_part_start_ptr); //copy new data

                    this->capacity = new_size;
                }
                else
                {
                    UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(UTF8Char), .size = new_size}));

                    memcpy(new_memory, this->data, first_part_size);
                    StringEncoder::Convert(input, input_size, new_memory + first_part_size);
                    memcpy(new_memory + first_part_size + length_res.output_size, second_part_start_ptr, second_part_size);

                    this->capacity = new_size;

                    if(this->data != nullptr)
                        this->allocator.Deallocate(this->data);

                    this->data = new_memory;
                }

                this->size = new_size;
            }
        }

        template<Character C, DeviceSize N>
        Void Insert(ConstCharIterator before_it, const C (&input)[N])
        {
            Insert(before_it, input, N - 1);
        }

        Void Insert(ConstCharIterator before_it, const String& str)
        {
            Insert(before_it, str.data, str.size);
        }

        Void EraseFirst(ConstCharIterator end_it) noexcept
        {
            auto addr = end_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            DeviceSize erase_size = addr - this->data;

            this->size -= erase_size;

            memmove(this->data, addr, this->size);
        }

        Void EraseLast(ConstCharIterator first_it) noexcept
        {
            auto addr = first_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            DeviceSize erase_size = (this->data + this->size) - addr;

            this->size -= erase_size;
        }

        Void Erase(ConstCharIterator begin, ConstCharIterator end) noexcept
        {
            if(end == GetCharSentinel())
            {
                EraseLast(begin);
            }
            else if(begin == GetCharIterator())
            {
                EraseFirst(end);
            }
            else
            {
                DeviceSize erase_size = (end.GetAddress() - begin.GetAddress());

                DeviceSize size_to_move = (this->data + this->size) - end.GetAddress();
                memmove(const_cast<UTF8Char*>(begin.GetAddress()), end.GetAddress(), size_to_move);

                this->size -= erase_size;
            }
        }

        Iterator GetIterator() noexcept
        {
            return Iterator(this->data);
        }

        ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(this->data);
        }

        Iterator GetSentinel() noexcept
        {
            return Iterator(this->data + this->size);
        }

        ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->data + this->size);
        }

        CharIterator GetCharIterator() noexcept
        {
            return CharIterator(this->data);
        }

        ConstCharIterator GetCharIterator() const noexcept
        {
            return ConstCharIterator(this->data);
        }

        CharIterator GetCharSentinel() noexcept
        {
            return CharIterator(this->data + this->size);
        }

        ConstCharIterator GetCharSentinel() const noexcept
        {
            return ConstCharIterator(this->data + this->size);
        }

        Detail::StringCharIteratorRangeAdaptor<UTF8Char> GetCharRange() noexcept
        {
            return RangeAdaptor{this->data, this->size};
        }

        Detail::StringCharIteratorRangeAdaptor<const UTF8Char> GetCharRange() const noexcept
        {
            return ConstRangeAdaptor{this->data, this->size};
        }

        String operator+(const String& str)
        {
            String out(this->allocator);
            out.Reserve(this->capacity + str.size);

            out.Append(*this);
            out.Append(str);

            return out;
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

        String& operator+=(const String& str)
        {
            this->Append(str);

            return *this;
        }

        template<Character C, DeviceSize N>
        String& operator+=(const C (&input)[N])
        {
            this->Append(input, N - 1);

            return *this;
        }

        ConstIterator Find(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        ConstIterator Find(const UTF8Char (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        ConstIterator Find(const String& str) const noexcept
        {
            return Find(str.data, str.size);
        }

        ConstIterator FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        ConstIterator FindReverse(const UTF8Char (&input)[N]) const noexcept
        {
            return FindReverse(input, N - 1);
        }

        ConstIterator FindReverse(const String& str) const noexcept
        {
            return FindReverse(str.data, str.size);
        }

        Bool StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::StringStartsWith(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        Bool StartsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return StartsWith(input, N - 1);
        }

        Bool StartsWith(const String& str) const noexcept
        {
            return StartsWith(str.data, str.size);
        }

        Bool EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
        {
            return ::Core::Detail::StringEndsWith(this->data, this->size, input, input_size);
        }

        template<DeviceSize N>
        Bool EndsWith(const UTF8Char (&input)[N]) const noexcept
        {
            return EndsWith(input, N - 1);
        }

        Bool EndsWith(const String& str) const noexcept
        {
            return EndsWith(str.data, str.size);
        }

        Bool operator==(const String& str) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, str.data, str.size);
        }

        template<DeviceSize N>
        Bool operator==(const UTF8Char (&input)[N]) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, input, N - 1);
        }

        Bool operator<(const String& str) const noexcept
        {
            return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
        }

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(UTF8Char), .size = reserve};
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