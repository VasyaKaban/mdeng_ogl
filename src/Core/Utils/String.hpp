#pragma once

#include <concepts>
#include <cassert>
#include <ranges>
#include "Instantiation.hpp"
#include "Memory.h"
#include "Nullable.hpp"

namespace Core
{
    struct CodePointResult
    {
        size_t input_offset;
        char32_t codepoint;
    };

    struct StringEncoderConvertResult
    {
        size_t output_size;
    };

    struct StringInputCheckResult
    {
        size_t bad_character_offset; //if equal to input_size then no error occured
        size_t output_size;
    };

    template<typename C>
    concept Character = std::same_as<C, char> || std::same_as<C, wchar_t> || std::same_as<C, char8_t> || std::same_as<C, char16_t> || std::same_as<C, char32_t>;

    template<typename E, typename C>
    concept StringEncoder = std::is_default_constructible_v<E> && Character<C> && requires(const E& encoder, char32_t codepoint, C* output) {
        requires Character<typename E::NativeCharType>;

        { encoder.ConvertCodePoint(codepoint, output) } -> std::same_as<StringEncoderConvertResult>;
    };

    struct SurrogatePair
    {
        char16_t first;
        char16_t last;
    };

    constexpr inline bool WIDE_CHAR_IS_UTF16 = (WCHAR_MAX == 0xFF'FF);
    constexpr inline bool WIDE_CHAR_IS_UTF32 = (WCHAR_MAX > 0xFF'FF);

    constexpr inline SurrogatePair HIGH_SURROGATE_PAIR_RANGE = {.first = 0xD8'00, .last = 0xDB'FF};
    constexpr inline SurrogatePair LOW_SURROGATE_PAIR_RANGE = {.first = 0xDC'00, .last = 0xDF'FF};

    //Let 'char' to be a valid UTF8 char
    //TODO: add enum VariableLengthEncoding for 'char' decode
    struct StringCodePointEncoder
    {
        Nullable<CodePointResult> GetNextCodePoint(const char* input, size_t input_size) const noexcept
        {
            return GetNextCodePoint(reinterpret_cast<const char8_t*>(input), input_size);
        }

        Nullable<CodePointResult> GetNextCodePoint(const wchar_t* input, size_t input_size) const noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return GetNextCodePoint(reinterpret_cast<const char16_t*>(input), input_size);
            else if(WIDE_CHAR_IS_UTF32)
                return GetNextCodePoint(reinterpret_cast<const char32_t*>(input), input_size);
            else
                return GetNextCodePoint(reinterpret_cast<const char8_t*>(input), input_size);
        }

        Nullable<CodePointResult> GetNextCodePoint(const char8_t* input, size_t input_size) const noexcept
        {
            Nullable<CodePointResult> res;
            if((input[0] & 0b1000'0000) == 0) //one char
            {
                res = CodePointResult{.input_offset = 1, .codepoint = static_cast<char32_t>(input[0])};
            }
            else if((input[0] & 0b1110'0000) == 0b1100'0000)
            {
                if(input_size >= 2 && (input[1] & 0b1100'0000) == 0b1000'0000)
                {
                    res = CodePointResult{.input_offset = 2, .codepoint = (static_cast<char32_t>(input[0] & 0b0001'1111) << 6) | static_cast<char32_t>(input[1] & 0b0011'1111)};
                }
            }
            else if((input[0] & 0b1111'0000) == 0b1110'0000)
            {
                if(input_size >= 3 && (input[1] & 0b1100'0000) == 0b1000'0000 && (input[2] & 0b1100'0000) == 0b1000'0000)
                {
                    res = CodePointResult{.input_offset = 3,
                                          .codepoint = (static_cast<char32_t>(input[0] & 0b0000'1111) << 12) | (static_cast<char32_t>(input[1] & 0b0011'1111) << 6) |
                                                       static_cast<char32_t>(input[2] & 0b0011'1111)};
                }
            }
            else if((input[0] & 0b1111'1000) == 0b1111'0000)
            {
                if(input_size >= 4 && (input[1] & 0b1100'0000) == 0b1000'0000 && (input[2] & 0b1100'0000) == 0b1000'0000 && (input[3] & 0b1100'0000) == 0b1000'0000)
                {
                    res = CodePointResult{.input_offset = 4,
                                          .codepoint = (static_cast<char32_t>(input[0] & 0b0000'0111) << 18) | (static_cast<char32_t>(input[1] & 0b0011'1111) << 12) |
                                                       (static_cast<char32_t>(input[2] & 0b0011'1111) << 6) | static_cast<char32_t>(input[3] & 0b0011'1111)};
                }
            }

            return res;
        }

        Nullable<CodePointResult> GetNextCodePoint(const char16_t* input, size_t input_size) const noexcept
        {
            Nullable<CodePointResult> res;
            if(input[0] >= HIGH_SURROGATE_PAIR_RANGE.first && input[0] <= HIGH_SURROGATE_PAIR_RANGE.last) //high surrogate
            {
                if(input_size >= 2)
                {
                    if(input[1] >= LOW_SURROGATE_PAIR_RANGE.first && input[1] <= LOW_SURROGATE_PAIR_RANGE.last)
                    {
                        char16_t high_surrogate = input[0];
                        char16_t low_surrogate = input[1];

                        res = CodePointResult{.input_offset = 2,
                                              .codepoint = ((static_cast<char32_t>(high_surrogate) - static_cast<char32_t>(HIGH_SURROGATE_PAIR_RANGE.first)) << 10) +
                                                           (static_cast<char32_t>(low_surrogate) - static_cast<char32_t>(LOW_SURROGATE_PAIR_RANGE.first)) + 0x1'00'00};
                    }
                }
            }
            else if(!(input[0] >= LOW_SURROGATE_PAIR_RANGE.first && input[0] <= LOW_SURROGATE_PAIR_RANGE.last))
            {
                res = CodePointResult{.input_offset = 1, .codepoint = static_cast<char32_t>(input[0])};
            }

            return res;
        }

        Nullable<CodePointResult> GetNextCodePoint(const char32_t* input, size_t input_size) const noexcept
        {
            Nullable<CodePointResult> res;
            if(input[0] <= 0x10'FF'FF)
                res = CodePointResult{.input_offset = 1, .codepoint = input[0]};

            return res;
        }

        template<typename E, Character C>
        requires StringEncoder<C, E>
        StringInputCheckResult CheckInputIsValid(const C* input, size_t input_size)
        {
            E encoder;
            StringInputCheckResult res = {.bad_character_offset = 0, .output_size = 0};
            while(res.bad_character_offset != input_size)
            {
                auto opt = GetNextCodePoint(input + res.bad_character_offset, input_size - res.bad_character_offset);
                if(!opt)
                    return res;

                res.bad_character_offset += opt->input_offset;

                res.output_size += encoder.ConvertCodePoint(opt->codepoint, nullptr)->output_size;
            }

            return res;
        }

        template<typename E, Character C>
        requires StringEncoder<C, E>
        void ConvertInput(const C* input, size_t input_size, typename E::CharType* output)
        {
            E encoder;
            StringInputCheckResult res = {.bad_character_offset = 0, .output_size = 0};
            while(res.bad_character_offset != input_size)
            {
                auto opt = GetNextCodePoint(input + res.bad_character_offset, input_size - res.bad_character_offset);
                res.bad_character_offset += opt->input_offset;

                res.output_size += encoder.ConvertCodePoint(opt->codepoint, output + res.output_size)->output_size;
            }
        }
    };

    struct UTF8StringEncoder
    {
        using CharType = char8_t;
        using NativeCharType = char;

        StringEncoderConvertResult ConvertCodePoint(char32_t codepoint, CharType* output) const noexcept
        {
            StringEncoderConvertResult res = {.output_size = 0};
            if(codepoint <= 0x00'7F)
            {
                if(output)
                    output[0] = codepoint & 0b0111'1111;

                res.output_size = 1;
            }
            else if(codepoint <= 0x07'FF)
            {
                if(output)
                {
                    output[1] = (codepoint & 0b0011'1111) | 0b1000'0000;
                    output[0] = ((codepoint >> 6) & 0b0001'1111) | 0b1100'0000;
                }

                res.output_size = 2;
            }
            else if(codepoint <= 0xFF'FF)
            {
                if(output)
                {
                    output[2] = (codepoint & 0b0011'1111) | 0b1000'0000;
                    output[1] = ((codepoint >> 6) & 0b0011'1111) | 0b1000'0000;
                    output[0] = ((codepoint >> 12) & 0b0000'1111) | 0b1110'0000;
                }

                res.output_size = 3;
            }
            else //if(codepoint < 0x10'FF'FF)
            {
                if(output)
                {
                    output[3] = (codepoint & 0b0011'1111) | 0b1000'0000;
                    output[2] = ((codepoint >> 6) & 0b0011'1111) | 0b1000'0000;
                    output[1] = ((codepoint >> 12) & 0b0011'1111) | 0b1000'0000;
                    output[0] = ((codepoint >> 18) & 0b0000'0111) | 0b1111'0000;
                }

                res.output_size = 4;
            }

            return res;
        }
    };

    struct UTF16StringEncoder
    {
        using CharType = char16_t;
        using NativeCharType = std::conditional_t<WIDE_CHAR_IS_UTF16, wchar_t, char16_t>;

        StringEncoderConvertResult ConvertCodePoint(char32_t codepoint, CharType* output) const noexcept
        {
            if(codepoint < static_cast<char32_t>(HIGH_SURROGATE_PAIR_RANGE.first))
            {
                if(output)
                    output[0] = static_cast<char16_t>(codepoint);

                return StringEncoderConvertResult{.output_size = 1};
            }
            else
            {
                if(output)
                {
                    output[0] = ((codepoint - 0x1'00'00) >> 10) + HIGH_SURROGATE_PAIR_RANGE.first;
                    output[1] = ((codepoint - 0x1'00'00) % 0x400) + LOW_SURROGATE_PAIR_RANGE.first;
                }

                return StringEncoderConvertResult{.output_size = 2};
            }
        }
    };

    struct UTF32StringEncoder
    {
        using CharType = char32_t;
        using NativeCharType = std::conditional_t<WIDE_CHAR_IS_UTF32, wchar_t, char32_t>;

        StringEncoderConvertResult ConvertCodePoint(char32_t codepoint, CharType* output) const noexcept
        {
            if(output)
                output[0] = codepoint;

            return StringEncoderConvertResult{.output_size = 1};
        }
    };

    namespace Detail
    {
        template<Character C, StringEncoder<C> E>
        class StringCharIterator
        {
            template<typename OC, typename OE>
            friend class String;

            StringCharIterator(C* data, size_t size) noexcept
                : data(data),
                  size(size)
            {}

            C* GetAddress() const noexcept
            {
                return this->data;
            }
        public:
            StringCharIterator() = default;
            ~StringCharIterator() = default;
            StringCharIterator(const StringCharIterator&) = default;
            StringCharIterator(StringCharIterator&&) = default;
            StringCharIterator& operator=(const StringCharIterator&) = default;
            StringCharIterator& operator=(StringCharIterator&&) = default;

            StringCharIterator operator++(int) noexcept
            {
                StringCharIterator out(*this);

                StringCodePointEncoder codepoint_encoder;
                this->size += codepoint_encoder.GetNextCodePoint(this->data, this->size)->input_offset;

                return out;
            }

            StringCharIterator& operator++() noexcept
            {
                StringCodePointEncoder codepoint_encoder;
                this->size += codepoint_encoder.GetNextCodePoint(this->data, this->size)->input_offset;

                return *this;
            }

            bool operator==(const StringCharIterator& it) const noexcept
            {
                return this->data == it.data && this->size == it.size;
            }

            C& operator*() const noexcept
            {
                return *this->data;
            }

            C* operator->() const noexcept
            {
                return this->data;
            }

            char32_t GetCodePoint() const noexcept
            {
                StringCodePointEncoder codepoint_encoder;
                return codepoint_encoder.GetNextCodePoint(this->data, this->size)->codepoint;
            }
        private:
            C* data;
            size_t size;
        };

        enum class StringReserveStatus
        {
            EnoughCapacity,
            GrowMemory,
            ReallocateMemory
        };

        template<Character C>
        struct StringReserveResult
        {
            StringReserveStatus status;
            C* new_memory;
        };
    };

    template<Character C, StringEncoder<C> E>
    class String
    {
    public:
        using Iterator = C*;
        using ConstIterator = const C*;

        using CharIterator = Detail::StringCharIterator<C, E>;
        using ConstCharIterator = Detail::StringCharIterator<const C, E>;

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
                this->data = reinterpret_cast<C*>(this->allocator.Allocate(GetMemoryRequirements(str.size)));
                if(!this->data)
                    CORE_THROW_EXCEPTION_MOCK("BAD ALLOC");

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
            this->ClearAndFlush();

            Allocator new_allocator = str.allocator;

            if(!str.IsEmpty())
            {
                this->data = reinterpret_cast<C*>(new_allocator.Allocate(GetMemoryRequirements(str.size)));
                if(!this->data)
                    CORE_THROW_EXCEPTION_MOCK("BAD ALLOC");

                memcpy(this->data, str.data, str.size);
            }

            this->size = str.size;
            this->capacity = str.size;
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

        template<Character OC, StringEncoder<OC> OE>
        String(const String<OC, OE>& str)
            : String(str.data, str.size, str.allocator)
        {}

        template<Character OC, StringEncoder<OC> OE>
        String(String<OC, OE>&& str) noexcept
            : String(str.data, str.size, str.allocator)
        {}

        template<Character OC, StringEncoder<OC> OE>
        String& operator=(const String<OC, OE>& str)
        {
            this->Clear();

            this->Append(str.data, str.size);

            return *this;
        }

        template<Character OC, StringEncoder<OC> OE>
        String& operator=(String<OC, OE>&& str) noexcept
        {
            this->Clear();

            this->Append(str.data, str.size);

            return *this;
        }

        String(const C* input, size_t input_size, Allocator allocator = GetGlobalAllocator())
            : data(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {
            this->Append(input, input_size);
        }

        template<Character OC>
        String(const OC* input, size_t input_size, Allocator allocator = GetGlobalAllocator())
            : data(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {
            this->Append(input, input_size);
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

        C* GetData() noexcept
        {
            return this->data;
        }

        const C* GetData() const noexcept
        {
            return this->data;
        }

        typename E::NativeCharType* GetDataAsNativeChar() noexcept
        {
            return reinterpret_cast<typename E::NativeCharType*>(this->data);
        }

        const typename E::NativeCharType* GetDataAsNativeChar() const noexcept
        {
            return reinterpret_cast<const typename E::NativeCharType*>(this->data);
        }

        void Reserve(size_t reserve)
        {
            if(this->capacity >= reserve)
                return;

            if(this->data != nullptr && this->allocator.Grow(this->data, reserve * sizeof(C))) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                C* new_memory = reinterpret_cast<C*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));
                if(!new_memory)
                    CORE_THROW_EXCEPTION_MOCK("Bad alloc");

                memcpy(new_memory, this->data, this->size * sizeof(C));

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

        void Prepend(const C* input, size_t input_size)
        {
            if(input_size == 0)
                return;

            size_t new_size = this->size + input_size;
            if(this->capacity >= this->size + input_size)
            {
                memmove(this->data + input_size, this->data, GetSizeInBytes(this->size));
                memcpy(this->data, input, GetSizeInBytes(input_size));
            }
            else if(this->data != nullptr && this->allocator.Grow(this->data, GetSizeInBytes(new_size)))
            {
                memmove(this->data + input_size, this->data, GetSizeInBytes(this->size));
                memcpy(this->data, input, GetSizeInBytes(input_size));

                this->capacity = new_size;
            }
            else
            {
                C* new_memory = this->allocator.Allocate(GetMemoryRequirements(new_size));
                if(!new_memory)
                    CORE_THROW_EXCEPTION_MOCK("Bad alloc");

                memcpy(new_memory, input, GetSizeInBytes(input_size));
                memcpy(new_memory + input_size, this->data, GetSizeInBytes(this->size));

                this->capacity = new_size;

                this->allocator.Deallocate(this->data);
                this->data = new_memory;
            }

            this->size = new_size;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<Character OC>
        void Prepend(const OC* input, size_t input_size);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Append(const C* input, size_t input_size)
        {
            if(input_size == 0)
                return;

            StringCodePointEncoder codepoint_encoder;
            auto check_res = codepoint_encoder.CheckInputIsValid<E>(input, input_size);
            if(check_res.bad_character_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character");

            this->Reserve(this->size + input_size);

            memcpy(this->data + this->size, input, input_size * sizeof(C));

            this->size += input_size;
        }

        template<Character OC>
        void Append(const OC* input, size_t input_size)
        {
            if(input_size == 0)
                return;

            StringCodePointEncoder codepoint_encoder;
            auto check_res = codepoint_encoder.CheckInputIsValid<E>(input, input_size);
            if(check_res.bad_character_offset != input_size)
                CORE_THROW_EXCEPTION_MOCK("Bad character");

            this->Reserve(this->size + check_res.output_size);

            codepoint_encoder.ConvertInput<E>(input, input_size, this->data + this->size);

            this->size += check_res.output_size;
        }

        void EraseFirst(ConstCharIterator end_it)
        {
            auto addr = end_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            size_t erase_size = addr - this->data;

            this->size -= erase_size;

            memmove(this->data, addr, this->size);
        }

        void EraseLast(ConstCharIterator first_it)
        {
            auto addr = first_it.GetAddress();
            assert(addr >= this->data && addr <= (this->data + this->size));

            size_t erase_size = (this->data + this->size) - addr;

            this->size -= erase_size;
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
            return CharIterator(this->data, this->size);
        }

        ConstCharIterator CharBegin() const noexcept
        {
            return ConstCharIterator(this->data, this->size);
        }

        CharIterator CharEnd() noexcept
        {
            return CharIterator(this->data + this->size, 0);
        }

        ConstCharIterator CharEnd() const noexcept
        {
            return ConstCharIterator(this->data + this->size, 0);
        }
    private:
        static MemoryRequirements GetMemoryRequirements(size_t size) noexcept
        {
            return MemoryRequirements{.alignment = alignof(C), .size = size * sizeof(C)};
        }

        static size_t GetSizeInBytes(size_t size) noexcept
        {
            return size * sizeof(C);
        }

#error HERE!!!!!
        Detail::ReserveResult RawReserve(size_t reserve)
        {
            if(this->capacity >= reserve)
                return Detail::ReserveResult{.status = ReserveStatus::EnoughCapacity};

            if(this->data != nullptr && this->allocator.Grow(this->data, reserve * sizeof(C))) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                C* new_memory = reinterpret_cast<C*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));
                if(!new_memory)
                    CORE_THROW_EXCEPTION_MOCK("Bad alloc");

                memcpy(new_memory, this->data, this->size * sizeof(C));

                if(this->data != nullptr)
                    this->allocator.Deallocate(this->data);

                this->data = new_memory;
                this->capacity = reserve;
            }
        }
    private:
        C* data;
        size_t size;
        size_t capacity;
        Allocator allocator;
    };

    using UTF8String = String<char8_t, UTF8StringEncoder>;
    using UTF16String = String<char16_t, UTF16StringEncoder>;
    using UTF32String = String<char32_t, UTF32StringEncoder>;

    //std compat
    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, String>
    auto begin(T&& str) noexcept
    {
        return std::forward<T>(str).Begin();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, String>
    auto end(T&& str) noexcept
    {
        return std::forward<T>(str).End();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, String>
    auto size(T&& str) noexcept
    {
        return std::forward<T>(str).GetSize();
    }
};

//std compat
namespace std
{
    template<Core::Character C, Core::StringEncoder<C> E>
    struct iterator_traits<Core::Detail::StringCharIterator<C, E>>
    {
        using difference_type = std::ptrdiff_t;
        using value_type = C;
        using pointer = C*;
        using reference = C&;
        using iterator_category = std::forward_iterator_tag;
    };
};
