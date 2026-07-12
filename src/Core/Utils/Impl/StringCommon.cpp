#include "StringCommon.h"

namespace Core
{
    DeviceSize StringEncoder::GetCodePointSize(UTF32Char codepoint) noexcept
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

    UTF8CodePoint StringEncoder::GetUTF8CodePoint(UTF32Char codepoint) noexcept
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

    UTF32Char StringEncoder::GetUTF32Codepoint(const UTF8Char* input) noexcept
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
    StringEncoderLengthResult StringEncoder::GetLength(const Char* input, DeviceSize input_size) noexcept
    {
        return GetLength(reinterpret_cast<const UTF8Char*>(input), input_size);
    }

    Void StringEncoder::Convert(const Char* input, DeviceSize input_size, UTF8Char* output) noexcept
    {
        return Convert(reinterpret_cast<const UTF8Char*>(input), input_size, output);
    }

    //WideChar
    StringEncoderLengthResult StringEncoder::GetLength(const WideChar* input, DeviceSize input_size) noexcept
    {
        if constexpr(WIDE_CHAR_IS_UTF16)
            return GetLength(reinterpret_cast<const UTF16Char*>(input), input_size);
        else if(WIDE_CHAR_IS_UTF32)
            return GetLength(reinterpret_cast<const UTF32Char*>(input), input_size);
        else
            return GetLength(reinterpret_cast<const UTF8Char*>(input), input_size);
    }

    Void StringEncoder::Convert(const WideChar* input, DeviceSize input_size, UTF8Char* output) noexcept
    {
        if constexpr(WIDE_CHAR_IS_UTF16)
            Convert(reinterpret_cast<const UTF16Char*>(input), input_size, output);
        else if(WIDE_CHAR_IS_UTF32)
            Convert(reinterpret_cast<const UTF32Char*>(input), input_size, output);
        else
            Convert(reinterpret_cast<const UTF8Char*>(input), input_size, output);
    }

    //UTF8Char
    StringEncoderLengthResult StringEncoder::GetLength(const UTF8Char* input, DeviceSize input_size) noexcept
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

    Void StringEncoder::Convert(const UTF8Char* input, DeviceSize input_size, UTF8Char* output) noexcept
    {
        CopyNonOverlappedMemory(input, output, input_size);
    }

    //UTF16Char
    StringEncoderLengthResult StringEncoder::GetLength(const UTF16Char* input, DeviceSize input_size) noexcept
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

    Void StringEncoder::Convert(const UTF16Char* input, DeviceSize input_size, UTF8Char* output) noexcept
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
            for(DeviceSize i = 0; i < utf8_codepoint.length; i++)
                output[res.output_size + i] = utf8_codepoint.utf8.u8_data[i];

