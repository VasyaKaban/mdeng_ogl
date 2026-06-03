#pragma once

#include "../API.h"
#include <string_view>
#include <string>

namespace Core
{
    CORE_API std::size_t GetUTF8ToUTF16Size(std::string_view input) noexcept;
    CORE_API std::size_t
    UTF8ToUTF16(std::string_view input, char16_t* output, std::size_t output_size) noexcept;
    CORE_API std::u16string UTF8ToUTF16(std::string_view input);

    CORE_API std::size_t GetUTF8ToUTF32Size(std::string_view input) noexcept;
    CORE_API std::size_t
    UTF8ToUTF32(std::string_view input, char32_t* output, std::size_t output_size) noexcept;
    CORE_API std::u32string UTF8ToUTF32(std::string_view input);

    CORE_API std::size_t GetUTF16ToUTF8Size(std::u16string_view input) noexcept;
    CORE_API std::size_t
    UTF16ToUTF8(std::u16string_view input, char* output, std::size_t output_size) noexcept;
    CORE_API std::string UTF16ToUTF8(std::u16string_view input);

    CORE_API std::size_t GetUTF16ToUTF32Size(std::u16string_view input) noexcept;
    CORE_API std::size_t
    UTF16ToUTF32(std::u16string_view input, char32_t* output, std::size_t output_size) noexcept;
    CORE_API std::u32string UTF16ToUTF32(std::u16string_view input);

    CORE_API std::size_t GetUTF32ToUTF8Size(std::u32string_view input) noexcept;
    CORE_API std::size_t
    UTF32ToUTF8(std::u32string_view input, char* output, std::size_t output_size) noexcept;
    CORE_API std::string UTF32ToUTF8(std::u32string_view input);

    CORE_API std::size_t GetUTF32ToUTF16Size(std::u32string_view input) noexcept;
    CORE_API std::size_t
    UTF32ToUTF16(std::u32string_view input, char16_t* output, std::size_t output_size) noexcept;
    CORE_API std::u16string UTF32ToUTF16(std::u32string_view input);

    //alphanumeric comparator -> same as operator< result
    CORE_API bool CompareUTF8(std::string_view str1, std::string_view str2) noexcept;
    CORE_API bool CompareUTF16(std::u16string_view str1, std::u16string_view str2) noexcept;
    CORE_API bool CompareUTF32(std::u32string_view str1, std::u32string_view str2) noexcept;

    CORE_API bool IsUnicodeC0ControlCode(char value) noexcept; //with Delete
    CORE_API bool IsUnicodeC0ControlCodeOrSpace(char value) noexcept;
};