#pragma once

#ifdef _WIN32
#    define UNICODE
#    define _UNICODE
#    define _CRT_SECURE_NO_WARNINGS
#    define NOMINMAX
#    include <Windows.h>
#    include <ShellScalingApi.h>

#    undef CreateWindow
#    undef CreateSemaphore
#    undef MemoryBarrier
#    undef GetMessage

#    include <filesystem>
#    include "Core/API.h"
#elif defined(linux)
#    include <dlfcn.h>
#else
#    error "Not supported yet"
#endif

namespace Core
{
#ifdef _WIN32
    class CORE_API Win32Exception : public std::exception
    {
    public:
        Win32Exception(DWORD _code) noexcept;
        virtual ~Win32Exception() override;

        virtual const char* what() const noexcept override;
    protected:
        DWORD code;
        mutable std::string description; //make it mutable for lazy what() method
    };

    class CORE_API DisplayException : public std::exception
    {
    public:
        DisplayException(LONG _code) noexcept;
        virtual ~DisplayException() override;

        virtual const char* what() const noexcept override;
    protected:
        LONG code;
        mutable std::string description; //make it mutable for lazy what() method
    };
#endif

    struct UTF8Result
    {
        char data[4];
        std::uint8_t length;
    };

    class CORE_API System
    {
    public:
        static const std::filesystem::path& GetExecutablePath();

        static std::string DecorateDynamicLibraryName(std::string_view name);

        static UTF8Result UTF32ToUTF8(char32_t utf32) noexcept;

        //high surrogate + low surrogate or code point
        static std::optional<char32_t> UTF16ToUTF32(char16_t utf16[2]) noexcept;

        //alphanumeric comparator -> same as operator< result
        static Bool CompareUTF8(std::string_view str1, std::string_view str2) noexcept;

        static Bool IsUnicodeC0ControlCode(char value) noexcept; //with Delete
        static Bool IsUnicodeC0ControlCodeOrSpace(char value) noexcept;

#ifdef _WIN32
    private:
        friend int WINAPI ::wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show);
        static Void SetCmdShow(int cmd_show) noexcept;
        static Void SetMainThreadID() noexcept;
    public:
        static int GetCmdShow() noexcept;
        static DWORD GetMainThreadID() noexcept;

        static DWORD GetLastError() noexcept;
        static Void SetLastError(DWORD code) noexcept;
        [[noreturn]] static Void ThrowLastError();
        static std::string WideToUTF8(std::wstring_view wstr);
        static std::wstring UTF8ToWide(std::string_view str);

        static LPCWSTR MakeIntAtomW(ATOM atom) noexcept;
#endif
    };
};