#pragma once

#include <concepts>
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
                res = CodePointResult{.input_offset = 1, .codepoint = input[0]};
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
                                              .codepoint = ((static_cast<char32_t>(high_surrogate) - HIGH_SURROGATE_PAIR_RANGE.first) << 10) +
                                                           (static_cast<char32_t>(low_surrogate) - LOW_SURROGATE_PAIR_RANGE.first) + 0x1'00'00};
                    }
                }
            }
            else if(!(input[0] >= LOW_SURROGATE_PAIR_RANGE.first && input[0] <= LOW_SURROGATE_PAIR_RANGE.last))
            {
                res = CodePointResult{.input_offset = 1, .codepoint = input[0]};
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
                output[0] = codepoint & 0b0111'1111;

                res.output_size = 1;
            }
            else if(codepoint <= 0x07'FF)
            {
                output[1] = (codepoint & 0b0011'1111) | 0b1000'0000;
                output[0] = ((codepoint >> 6) & 0b0001'1111) | 0b1100'0000;

                res.output_size = 2;
            }
            else if(codepoint <= 0xFF'FF)
            {
                output[2] = (codepoint & 0b0011'1111) | 0b1000'0000;
                output[1] = ((codepoint >> 6) & 0b0011'1111) | 0b1000'0000;
                output[0] = ((codepoint >> 12) & 0b0000'1111) | 0b1110'0000;

                res.output_size = 3;
            }
            else //if(codepoint < 0x10'FF'FF)
            {
                output[3] = (codepoint & 0b0011'1111) | 0b1000'0000;
                output[2] = ((codepoint >> 6) & 0b0011'1111) | 0b1000'0000;
                output[1] = ((codepoint >> 12) & 0b0011'1111) | 0b1000'0000;
                output[0] = ((codepoint >> 18) & 0b0000'0111) | 0b1111'0000;

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
            if(codepoint < HIGH_SURROGATE_PAIR_RANGE.first)
            {
                output[0] = codepoint;

                return StringEncoderConvertResult{.output_size = 1};
            }
            else
            {
                output[0] = ((codepoint - 0x1'00'00) >> 10) + HIGH_SURROGATE_PAIR_RANGE.first;
                output[1] = ((codepoint - 0x1'00'00) % 0x400) + LOW_SURROGATE_PAIR_RANGE.first;

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
            output[0] = codepoint;

            return StringEncoderConvertResult{.output_size = 1};
        }
    };

    namespace Detail
    {
        template<Character C, StringEncoder<C> E>
        class StringCharIterator
        {
#pragma message("TODO!")
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
              allocator(allocator),
              encoder()
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
              allocator(str.allocator),
              encoder()
        {
            if(!str.IsEmpty())
            {
                this->data = reinterpret_cast<C*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(C), .size = str.size}));
                if(!this->data)
                    CORE_THROW_EXCEPTION_MOCK("BAD ALLOC");

                memcpy(this->data, str.data, str.size);
            }
        }

        String(String&& str) noexcept
            : data(std::exchange(str.data, nullptr)),
              size(std::exchange(str.size, 0)),
              capacity(std::exchange(str.capacity, 0)),
              allocator(str.allocator),
              encoder()
        {}

        String& operator=(const String& str)
        {
            this->ClearAndFlush();

            Allocator new_allocator = str.allocator;

            if(!str.IsEmpty())
            {
                this->data = reinterpret_cast<C*>(new_allocator.Allocate(MemoryRequirements{.alignment = alignof(C), .size = str.size}));
                if(!this->data)
                    CORE_THROW_EXCEPTION_MOCK("BAD ALLOC");

                memcpy(this->data, str.data, str.size);
            }

            this->allocator = new_allocator;

            return *this;
        }

        String& operator=(String&& str) noexcept
        {
            this->ClearAndFlush();

            data = std::exchange(str.data, nullptr);
            size = std::exchange(str.size, 0);
            capacity = std::exchange(str.capacity, 0);
            allocator = str.allocator;

            return *this;
        }

        template<Character OC, StringEncoder<OC> OE>
        String(const String<OC, OE>& str);

        template<Character OC, StringEncoder<OC> OE>
        String(String<OC, OE>&& str) noexcept;

        template<Character OC, StringEncoder<OC> OE>
        String& operator=(const String<OC, OE>& str);

        template<Character OC, StringEncoder<OC> OE>
        String& operator=(String<OC, OE>&& str) noexcept;

        String(C* input, size_t input_size);

        template<Character OC>
        String(OC* input, size_t input_size);

        bool IsEmpty() const noexcept;
        size_t GetSize() const noexcept;
        size_t GetCapacity() const noexcept;
        size_t GetCodePointSize() const noexcept;
        Allocator GetAllocator() const noexcept;
        C* GetData() noexcept;
        const C* GetData() const noexcept;
        typename E::NativeCharType* GetDataAsNativeChar() noexcept;
        const typename E::NativeCharType* GetDataAsNativeChar() const noexcept;

        void Reserve(size_t size);
        void Clear() noexcept;
        void ClearAndFlush() noexcept;

        void Prepend(C* input, size_t input_size);

        template<Character OC>
        void Prepend(OC* input, size_t input_size);

        void Append(C* input, size_t input_size);

        template<Character OC>
        void Append(OC* input, size_t input_size);

        void EraseFirst(ConstCharIterator end_it);
        void EraseLast(ConstCharIterator first_it);

        Iterator Begin() noexcept;
        ConstIterator Begin() const noexcept;
        Iterator End() noexcept;
        ConstIterator End() const noexcept;

        CharIterator CharBegin() noexcept;
        ConstCharIterator CharBegin() const noexcept;
        CharIterator CharEnd() noexcept;
        ConstCharIterator CharEnd() const noexcept;
    private:
        C* data;
        size_t size;
        size_t capacity;

        Allocator allocator;
        [[maybe_unused]] E encoder;
    };

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

    using UTF8String = String<char8_t, UTF8StringEncoder>;
    using UTF16String = String<char16_t, UTF16StringEncoder>;
    using UTF32String = String<char32_t, UTF32StringEncoder>;
};