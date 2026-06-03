#include "Unicode.h"

namespace Core
{
    static bool IsValidNonPrefixUTF8Byte(char value) noexcept
    {
        return (value & 0b1100'0000) == 0b1000'0000;
    }

    static bool
    GetNextUTF8Codepoint(std::string_view str, char32_t& output, std::size_t& offset) noexcept
    {
        if(offset == str.size())
            return 0;

        bool res = false;
        if((str[offset] & 0b1000'0000) == 0b0000'0000) //1
        {
            res = true;

            output = str[offset];
            offset++;
        }
        else if((str[offset] & 0b1110'0000) == 0b1100'0000) //2
        {
            if(str.size() - offset >= 2)
            {
                if(IsValidNonPrefixUTF8Byte(str[offset + 1]))
                {
                    res = true;

                    output = (static_cast<char32_t>(str[offset] & 0b1101'1111) << 8) |
                             (str[offset + 1] & 0b1011'1111);

                    offset += 2;
                }
            }
        }
        else if((str[offset] & 0b1111'0000) == 0b1110'0000) //3
        {
            if(str.size() - offset >= 3)
            {
                if(IsValidNonPrefixUTF8Byte(str[offset + 1]) &&
                   IsValidNonPrefixUTF8Byte(str[offset + 2]))
                {
                    res = true;

                    output = (static_cast<char32_t>(str[offset] & 0b1110'1111) << 16) |
                             (static_cast<char32_t>(str[offset + 1] & 0b1011'1111) << 8) |
                             (str[offset + 2] & 0b1011'1111);

                    offset += 3;
                }
            }
        }
        else if((str[offset] & 0b1111'1000) == 0b1111'0000) //4
        {
            if(str.size() - offset >= 4)
            {
                if(IsValidNonPrefixUTF8Byte(str[offset + 1]) &&
                   IsValidNonPrefixUTF8Byte(str[offset + 2]) &&
                   IsValidNonPrefixUTF8Byte(str[offset + 3]))
                {
                    res = true;

                    output = (static_cast<char32_t>(str[offset] & 0b1111'0111) << 24) |
                             (static_cast<char32_t>(str[offset + 1] & 0b1011'1111) << 16) |
                             (static_cast<char32_t>(str[offset + 2] & 0b1011'1111) << 8) |
                             (str[offset + 3] & 0b1011'1111);

                    offset += 4;
                }
            }
        }

        return res;
    }

    static bool IsHighSurrogate(char16_t value) noexcept
    {
        return value >= 0xD8'00 && value <= 0xDB'FF;
    }

    static bool IsLowSurrogate(char16_t value) noexcept
    {
        return value >= 0xDC'00 && value <= 0xDF'FF;
    }

    static bool
    GetNextUTF16Codepoint(std::u16string_view str, char32_t& output, std::size_t& offset) noexcept
    {
        if(offset == str.size())
            return 0;

        bool res = false;
        if(IsHighSurrogate(str[offset]))
        {
            if(str.size() - offset >= 2)
            {
                if(IsLowSurrogate(str[offset + 1]))
                {
                    res = true;

                    output =
                        ((str[offset] - 0xD8'00) << 10) + (str[offset + 1] - 0xDC'00) + 0x1'00'00;
                    offset += 2;
                }
            }
        }
        else
        {
            if(!IsLowSurrogate(str[offset]))
            {
                res = true;

                output = str[offset];
                offset++;
            }
        }

        return res;
    }

    static std::size_t CodepointToUTF8(char32_t codepoint, char output[4]) noexcept
    {
        std::size_t res = 0;

        if(codepoint <= 0x00'7F)
        {
            output[0] = static_cast<char>(codepoint);

            res = 1;
        }
        else if(codepoint <= 0x07'FF)
        {
            output[0] = 0b1100'0000 | ((codepoint >> 6) & 0b1'1111);
            output[1] = 0b1000'0000 | (codepoint & (0b11'1111));

            res = 2;
        }
        else if(codepoint <= 0xFF'FF)
        {
            output[0] = 0b1110'0000 | ((codepoint >> 12) & 0b1111);
            output[1] = 0b1000'0000 | ((codepoint >> 6) & 0b11'1111);
            output[2] = 0b1000'0000 | (codepoint & (0b11'1111));

            res = 3;
        }
        else if(codepoint <= 0x10'FF'FF)
        {
            output[0] = 0b1111'0000 | ((codepoint >> 18) & 0b111);
            output[1] = 0b1000'0000 | ((codepoint >> 12) & 0b11'1111);
            output[2] = 0b1000'0000 | ((codepoint >> 6) & 0b11'1111);
            output[3] = 0b1000'0000 | (codepoint & (0b11'1111));

            res = 4;
        }

        return res;
    }

    static std::size_t CodepointToUTF16(char32_t codepoint, char16_t output[2]) noexcept
    {
        std::size_t res = 0;

        if(codepoint < 0x1'00'00)
        {
            output[0] = static_cast<char16_t>(codepoint);

            res = 1;
        }
        else if(codepoint <= 0x10'FF'FF)
        {
            output[0] = ((codepoint - 0x1'00'00) >> 10) + 0xD8'00;
            output[1] = (((codepoint - 0x1'00'00) >> 10) & 0b11'1111'1111) + 0xDC'00;

            res = 2;
        }

        return res;
    }

    std::size_t GetUTF8ToUTF16Size(std::string_view input) noexcept
    {
        std::size_t output_size = 0;
        std::size_t offset = 0;
        char32_t c;
        char16_t buffer[2];

        while(offset < input.size())
        {
            if(!GetNextUTF8Codepoint(input, c, offset))
                break;

            std::size_t length = CodepointToUTF16(c, buffer);
            if(length == 0)
                break;

            output_size += length;
        }

        return output_size;
    }

    std::size_t
    UTF8ToUTF16(std::string_view input, char16_t* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        std::size_t offset = 0;
        char32_t c;
        char16_t buffer[2];

        while(offset < input.size() && output_offset < output_size)
        {
            if(!GetNextUTF8Codepoint(input, c, offset))
                break;

            std::size_t length = CodepointToUTF16(c, buffer);
            if(length == 0)
                break;

            if(output_offset + length >= output_size)
                break;

            for(std::size_t j = 0; j < length; j++)
                output[output_offset++] = buffer[j];
        }

        return output_offset;
    }

    std::u16string UTF8ToUTF16(std::string_view input)
    {
        std::size_t size = GetUTF8ToUTF16Size(input);
        std::u16string out(size, 0);
        UTF8ToUTF16(input, out.data(), size);

        return out;
    }

    std::size_t GetUTF8ToUTF32Size(std::string_view input) noexcept
    {
        std::size_t output_size = 0;
        std::size_t offset = 0;
        char32_t c;

        while(offset < input.size())
        {
            if(!GetNextUTF8Codepoint(input, c, offset))
                break;

            output_size++;
        }

        return output_size;
    }

    std::size_t
    UTF8ToUTF32(std::string_view input, char32_t* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        std::size_t offset = 0;
        char32_t c;

        while(offset < input.size() && output_offset < output_size)
        {
            if(!GetNextUTF8Codepoint(input, c, offset))
                break;

            output[output_offset++] = c;
        }

        return output_offset;
    }

    std::u32string UTF8ToUTF32(std::string_view input)
    {
        std::size_t size = GetUTF8ToUTF32Size(input);
        std::u32string out(size, 0);
        UTF8ToUTF32(input, out.data(), size);

        return out;
    }

    std::size_t GetUTF16ToUTF8Size(std::u16string_view input) noexcept
    {
        std::size_t output_size = 0;
        std::size_t offset = 0;
        char32_t c;
        char buffer[4];

        while(offset < input.size())
        {
            if(!GetNextUTF16Codepoint(input, c, offset))
                break;

            std::size_t length = CodepointToUTF8(c, buffer);
            if(length == 0)
                break;

            output_size += length;
        }

        return output_size;
    }
    std::size_t
    UTF16ToUTF8(std::u16string_view input, char* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        std::size_t offset = 0;
        char32_t c;
        char buffer[4];

        while(offset < input.size() && output_offset < output_size)
        {
            if(!GetNextUTF16Codepoint(input, c, offset))
                break;

            std::size_t length = CodepointToUTF8(c, buffer);
            if(length == 0)
                break;

            if(output_offset + length >= output_size)
                break;

            for(std::size_t j = 0; j < length; j++)
                output[output_offset++] = buffer[j];
        }

        return output_offset;
    }

    std::string UTF16ToUTF8(std::u16string_view input)
    {
        std::size_t size = GetUTF16ToUTF8Size(input);
        std::string out(size, 0);
        UTF16ToUTF8(input, out.data(), size);

        return out;
    }

    std::size_t GetUTF16ToUTF32Size(std::u16string_view input) noexcept
    {
        std::size_t output_size = 0;
        std::size_t offset = 0;
        char32_t c;

        while(offset < input.size())
        {
            if(!GetNextUTF16Codepoint(input, c, offset))
                break;

            output_size++;
        }

        return output_size;
    }

    std::size_t
    UTF16ToUTF32(std::u16string_view input, char32_t* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        std::size_t offset = 0;
        char32_t c;

        while(offset < input.size() && output_offset < output_size)
        {
            if(!GetNextUTF16Codepoint(input, c, offset))
                break;

            output[output_offset++] = c;
        }

        return output_offset;
    }

    std::u32string UTF16ToUTF32(std::u16string_view input)
    {
        std::size_t size = GetUTF16ToUTF32Size(input);
        std::u32string out(size, 0);
        UTF16ToUTF32(input, out.data(), size);

        return out;
    }

    std::size_t GetUTF32ToUTF8Size(std::u32string_view input) noexcept
    {
        std::size_t output_size = 0;
        char buffer[4];

        for(char32_t c: input)
        {
            std::size_t length = CodepointToUTF8(c, buffer);
            if(length == 0)
                break;

            output_size += length;
        }
        return output_size;
    }

    std::size_t
    UTF32ToUTF8(std::u32string_view input, char* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        char buffer[4];

        for(std::size_t i = 0; i < input.size() && output_offset < output_size; i++)
        {
            char32_t c = input[i];
            std::size_t length = CodepointToUTF8(c, buffer);
            if(length == 0)
                break;

            if(output_offset + length >= output_size)
                break;

            for(std::size_t j = 0; j < length; j++)
                output[output_offset++] = buffer[j];
        }

        return output_offset;
    }

    std::string UTF32ToUTF8(std::u32string_view input)
    {
        std::size_t size = GetUTF32ToUTF8Size(input);
        std::string out(size, 0);
        UTF32ToUTF8(input, out.data(), size);

        return out;
    }

    std::size_t GetUTF32ToUTF16Size(std::u32string_view input) noexcept
    {
        std::size_t output_size = 0;
        char16_t buffer[2];

        for(char32_t c: input)
        {
            std::size_t length = CodepointToUTF16(c, buffer);
            if(length == 0)
                break;

            output_size += length;
        }
        return output_size;
    }

    std::size_t
    UTF32ToUTF16(std::u32string_view input, char16_t* output, std::size_t output_size) noexcept
    {
        std::size_t output_offset = 0;
        char16_t buffer[2];

        for(std::size_t i = 0; i < input.size() && output_offset < output_size; i++)
        {
            char32_t c = input[i];
            std::size_t length = CodepointToUTF16(c, buffer);
            if(length == 0)
                break;

            if(output_offset + length >= output_size)
                break;

            for(std::size_t j = 0; j < length; j++)
                output[output_offset++] = buffer[j];
        }

        return output_offset;
    }

    std::u16string UTF32ToUTF16(std::u32string_view input)
    {
        std::size_t size = GetUTF32ToUTF16Size(input);
        std::u16string out(size, 0);
        UTF32ToUTF16(input, out.data(), size);

        return out;
    }

    bool CompareUTF8(std::string_view str1, std::string_view str2) noexcept
    {
        //for each string get str1_codepoint and str2_codepoint
        //if str1_codepoint < str2_codepoint -> true
        //else if str1_codepoint > str2_codepoint -> false
        //else continue
        //after all return false

        std::size_t str1_offset = 0;
        std::size_t str2_offset = 0;

        while(str1_offset != str1.size() && str2_offset != str2.size())
        {
            char32_t str1_code;
            bool str1_res = GetNextUTF8Codepoint(str1, str1_code, str1_offset);
            if(!str1_res)
                return false;

            char32_t str2_code;
            bool str2_res = GetNextUTF8Codepoint(str2, str2_code, str2_offset);
            if(!str2_res)
                return false;

            if(str1_code < str2_code)
                return true;
            else if(str1_code > str2_code)
                return false;
        }

        if(str1_offset <= str2_offset)
            return true;

        return false;
    }

    bool CompareUTF16(std::u16string_view str1, std::u16string_view str2) noexcept
    {
        //for each string get str1_codepoint and str2_codepoint
        //if str1_codepoint < str2_codepoint -> true
        //else if str1_codepoint > str2_codepoint -> false
        //else continue
        //after all return false

        std::size_t str1_offset = 0;
        std::size_t str2_offset = 0;

        while(str1_offset != str1.size() && str2_offset != str2.size())
        {
            char32_t str1_code;
            bool str1_res = GetNextUTF16Codepoint(str1, str1_code, str1_offset);
            if(!str1_res)
                return false;

            char32_t str2_code;
            bool str2_res = GetNextUTF16Codepoint(str2, str2_code, str2_offset);
            if(!str2_res)
                return false;

            if(str1_code < str2_code)
                return true;
            else if(str1_code > str2_code)
                return false;
        }

        return str1_offset <= str2_offset;
    }

    bool CompareUTF32(std::u32string_view str1, std::u32string_view str2) noexcept
    {
        std::size_t length = std::min(str1.size(), str2.size());

        std::u32string_view str1_view{str1.data(), length};
        std::u32string_view str2_view{str2.data(), length};

        std::strong_ordering ordering = str1 <=> str2;
        if(ordering == std::strong_ordering::less)
            return true;
        else if(ordering == std::strong_ordering::greater)
            return false;
        else
            return str1.size() < str2.size();
    }

    bool IsUnicodeC0ControlCode(char value) noexcept
    {
        return (value >= 0 && value <= 31) || value == 127;
    }

    bool IsUnicodeC0ControlCodeOrSpace(char value) noexcept
    {
        return IsUnicodeC0ControlCode(value) || value == 32;
    }
};