            res.output_size += utf8_codepoint.length;
        }
    }

    //UTF32Char
    StringEncoderLengthResult StringEncoder::GetLength(const UTF32Char* input, DeviceSize input_size) noexcept
    {
        StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
        while(res.input_offset != input_size)
        {
            if(input[res.output_size] > 0x10'FF'FF)
                break;
        }

        return res;
    }

    Void StringEncoder::Convert(const UTF32Char* input, DeviceSize input_size, UTF8Char* output) noexcept
    {
        StringEncoderLengthResult res = {.input_offset = 0, .output_size = 0};
        while(res.input_offset != input_size)
        {
            auto utf8_codepoint = GetUTF8CodePoint(input[res.input_offset]);
            for(DeviceSize i = 0; i < utf8_codepoint.length; i++)
                output[res.output_size + i] = utf8_codepoint.utf8.u8_data[i];

            res.output_size += utf8_codepoint.length;
            res.input_offset++;
        }
    }

    //utf8 to other
    DeviceSize StringEncoder::GetUTF16Size(const UTF8Char* input, DeviceSize input_size) noexcept
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

    DeviceSize StringEncoder::GetUTF32Size(const UTF8Char* input, DeviceSize input_size) noexcept
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

    DeviceSize StringEncoder::GetWideCharSize(const UTF8Char* input, DeviceSize input_size) noexcept
    {
        if constexpr(WIDE_CHAR_IS_UTF16)
            return GetUTF16Size(input, input_size);
        else if constexpr(WIDE_CHAR_IS_UTF32)
            return GetUTF32Size(input, input_size);
        else
            return input_size;
    }

    Void StringEncoder::ConvertToUTF16(const UTF8Char* input, DeviceSize input_size, UTF16Char* output) noexcept
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

    Void StringEncoder::ConvertToUTF32(const UTF8Char* input, DeviceSize input_size, UTF32Char* output) noexcept
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

    Void StringEncoder::ConvertToWideChar(const UTF8Char* input, DeviceSize input_size, WideChar* output) noexcept
    {
        if constexpr(WIDE_CHAR_IS_UTF16)
            return ConvertToUTF16(input, input_size, reinterpret_cast<UTF16Char*>(output));
        else if constexpr(WIDE_CHAR_IS_UTF32)
            return ConvertToUTF32(input, input_size, reinterpret_cast<UTF32Char*>(output));
        else
            CopyNonOverlappedMemory(input, output, input_size);
    }

    namespace Detail
    {
        const char8_t* FindInString(const char8_t* data, DeviceSize data_size, const char8_t* input, DeviceSize input_size) noexcept
        {
            if(input_size == 0)
                return nullptr;

            DeviceSize str_offset = 0;
            while(data_size - str_offset >= input_size)
            {
                DeviceSize input_offset = 0;
                for(; input_offset < input_size; input_offset++)
                {
                    if(input[input_offset] != data[str_offset + input_offset])
                        break;
                }

                if(input_offset == input_size)
                    return data + str_offset;

                str_offset++;
            }

            return nullptr;
        }

        const char8_t* FindInStringReverse(const char8_t* data, DeviceSize data_size, const char8_t* input, DeviceSize input_size) noexcept
        {
            if(input_size == 0)
                return data + data_size;

            if(data_size < input_size)
                return nullptr;

            //1 2 3 4 5
            //      4 5
            DeviceSize str_offset = data_size - input_size;
            while(str_offset >= 0)
            {
                DeviceSize input_offset = 0;
                for(; input_offset < input_size; input_offset++)
                {
                    if(input[input_offset] != data[str_offset + input_offset])
                        break;
                }

                if(input_offset == input_size)
                    return data + str_offset;

                if(str_offset == 0)
                    break;

                str_offset--;
            }

            return nullptr;
        }

        Bool StringStartsWith(const char8_t* data, DeviceSize data_size, const char8_t* input, DeviceSize input_size) noexcept
        {
            if(input_size == 0)
                return true;

            if(data_size < input_size)
                return false;

            for(DeviceSize i = 0; i < input_size; i++)
            {
                if(input[i] != data[i])
                    return false;
            }

            return true;
        }

        Bool StringEndsWith(const char8_t* data, DeviceSize data_size, const char8_t* input, DeviceSize input_size) noexcept
        {
            if(input_size == 0)
                return true;

            if(data_size < input_size)
                return false;

            DeviceSize str_offset = (data_size - input_size);
            for(DeviceSize i = 0; i < input_size; i++)
            {
                if(input[i] != data[str_offset + i])
                    return false;
            }

            return true;
        }

        Bool CompareStringsEquality(const char8_t* data1, DeviceSize data_size1, const char8_t* data2, DeviceSize data_size2) noexcept
        {
            if(data_size1 == 0 && data_size2 == 0)
                return true;
            else if(data_size1 != data_size2)
                return false;
            else
                return memcmp(data1, data2, data_size1) == 0;
        }

        Bool CompareStringsLexicallyLess(const char8_t* data1, DeviceSize data_size1, const char8_t* data2, DeviceSize data_size2) noexcept
        {
            //for each string get str1_codepoint and str2_codepoint
            //if str1_codepoint < str2_codepoint -> true
            //else if str1_codepoint > str2_codepoint -> false
            //else continue
            //after all return false

            using Iterator = ::Core::Detail::StringCharIterator<const char8_t>;

            auto str1_begin = Iterator(data1);
            auto str1_end = Iterator(data1 + data_size1);

            auto str2_begin = Iterator(data2);
            auto str2_end = Iterator(data2 + data_size2);

            while(str1_begin != str1_end && str2_begin != str2_end)
            {
                auto str1_codepoint = *str1_begin;
                auto str2_codepoint = *str2_begin;

                if(str1_codepoint.utf32 < str2_codepoint.utf32)
                    return true;
                else if(str1_codepoint.utf32 > str2_codepoint.utf32)
                    return false;

                str1_begin++;
                str2_begin++;
            }

            //prefix is same for both -> select shorter string
            if(data_size1 < data_size2)
                return true;

            return false;
        }
    };
};