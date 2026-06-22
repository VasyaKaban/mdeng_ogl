#pragma once

#include <concepts>
#include <ranges>
#include "../Instantiation.hpp"

namespace Core
{
    struct StringEncoderLengthResult
    {
        size_t input_offset;
        size_t output_size;
    };

    struct UTF8CodePoint
    {
        union
        {
            char8_t u8_data[4];
            char data[4];
        } utf8;
        uint8_t length;
    };

    struct CodePoint
    {
        UTF8CodePoint utf8;
        char32_t utf32;
    };

    template<typename C>
    concept Character = std::same_as<C, char> || std::same_as<C, wchar_t> || std::same_as<C, char8_t> || std::same_as<C, char16_t> || std::same_as<C, char32_t>;

    struct SurrogatePair
    {
        char16_t first;
        char16_t last;
    };

    constexpr inline bool WIDE_CHAR_IS_UTF16 = (WCHAR_MAX == 0xFF'FF);
    constexpr inline bool WIDE_CHAR_IS_UTF32 = (WCHAR_MAX > 0xFF'FF);

    constexpr inline SurrogatePair HIGH_SURROGATE_PAIR_RANGE = {.first = 0xD8'00, .last = 0xDB'FF};
    constexpr inline SurrogatePair LOW_SURROGATE_PAIR_RANGE = {.first = 0xDC'00, .last = 0xDF'FF};

    constexpr inline char32_t TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT = 0x1'00'00;

    //Let 'char' to be a valid UTF8 char
    //TODO: add enum VariableLengthEncoding for 'char' decode
    struct StringEncoder
    {
        static size_t GetCodePointSize(char32_t codepoint) noexcept
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

        static UTF8CodePoint GetUTF8CodePoint(char32_t codepoint) noexcept
        {
            UTF8CodePoint res;
            if(codepoint <= 0x00'7F)
            {
                res.length = 1;
                res.utf8 = {static_cast<char8_t>(codepoint & 0b0111'1111)};
            }
            else if(codepoint <= 0x07'FF)
            {
                res.length = 2;
                res.utf8 = {static_cast<char8_t>(((codepoint >> 6) & 0b0001'1111) | 0b1100'0000), static_cast<char8_t>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }
            else if(codepoint <= 0xFF'FF)
            {
                res.length = 3;
                res.utf8 = {static_cast<char8_t>(((codepoint >> 12) & 0b0000'1111) | 0b1110'0000),
                            static_cast<char8_t>(((codepoint >> 6) & 0b0011'1111) | 0b100'0000),
                            static_cast<char8_t>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }
            else //if(codepoint < 0x10'FF'FF)
            {
                res.length = 4;
                res.utf8 = {static_cast<char8_t>(((codepoint >> 18) & 0b0000'0111) | 0b1111'0000),
                            static_cast<char8_t>(((codepoint >> 12) & 0b0011'1111) | 0b1000'0000),
                            static_cast<char8_t>(((codepoint >> 6) & 0b0011'1111) | 0b1000'0000),
                            static_cast<char8_t>((codepoint & 0b0011'1111) | 0b1000'0000)};
            }

            return res;
        }

        static char32_t GetUTF32Codepoint(const char8_t* input) noexcept
        {
            if((input[0] & 0b1000'0000) == 0) //one char
                return static_cast<char32_t>(input[0]);
            else if((input[0] & 0b1110'0000) == 0b1100'0000)
                return (static_cast<char32_t>(input[0] & 0b0001'1111) << 6) | static_cast<char32_t>(input[1] & 0b0011'1111);
            else if((input[0] & 0b1111'0000) == 0b1110'0000)
                return (static_cast<char32_t>(input[0] & 0b0000'1111) << 12) | (static_cast<char32_t>(input[1] & 0b0011'1111) << 6) | static_cast<char32_t>(input[2] & 0b0011'1111);
            else //if((input[0] & 0b1111'1000) == 0b1111'0000)
                return (static_cast<char32_t>(input[0] & 0b0000'0111) << 18) | (static_cast<char32_t>(input[1] & 0b0011'1111) << 12) | (static_cast<char32_t>(input[2] & 0b0011'1111) << 6) |
                       static_cast<char32_t>(input[3] & 0b0011'1111);
        }

        //char
        static StringEncoderLengthResult GetLength(const char* input, size_t input_size) noexcept
        {
            return GetLength(reinterpret_cast<const char8_t*>(input), input_size);
        }

        static void Convert(const char* input, size_t input_size, char8_t* output) noexcept
        {
            return Convert(reinterpret_cast<const char8_t*>(input), input_size, output);
        }

        //wchar_t
        static StringEncoderLengthResult GetLength(const wchar_t* input, size_t input_size) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return GetLength(reinterpret_cast<const char16_t*>(input), input_size);
            else if(WIDE_CHAR_IS_UTF32)
                return GetLength(reinterpret_cast<const char32_t*>(input), input_size);
            else
                return GetLength(reinterpret_cast<const char8_t*>(input), input_size);
        }

        static void Convert(const wchar_t* input, size_t input_size, char8_t* output) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                Convert(reinterpret_cast<const char16_t*>(input), input_size, output);
            else if(WIDE_CHAR_IS_UTF32)
                Convert(reinterpret_cast<const char32_t*>(input), input_size, output);
            else
                Convert(reinterpret_cast<const char8_t*>(input), input_size, output);
        }

        //char8_t
        static StringEncoderLengthResult GetLength(const char8_t* input, size_t input_size) noexcept
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

        static void Convert(const char8_t* input, size_t input_size, char8_t* output) noexcept
        {
            memcpy(output, input, input_size);
        }

        //char16_t
        static StringEncoderLengthResult GetLength(const char16_t* input, size_t input_size) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};

            char32_t codepoint;
            while(res.input_offset != input_size)
            {
                if(input[res.input_offset] >= LOW_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= LOW_SURROGATE_PAIR_RANGE.last) //low surogate
                    break;
                else if(input[res.input_offset] >= HIGH_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= HIGH_SURROGATE_PAIR_RANGE.last) //high surrogate
                {
                    if(input_size < 2 || !(input[res.input_offset + 1] >= LOW_SURROGATE_PAIR_RANGE.first && input[res.input_offset + 1] <= LOW_SURROGATE_PAIR_RANGE.last))
                        break;

                    char16_t high_surrogate = input[res.input_offset];
                    char16_t low_surrogate = input[res.input_offset + 1];

                    codepoint = ((static_cast<char32_t>(high_surrogate) - static_cast<char32_t>(HIGH_SURROGATE_PAIR_RANGE.first)) << 10) +
                                (static_cast<char32_t>(low_surrogate) - static_cast<char32_t>(LOW_SURROGATE_PAIR_RANGE.first)) + 0x1'00'00;

                    res.input_offset += 2;
                }
                else
                {
                    codepoint = static_cast<char32_t>(input[res.input_offset]);

                    res.input_offset++;
                }

                res.output_size += GetCodePointSize(codepoint);
            }

            return res;
        }

        static void Convert(const char16_t* input, size_t input_size, char8_t* output) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};

            char32_t codepoint;
            while(res.input_offset != input_size)
            {
                if(input[res.input_offset] >= HIGH_SURROGATE_PAIR_RANGE.first && input[res.input_offset] <= HIGH_SURROGATE_PAIR_RANGE.last) //high surrogate
                {
                    char16_t high_surrogate = input[res.input_offset];
                    char16_t low_surrogate = input[res.input_offset + 1];

                    codepoint = ((static_cast<char32_t>(high_surrogate) - static_cast<char32_t>(HIGH_SURROGATE_PAIR_RANGE.first)) << 10) +
                                (static_cast<char32_t>(low_surrogate) - static_cast<char32_t>(LOW_SURROGATE_PAIR_RANGE.first)) + 0x1'00'00;

                    res.input_offset += 2;
                }
                else
                {
                    codepoint = static_cast<char32_t>(input[res.input_offset]);

                    res.input_offset++;
                }

                auto utf8_codepoint = GetUTF8CodePoint(codepoint);
                memcpy(output + res.output_size, utf8_codepoint.utf8.u8_data, utf8_codepoint.length);

                res.output_size += utf8_codepoint.length;
            }
        }

        //char32_t
        static StringEncoderLengthResult GetLength(const char32_t* input, size_t input_size) noexcept
        {
            StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
            while(res.input_offset != input_size)
            {
                if(input[res.output_size] > 0x10'FF'FF)
                    break;
            }

            return res;
        }

        static void Convert(const char32_t* input, size_t input_size, char8_t* output) noexcept
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
        static size_t GetUTF16Size(const char8_t* input, size_t input_size) noexcept
        {
            size_t out = 0;

            size_t offset = 0;
            while(offset != input_size)
            {
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(input + offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                if(utf32 < TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT)
                    out++;
                else
                    out += 2;

                offset += utf8.length;
            }

            return out;
        }

        static size_t GetUTF32Size(const char8_t* input, size_t input_size) noexcept
        {
            size_t out = 0;

            size_t offset = 0;
            while(offset != input_size)
            {
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(input + offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                out++;

                offset += utf8.length;
            }

            return out;
        }

        static size_t GetWideCharSize(const char8_t* input, size_t input_size) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return GetUTF16Size(input, input_size);
            else if constexpr(WIDE_CHAR_IS_UTF32)
                return GetUTF32Size(input, input_size);
            else
                return input_size;
        }

        static void ConvertToUTF16(const char8_t* input, size_t input_size, char16_t* output) noexcept
        {
            size_t output_offset = 0;

            size_t input_offset = 0;
            while(input_offset != input_size)
            {
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(input + input_offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                if(utf32 < TWO_UTF16_CODE_UNITS_FIRST_CODE_POINT)
                {
                    output[output_offset] = static_cast<char16_t>(utf32);
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

        static void ConvertToUTF32(const char8_t* input, size_t input_size, char32_t* output) noexcept
        {
            size_t output_offset = 0;

            size_t input_offset = 0;
            while(input_offset != input_size)
            {
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(input + input_offset);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);

                output[output_offset] = utf32;

                output_offset++;
                input_offset += utf8.length;
            }
        }

        static void ConvertToWideChar(const char8_t* input, size_t input_size, wchar_t* output) noexcept
        {
            if constexpr(WIDE_CHAR_IS_UTF16)
                return ConvertToUTF16(input, input_size, reinterpret_cast<char16_t*>(output));
            else if constexpr(WIDE_CHAR_IS_UTF32)
                return ConvertToUTF32(input, input_size, reinterpret_cast<char32_t*>(output));
            else
                memcpy(output, input, input_size);
        }
    };

    namespace Detail
    {
        template<typename C>
        requires std::same_as<char8_t, C> || std::same_as<const char8_t, C>
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
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(this->data);
                auto utf8 = StringEncoder::GetUTF8CodePoint(utf32);
                this->data += utf8.length;

                return *this;
            }

            template<typename OC>
            bool operator==(const StringCharIterator<OC>& it) const noexcept
            {
                return this->data == it.data;
            }

            CodePoint operator*() const noexcept
            {
                char32_t utf32 = StringEncoder::GetUTF32Codepoint(this->data);
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
        requires std::same_as<char8_t, C> || std::same_as<const char8_t, C>
        class StringCharIteratorRangeAdaptor
        {
        public:
            using Iterator = StringCharIterator<C>;

            explicit StringCharIteratorRangeAdaptor(C* data, size_t size) noexcept
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
            size_t size;
        };

        //std compat
        template<typename T>
        requires TypeInstantiation<std::remove_cvref_t<T>, StringCharIteratorRangeAdaptor>
        auto begin(T&& rng) noexcept
        {
            return std::forward<T>(rng).GetIterator();
        }

        template<typename T>
        requires TypeInstantiation<std::remove_cvref_t<T>, StringCharIteratorRangeAdaptor>
        auto end(T&& rng) noexcept
        {
            return std::forward<T>(rng).GetSentinel();
        }

        //string utils
        const char8_t* FindInString(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept;

        const char8_t* FindInStringReverse(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept;

        bool StringStartsWith(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept;

        bool StringEndsWith(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept;

        bool CompareStringsEquality(const char8_t* data1, size_t data_size1, const char8_t* data2, size_t data_size2) noexcept;

        bool CompareStringsLexicallyLess(const char8_t* data1, size_t data_size1, const char8_t* data2, size_t data_size2) noexcept;

    };
};

//std compat
namespace std
{
    template<typename C>
    struct iterator_traits<::Core::Detail::StringCharIterator<C>>
    {
        using difference_type = ptrdiff_t;
        using value_type = ::Core::CodePoint;
        using pointer = void;
        using reference = void;
        using iterator_category = std::forward_iterator_tag;
    };
};
