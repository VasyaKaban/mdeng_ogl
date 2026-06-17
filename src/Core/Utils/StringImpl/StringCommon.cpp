#include "StringCommon.h"

namespace Core
{
    namespace Detail
    {
        const char8_t* FindInString(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept
        {
            if(input_size == 0)
                return nullptr;

            size_t str_offset = 0;
            while(data_size - str_offset >= input_size)
            {
                size_t input_offset = 0;
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

        const char8_t* FindInStringReverse(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept
        {
            if(input_size == 0)
                return data + data_size;

            if(data_size < input_size)
                return nullptr;

            //1 2 3 4 5
            //      4 5
            size_t str_offset = data_size - input_size;
            while(str_offset >= 0)
            {
                size_t input_offset = 0;
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

        bool StringStartsWith(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept
        {
            if(input_size == 0)
                return true;

            if(data_size < input_size)
                return false;

            for(size_t i = 0; i < input_size; i++)
            {
                if(input[i] != data[i])
                    return false;
            }

            return true;
        }

        bool StringEndsWith(const char8_t* data, size_t data_size, const char8_t* input, size_t input_size) noexcept
        {
            if(input_size == 0)
                return true;

            if(data_size < input_size)
                return false;

            size_t str_offset = (data_size - input_size);
            for(size_t i = 0; i < input_size; i++)
            {
                if(input[i] != data[str_offset + i])
                    return false;
            }

            return true;
        }

        bool CompareStringsEquality(const char8_t* data1, size_t data_size1, const char8_t* data2, size_t data_size2) noexcept
        {
            if(data_size1 == 0 && data_size2 == 0)
                return true;
            else if(data_size1 != data_size2)
                return false;
            else
                return memcmp(data1, data2, data_size1) == 0;
        }

        bool CompareStringsLexicallyLess(const char8_t* data1, size_t data_size1, const char8_t* data2, size_t data_size2) noexcept
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