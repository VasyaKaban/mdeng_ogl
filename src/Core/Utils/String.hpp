#pragma once

#include <cassert>
#include "Memory.h"
#include "StringImpl/StringCommon.h"

namespace Core
{
    class String
    {
    public:
        using Iterator = char8_t*;
        using ConstIterator = const char8_t*;

        using CharIterator = Detail::StringCharIterator<char8_t>;
        using ConstCharIterator = Detail::StringCharIterator<const char8_t>;

        using RangeAdaptor = Detail::StringCharIteratorRangeAdaptor<char8_t>;
        using ConstRangeAdaptor = Detail::StringCharIteratorRangeAdaptor<const char8_t>;

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
            : data(nullptr),
              size(str.size),
              capacity(str.size),
              allocator(str.allocator)
        {
            if(!str.IsEmpty())
            {
                this->data = reinterpret_cast<char8_t*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(char8_t), .size = str.size}));
                memcpy(this->data, str.data, str.size);
            }
        }

        String(String&& str) noexcept
            : data(std::exchange(str.data, nullptr)),
              size(std::exchange(str.size, 0)),
              capacity(std::exchange(str.capacity, 0)),
              allocator(str.allocator)
        {}

        String& operator=(const String& str)
        {
            Allocator new_allocator = str.allocator;

            if(str.IsEmpty())
            {
                this->ClearAndFlush();
            }
            else
            {
                char8_t* new_memory = reinterpret_cast<char8_t*>(new_allocator.Allocate(MemoryRequirements{.alignment = alignof(char8_t), .size = str.size}));

                this->ClearAndFlush();
                memcpy(new_memory, str.data, str.size);

                if(this->data)
                    this->allocator.Deallocate(this->data);

                this->data = new_memory;
                this->size = str.size;
                this->capacity = this->size;
            }

            this->allocator = new_allocator;

            return *this;
        }

        String& operator=(String&& str) noexcept
        {
            this->ClearAndFlush();

            this->data = std::exchange(str.data, nullptr);
            this->size = std::exchange(str.size, 0);
            this->capacity = std::exchange(str.capacity, 0);
            this->allocator = str.allocator;

            return *this;
        }

        template<Character C>
        String(const C* input, size_t input_size, Allocator allocator = GetGlobalAllocator())
            : String(allocator)
        {
            this->Append(input, input_size);
        }

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<Character C, size_t N>
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
            : String(range.Begin().GetAddress(), range.End().GetAddress() - range.Begin().GetAddress(), allocator)
        {}

        template<Character C, size_t N>
        String& operator=(const C (&input)[N]) noexcept
        {
            this->ClearAndFlush();

            this->Append(input, N - 1);

            return *this;
        }

        template<Character C>
        String& operator=(const Detail::StringCharIteratorRangeAdaptor<C> range) noexcept
        {
            this->ClearAndFlush();

            this->Append(range.Begin().GetAddress(), range.End().GetAddress() - range.Begin().GetAddress());

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

        size_t GetCapacity() const noexcept
        {
            return this->capacity;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        char8_t* GetData() noexcept
        {
            return this->data;
        }

        const char8_t* GetData() const noexcept
        {
            return this->data;
        }

        char* GetDataAsNativeChar() noexcept
        {
            return reinterpret_cast<char*>(this->data);
        }

        const char* GetDataAsNativeChar() const noexcept
        {
            return reinterpret_cast<const char*>(this->data);
        }

        void Reserve(size_t reserve)
        {
            if(this->capacity >= reserve)
                return;

            if(this->data != nullptr && this->allocator.Grow(this->data, reserve)) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                char8_t* new_memory = reinterpret_cast<char8_t*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(char8_t), .size = reserve}));

                memcpy(new_memory, this->data, this->size);

                if(this->data != nullptr)
                    this->allocator.Deallocate(this->data);

                this->data = new_memory;
                this->capacity = reserve;
            }
        }

        void Clear() noexcept
        {
            this->size = 0;
        }

        void ClearAndFlush() noexcept
        {
            if(this->data)
            {
                this->allocator.Deallocate(this->data);

                this->data = nullptr;
                this->size = 0;
                this->capacity = 0;
            }
        }

        bool FlushUnusedReserve() noexcept
        {
            bool res = false;

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
        void Prepend(const C* input, size_t input_size)
        {
            if(input_size == 0)
                return;

            auto length_res = StringEncoder::GetLength(input, input_size);
            if(length_res.input_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character");

            size_t new_size = this->size + length_res.output_size;

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
                char8_t* new_memory = reinterpret_cast<char8_t*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(char8_t), .size = new_size}));

                StringEncoder::Convert(input, input_size, new_memory); //copy new data
                memcpy(new_memory + length_res.output_size, this->data, this->size);

                this->capacity = new_size;

                this->allocator.Deallocate(this->data);
                this->data = new_memory;
            }

            this->size = new_size;
        }

        template<Character C, size_t N>
        void Prepend(const C (&input)[N])
        {
            Prepend(input, N - 1);
        }

        void Prepend(const String& str)
        {
            Prepend(str.data, str.size);
        }

        template<Character C>
        void Append(const C* input, size_t input_size)
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

        template<Character C, size_t N>
        void Append(const C (&input)[N])
        {
            Append(input, N - 1);
        }

        void Append(const String& str)
        {
            Append(str.data, str.size);
        }

        template<Character C>
        void Insert(ConstCharIterator before_it, const C* input, size_t input_size)
        {
            if(before_it == CharBegin())
            {
                Prepend(input, input_size);
            }
            else if(before_it == CharEnd())
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

                size_t new_size = this->size + length_res.output_size;

                char8_t* second_part_start_ptr = const_cast<char8_t*>(before_it.GetAddress());
                char8_t* second_part_final_ptr = second_part_start_ptr + length_res.output_size;
                size_t first_part_size = second_part_start_ptr - this->data;
                size_t second_part_size = this->size - first_part_size;

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
                    char8_t* new_memory = reinterpret_cast<char8_t*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(char8_t), .size = new_size}));

                    memcpy(new_memory, this->data, first_part_size);
                    StringEncoder::Convert(input, input_size, new_memory + first_part_size);
                    memcpy(new_memory + first_part_size + length_res.output_size, second_part_start_ptr, second_part_size);

                    this->capacity = new_size;

                    this->allocator.Deallocate(this->data);
                    this->data = new_memory;
                }

                this->size = new_size;
            }
        }

        template<Character C, size_t N>
        void Insert(ConstCharIterator before_it, const C (&input)[N])
        {
            Insert(before_it, input, N - 1);
        }

        void Insert(ConstCharIterator before_it, const String& str)
        {
            Insert(before_it, str.data, str.size);
        }

        void EraseFirst(ConstCharIterator end_it) noexcept
        {
            auto addr = end_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            size_t erase_size = addr - this->data;

            this->size -= erase_size;

            memmove(this->data, addr, this->size);
        }

        void EraseLast(ConstCharIterator first_it) noexcept
        {
            auto addr = first_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            size_t erase_size = (this->data + this->size) - addr;

            this->size -= erase_size;
        }

        void Erase(ConstCharIterator begin, ConstCharIterator end) noexcept
        {
            if(end == CharEnd())
            {
                EraseLast(begin);
            }
            else if(begin == CharBegin())
            {
                EraseFirst(end);
            }
            else
            {
                size_t erase_size = (end.GetAddress() - begin.GetAddress());

                size_t size_to_move = (this->data + this->size) - end.GetAddress();
                memmove(const_cast<char8_t*>(begin.GetAddress()), end.GetAddress(), size_to_move);

                this->size -= erase_size;
            }
        }

        Iterator Begin() noexcept
        {
            return Iterator(this->data);
        }

        ConstIterator Begin() const noexcept
        {
            return ConstIterator(this->data);
        }

        Iterator End() noexcept
        {
            return Iterator(this->data + this->size);
        }

        ConstIterator End() const noexcept
        {
            return ConstIterator(this->data + this->size);
        }

        CharIterator CharBegin() noexcept
        {
            return CharIterator(this->data);
        }

        ConstCharIterator CharBegin() const noexcept
        {
            return ConstCharIterator(this->data);
        }

        CharIterator CharEnd() noexcept
        {
            return CharIterator(this->data + this->size);
        }

        ConstCharIterator CharEnd() const noexcept
        {
            return ConstCharIterator(this->data + this->size);
        }

        Detail::StringCharIteratorRangeAdaptor<char8_t> GetCharRange() noexcept
        {
            return RangeAdaptor{this->data, this->size};
        }

        Detail::StringCharIteratorRangeAdaptor<const char8_t> GetCharRange() const noexcept
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

        template<Character C, size_t N>
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

        template<Character C, size_t N>
        String& operator+=(const C (&input)[N])
        {
            this->Append(input, N - 1);

            return *this;
        }

        ConstIterator Find(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        }

        template<size_t N>
        ConstIterator Find(const char8_t (&input)[N]) const noexcept
        {
            return Find(input, N - 1);
        }

        ConstIterator Find(const String& str) const noexcept
        {
            return Find(str.data, str.size);
        }

        ConstIterator FindReverse(const char8_t* input, size_t input_size) const noexcept
        {
            return ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        }

        template<size_t N>
        ConstIterator FindReverse(const char8_t (&input)[N]) const noexcept
        {
            return FindReverse(input, N - 1);
        }

        ConstIterator FindReverse(const String& str) const noexcept
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

        bool StartsWith(const String& str) const noexcept
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

        bool EndsWith(const String& str) const noexcept
        {
            return EndsWith(str.data, str.size);
        }

        bool operator==(const String& str) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, str.data, str.size);
        }

        template<size_t N>
        bool operator==(const char8_t (&input)[N]) const noexcept
        {
            return ::Core::Detail::CompareStringsEquality(this->data, this->size, input, N - 1);
        }

        bool operator<(const String& str) const noexcept
        {
            return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
        }
    private:
        char8_t* data;
        size_t size;
        size_t capacity;
        Allocator allocator;
    };

    //std compat
    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, String>
    auto begin(T&& str) noexcept
    {
        return std::forward<T>(str).Begin();
    }

    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, String>
    auto end(T&& str) noexcept
    {
        return std::forward<T>(str).End();
    }

    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, String>
    auto size(T&& str) noexcept
    {
        return std::forward<T>(str).GetSize();
    }
};