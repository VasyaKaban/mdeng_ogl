#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"
#include "../Utility.hpp"
#include "../../API.h"

namespace Core
{
    struct StringEncoderLengthResult
    {
        DeviceSize input_offset;
        DeviceSize output_size;
    };

    struct UTF8CodePoint
    {
        union
        {
            UTF8Char u8_data[4];
            Char data[4];
        } utf8;
        UInt8 length;
    };

    struct CodePoint
    {
        UTF8CodePoint utf8;
        UTF32Char utf32;
    };

    template<typename C>
    concept Character = SameAs<C, Char> || SameAs<C, WideChar> || SameAs<C, UTF8Char> || SameAs<C, UTF16Char> || SameAs<C, UTF32Char>;

    struct SurrogatePair
    {
        UTF16Char first;
        UTF16Char last;
    };

    constexpr inline Bool WIDE_CHAR_IS_UTF16 = (NumericLimits<WideChar>::Max == 0xFF'FF);
    constexpr inline Bool WIDE_CHAR_IS_UTF32 = (NumericLimits<WideChar>::Max > 0xFF'FF);

    constexpr inline SurrogatePair HIGH_SURROGATE_PAIR_RANGE = {.first = 0xD8'00, .last = 0xDB'FF};
    constexpr inline SurrogatePair LOW_SURROGATE_PAIR_RANGE = {.first = 0xDC'00, .last = 0xDF'FF};

    constexpr inline UTF32Char TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT = 0x1'00'00;

    //Let 'char' to be a valid UTF8 char
    //TODO: add enum VariableLengthEncoding for 'char' decode
    struct CORE_API StringEncoder
    {
        static DeviceSize GetCodePointSize(UTF32Char codepoint) noexcept;
        static UTF8CodePoint GetUTF8CodePoint(UTF32Char codepoint) noexcept;
        static UTF32Char GetUTF32Codepoint(const UTF8Char* input) noexcept;
        static const UTF8Char* BacktrackUTF8(const UTF8Char* data) noexcept;

        //char
        static StringEncoderLengthResult GetLength(const Char* input, DeviceSize input_size) noexcept;
        static Void Convert(const Char* input, DeviceSize input_size, UTF8Char* output) noexcept;

        //WideChar
        static StringEncoderLengthResult GetLength(const WideChar* input, DeviceSize input_size) noexcept;
        static Void Convert(const WideChar* input, DeviceSize input_size, UTF8Char* output) noexcept;

        //UTF8Char
        static StringEncoderLengthResult GetLength(const UTF8Char* input, DeviceSize input_size) noexcept;
        static Void Convert(const UTF8Char* input, DeviceSize input_size, UTF8Char* output) noexcept;

        //UTF16Char
        static StringEncoderLengthResult GetLength(const UTF16Char* input, DeviceSize input_size) noexcept;
        static Void Convert(const UTF16Char* input, DeviceSize input_size, UTF8Char* output) noexcept;

        //UTF32Char
        static StringEncoderLengthResult GetLength(const UTF32Char* input, DeviceSize input_size) noexcept;
        static Void Convert(const UTF32Char* input, DeviceSize input_size, UTF8Char* output) noexcept;

        //utf8 to other
        static DeviceSize GetUTF16Size(const UTF8Char* input, DeviceSize input_size) noexcept;
        static DeviceSize GetUTF32Size(const UTF8Char* input, DeviceSize input_size) noexcept;
        static DeviceSize GetWideCharSize(const UTF8Char* input, DeviceSize input_size) noexcept;

        static Void ConvertToUTF16(const UTF8Char* input, DeviceSize input_size, UTF16Char* output) noexcept;
        static Void ConvertToUTF32(const UTF8Char* input, DeviceSize input_size, UTF32Char* output) noexcept;
        static Void ConvertToWideChar(const UTF8Char* input, DeviceSize input_size, WideChar* output) noexcept;
    };

    namespace Detail
    {
        template<typename C>
        requires SameAs<UTF8Char, C> || SameAs<const UTF8Char, C>
        class StringCharIterator
        {
        public:
            explicit StringCharIterator(C* data) noexcept
                : data(data)
            {}

            StringCharIterator() = default;
            ~StringCharIterator() = default;
            StringCharIterator(const StringCharIterator&) = default;
            StringCharIterator(StringCharIterator&&) = default;
            StringCharIterator& operator=(const StringCharIterator&) = default;
            StringCharIterator& operator=(StringCharIterator&&) = default;

            StringCharIterator operator++(int) noexcept
            {
                StringCharIterator out(*this);

                ++(*this);

                return out;
            }

            StringCharIterator& operator++() noexcept
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(this->data);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);
                this->data += utf8.length;

                return *this;
            }

            StringCharIterator operator--(int) noexcept
            {
                StringCharIterator out(*this);

                --(*this);

                return out;
            }

            StringCharIterator& operator--() noexcept
            {
                auto ptr = StringEncoder::BacktrackUTF8(this->data);
                this->data = const_cast<C*>(ptr);

                return *this;
            }

            template<typename OC>
            Bool operator==(const StringCharIterator<OC>& it) const noexcept
            {
                return this->data == it.data;
            }

            CodePoint operator*() const noexcept
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(this->data);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                return CodePoint{.utf8 = utf8, .utf32 = utf32};
            }

            C* GetAddress() const noexcept
            {
                return this->data;
            }

            operator StringCharIterator<const C>() const noexcept
            {
                return StringCharIterator<const C>(this->data);
            }
        private:
            C* data;
        };

        //string utils
        CORE_API const UTF8Char* FindInString(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        CORE_API const UTF8Char* FindInStringReverse(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        CORE_API Bool StringStartsWith(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        CORE_API Bool StringEndsWith(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        CORE_API Bool CompareStringsEquality(const UTF8Char* data1, DeviceSize data_size1, const UTF8Char* data2, DeviceSize data_size2) noexcept;

        CORE_API Bool CompareStringsLexicallyLess(const UTF8Char* data1, DeviceSize data_size1, const UTF8Char* data2, DeviceSize data_size2) noexcept;
    };
};