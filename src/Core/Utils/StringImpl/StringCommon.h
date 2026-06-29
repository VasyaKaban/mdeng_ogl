#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"

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
    struct StringEncoder
    {
        static DeviceSize GetCodePointSize(UTF32Char codepoint) noexcept
        {
            if(codepoint <= 0x00'7F)
                return 1;
            else if(codepoint <= 0x07'FF)
                return 2;
            else if(codepoint <= 0xFF'FF)
                return 3;
            else //if(codepoint < 0x10'FF'FF)
                return 4;
        }

        static UTF8CodePoint GetUTF8CodePoint(UTF32Char codepoint) noexcept
        {
            UTF8CodePoint res;
            if(codepoint <= 0x00'7F)
            {
                res.length = 1;
                res.utf8 = {static_cast<UTF8Char>(codepoint & 0b0111'1111)};
            }
            else if(codepoint <= 0x07'FF)
            {
                res.length = 2;
                res.utf8 = {static_cast<UTF8Char>(((codepoint >> 6) & 0b0001'1111) | 0b1100'0000), static_cast<UTF8Char>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }
            else if(codepoint <= 0xFF'FF)
            {
                res.length = 3;
                res.utf8 = {static_cast<UTF8Char>(((codepoint >> 12) & 0b0000'1111) | 0b1110'0000),
                            static_cast<UTF8Char>(((codepoint >> 6) & 0b0011'1111) | 0b100'0000),
                            static_cast<UTF8Char>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }
            else //if(codepoint < 0x10'FF'FF)
            {
                res.length = 4;
                res.utf8 = {static_cast<UTF8Char>(((codepoint >> 18) & 0b0000'0111) | 0b1111'0000),
                            static_cast<UTF8Char>(((codepoint >> 12) & 0b0011'1111) | 0b1000'0000),
                            static_cast<UTF8Char>(((codepoint >> 6) & 0b0011'1111) | 0b1000'0000),
                            static_cast<UTF8Char>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }

            return res;
        }

        static UTF32Char GetUTF32Codepoint(const UTF8Char* input) noexcept
        {
            if((input[0] & 0b1000'0000) == 0) //one char
                return static_cast<UTF32Char>(input[0]);
            else if((input[0] & 0b1110'0000) == 0b1100'0000)
                return (static_cast<UTF32Char>(input[0] & 0b0001'1111) << 6) | static_cast<UTF32Char>(input[1] & 0b0011'1111);
            else if((input[0] & 0b1111'0000) == 0b1110'0000)
                return (static_cast<UTF32Char>(input[0] & 0b0000'1111) << 12) | (static_cast<UTF32Char>(input[1] & 0b0011'1111) << 6) | static_cast<UTF32Char>(input[2] & 0b0011'1111);
            else //if((input[0] & 0b1111'1000) == 0b1111'0000)
                return (static_cast<UTF32Char>(input[0] & 0b0000'0111) << 18) | (static_cast<UTF32Char>(input[1] & 0b0011'1111) << 12) | (static_cast<UTF32Char>(input[2] & 0b0011'1111) << 6) |
                       static_cast<UTF32Char>(input[3] & 0b0011'1111);
        }

        //char
        static StringEncoderLengthResult GetLength(const Char* input, DeviceSize input_size) noexcept
        {
            return GetLength(reinterpret_cast<const UTF8Char*>(input), input_size);
        }

        static void Convert(const Char* input, DeviceSize input_size, UTF8Char* output) noexcept
        {
            return Convert(reinterpret_cast<const UTF8Char*>(input), input_size, output);
        }

        //WideChar
        static StringEncoderLengthResult GetLength(const WideChar* input, DeviceSize input_size) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return GetLength(reinterpret_cast<const UTF16Char*>(input), input_size);
            else if(WIDE_CHAR_IS_UTF32)
                return GetLength(reinterpret_cast<const UTF32Char*>(input), input_size);
            else
                return GetLength(reinterpret_cast<const UTF8Char*>(input), input_size);
        }

        static void Convert(const WideChar* input, DeviceSize input_size, UTF8Char* output) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                Convert(reinterpret_cast<const UTF16Char*>(input), input_size, output);
            else if(WIDE_CHAR_IS_UTF32)
                Convert(reinterpret_cast<const UTF32Char*>(input), input_size, output);
            else
                Convert(reinterpret_cast<const UTF8Char*>(input), input_size, output);
        }

        //UTF8Char
        static StringEncoderLengthResult GetLength(const UTF8Char* input, DeviceSize input_size) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
            while(res.input_offset != input_size)
            {
                if((input[res.input_offset] & 0b1000'0000) == 0) //one char
                {
                    res.input_offset++;
                    res.output_size++;
                }
                else if((input[res.input_offset] & 0b1110'0000) == 0b1100'0000)
                {
                    if(input_size < 2 || (input[res.input_offset + 1] & 0b1100'0000) != 0b1000'0000)
                        break;

                    res.input_offset += 2;
                    res.output_size += 2;
                }
                else if((input[res.input_offset] & 0b1111'0000) == 0b1110'0000)
                {
                    if(input_size < 3 || (input[res.input_offset + 1] & 0b1100'0000) != 0b1000'0000 || (input[res.input_offset + 2] & 0b1100'0000) != 0b1000'0000)
                        break;

                    res.input_offset += 3;
                    res.output_size += 3;
                }
                else if((input[res.input_offset] & 0b1111'1000) == 0b1111'0000)
                {
                    if(input_size < 4 || (input[1] & 0b1100'0000) != 0b1000'0000 || (input[2] & 0b1100'0000) != 0b1000'0000 || (input[3] & 0b1100'0000) != 0b1000'0000)
                        break;

                    res.input_offset += 4;
                    res.output_size += 4;
                }
                else
                    break;
            }

            return res;
        }

        static void Convert(const UTF8Char* input, DeviceSize input_size, UTF8Char* output) noexcept
        {
            memcpy(output, input, input_size);
        }

        //UTF16Char
        static StringEncoderLengthResult GetLength(const UTF16Char* input, DeviceSize input_size) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};

            UTF32Char codepoint;
            while(res.input_offset != input_size)
            {
                if(input[res.input_offset] >= LOW_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= LOW_SURROGATE_PAIR_RANGE.last) //low surogate
                    break;
                else if(input[res.input_offset] >= HIGH_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= HIGH_SURROGATE_PAIR_RANGE.last) //high surrogate
                {
                    if(input_size < 2 || !(input[res.input_offset + 1] >= LOW_SURROGATE_PAIR_RANGE.first && input[res.input_offset + 1] <= LOW_SURROGATE_PAIR_RANGE.last))
                        break;

                    UTF16Char high_surrogate = input[res.input_offset];
                    UTF16Char low_surrogate = input[res.input_offset + 1];

                    codepoint = ((static_cast<UTF32Char>(high_surrogate) - static_cast<UTF32Char>(HIGH_SURROGATE_PAIR_RANGE.first)) << 10) +
                                (static_cast<UTF32Char>(low_surrogate) - static_cast<UTF32Char>(LOW_SURROGATE_PAIR_RANGE.first)) + 0x1'00'00;

                    res.input_offset += 2;
                }
                else
                {
                    codepoint = static_cast<UTF32Char>(input[res.input_offset]);

                    res.input_offset++;
                }

                res.output_size += GetCodePointSize(codepoint);
            }

            return res;
        }

        static void Convert(const UTF16Char* input, DeviceSize input_size, UTF8Char* output) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};

            UTF32Char codepoint;
            while(res.input_offset != input_size)
            {
                if(input[res.input_offset] >= HIGH_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= HIGH_SURROGATE_PAIR_RANGE.last) //high surrogate
                {
                    UTF16Char high_surrogate = input[res.input_offset];
                    UTF16Char low_surrogate = input[res.input_offset + 1];

                    codepoint = ((static_cast<UTF32Char>(high_surrogate) - static_cast<UTF32Char>(HIGH_SURROGATE_PAIR_RANGE.first)) << 10) +
                                (static_cast<UTF32Char>(low_surrogate) - static_cast<UTF32Char>(LOW_SURROGATE_PAIR_RANGE.first)) + 0x1'00'00;

                    res.input_offset += 2;
                }
                else
                {
                    codepoint = static_cast<UTF32Char>(input[res.input_offset]);

                    res.input_offset++;
                }

                auto utf8_codepoint = GetUTF8CodePoint(codepoint);
                memcpy(output + res.output_size, utf8_codepoint.utf8.u8_data, utf8_codepoint.length);

                res.output_size += utf8_codepoint.length;
            }
        }

        //UTF32Char
        static StringEncoderLengthResult GetLength(const UTF32Char* input, DeviceSize input_size) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
            while(res.input_offset != input_size)
            {
                if(input[res.output_size] > 0x10'FF'FF)
                    break;
            }

            return res;
        }

        static void Convert(const UTF32Char* input, DeviceSize input_size, UTF8Char* output) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
            while(res.input_offset != input_size)
            {
                auto utf8_codepoint = GetUTF8CodePoint(input[res.input_offset]);
                memcpy(output + res.output_size, utf8_codepoint.utf8.u8_data, utf8_codepoint.length);

                res.output_size += utf8_codepoint.length;
                res.input_offset++;
            }
        }

        //utf8 to other
        static DeviceSize GetUTF16Size(const UTF8Char* input, DeviceSize input_size) noexcept
        {
            DeviceSize out = 0;

            DeviceSize offset = 0;
            while(offset != input_size)
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(input + offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                if(utf32 < TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT)
                    out++;
                else
                    out += 2;

                offset += utf8.length;
            }

            return out;
        }

        static DeviceSize GetUTF32Size(const UTF8Char* input, DeviceSize input_size) noexcept
        {
            DeviceSize out = 0;

            DeviceSize offset = 0;
            while(offset != input_size)
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(input + offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                out++;

                offset += utf8.length;
            }

            return out;
        }

        static DeviceSize GetWideCharSize(const UTF8Char* input, DeviceSize input_size) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return GetUTF16Size(input, input_size);
            else if constexpr(WIDE_CHAR_IS_UTF32)
                return GetUTF32Size(input, input_size);
            else
                return input_size;
        }

        static void ConvertToUTF16(const UTF8Char* input, DeviceSize input_size, UTF16Char* output) noexcept
        {
            DeviceSize output_offset = 0;

            DeviceSize input_offset = 0;
            while(input_offset != input_size)
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(input + input_offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                if(utf32 < TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT)
                {
                    output[output_offset] = static_cast<UTF16Char>(utf32);
                    output_offset++;
                }
                else
                {
                    output[output_offset] = ((utf32 - TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT) >> 10) + HIGH_SURROGATE_PAIR_RANGE.first;
                    output[output_offset + 1] = ((utf32 - TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT) % 0x400) + LOW_SURROGATE_PAIR_RANGE.first;

                    output_offset += 2;
                }

                input_offset += utf8.length;
            }
        }

        static void ConvertToUTF32(const UTF8Char* input, DeviceSize input_size, UTF32Char* output) noexcept
        {
            DeviceSize output_offset = 0;

            DeviceSize input_offset = 0;
            while(input_offset != input_size)
            {
                UTF32Char utf32 = StringEncoder::GetUTF32Codepoint(input + input_offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                output[output_offset] = utf32;

                output_offset++;
                input_offset += utf8.length;
            }
        }

        static void ConvertToWideChar(const UTF8Char* input, DeviceSize input_size, WideChar* output) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return ConvertToUTF16(input, input_size, reinterpret_cast<UTF16Char*>(output));
            else if constexpr(WIDE_CHAR_IS_UTF32)
                return ConvertToUTF32(input, input_size, reinterpret_cast<UTF32Char*>(output));
            else
                memcpy(output, input, input_size);
        }
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

        template<typename C>
        requires SameAs<UTF8Char, C> || SameAs<const UTF8Char, C>
        class StringCharIteratorRangeAdaptor
        {
        public:
            using Iterator = StringCharIterator<C>;

            explicit StringCharIteratorRangeAdaptor(C* data, DeviceSize size) noexcept
                : data(data),
                  size(size)
            {}

            StringCharIteratorRangeAdaptor() = default;
            ~StringCharIteratorRangeAdaptor() = default;
            StringCharIteratorRangeAdaptor(const StringCharIteratorRangeAdaptor&) = default;
            StringCharIteratorRangeAdaptor(StringCharIteratorRangeAdaptor&&) = default;
            StringCharIteratorRangeAdaptor& operator=(const StringCharIteratorRangeAdaptor&) = default;
            StringCharIteratorRangeAdaptor& operator=(StringCharIteratorRangeAdaptor&&) = default;

            Iterator GetIterator() noexcept
            {
                return Iterator(this->data);
            }

            Iterator GetSentinel() noexcept
            {
                return Iterator(this->data + this->size);
            }
        private:
            C* data;
            DeviceSize size;
        };

        //std compat
        template<typename T>
        requires TypeInstantiation<DropConstVolatileReference<T>, StringCharIteratorRangeAdaptor>
        auto begin(T&& rng) noexcept
        {
            return Forward(rng).GetIterator();
        }

        template<typename T>
        requires TypeInstantiation<DropConstVolatileReference<T>, StringCharIteratorRangeAdaptor>
        auto end(T&& rng) noexcept
        {
            return Forward(rng).GetSentinel();
        }

        //string utils
        const UTF8Char* FindInString(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        const UTF8Char* FindInStringReverse(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        Bool StringStartsWith(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        Bool StringEndsWith(const UTF8Char* data, DeviceSize data_size, const UTF8Char* input, DeviceSize input_size) noexcept;

        Bool CompareStringsEquality(const UTF8Char* data1, DeviceSize data_size1, const UTF8Char* data2, DeviceSize data_size2) noexcept;

        Bool CompareStringsLexicallyLess(const UTF8Char* data1, DeviceSize data_size1, const UTF8Char* data2, DeviceSize data_size2) noexcept;

    };
};