#include "System.h"

namespace Core
{
#ifdef _WIN32
    Win32Exception::Win32Exception(DWORD _code) noexcept
        : code(_code)
    {}

    Win32Exception::~Win32Exception()
    {}

    const char* Win32Exception::what() const noexcept
    {
        if(description.empty())
        {
            if(code == 0)
                description = "Success";
            else
            {
                wchar_t* buffer = nullptr;
                auto size =
                    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                   nullptr,
                                   code,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   reinterpret_cast<LPWSTR>(&buffer),
                                   0,
                                   nullptr);

                if(size == 0)
                    description = "Failed to allocate buffer for error message";

                description = System::WideToUTF8(std::wstring_view(buffer, size));

                LocalFree(buffer);
            }
        }

        return description.c_str();
    }
#endif

    static const std::filesystem::path EXECUTABLE_PATH = []()
    {
#ifdef _WIN32
        wchar_t module_filename[MAX_PATH];
        auto size = GetModuleFileNameW(nullptr, module_filename, MAX_PATH);
        if(size == 0)
            throw GetLastError();

        return std::filesystem::path(module_filename, module_filename + size);
#elif defined(linux)
        char exe_path[PATH_MAX];
        auto exe_path_length = readlink("/proc/self/exe", exe_path, PATH_MAX);
        if(exe_path_length == -1)
            throw std::runtime_error(
                std::format("Failed to retrieve executable path. {}", strerror(errno)));

        return std::filesystem::path(exe_path, exe_path + exe_path_length);
#endif
    }();

    const std::filesystem::path& System::GetExecutablePath()
    {
        return EXECUTABLE_PATH;
    }

    std::string System::DecorateDynamicLibraryName(std::string_view name)
    {
#ifdef _WIN32
        return std::format("{}.dll", name);
#elif defined(linux)
        return std::format("lib{}.so", name);
#endif
    }

#ifdef _WIN32
    DWORD System::GetLastError() noexcept
    {
        return ::GetLastError();
    }

    void System::SetLastError(DWORD code) noexcept
    {
        ::SetLastError(code);
    }

    [[noreturn]] void System::ThrowLastError()
    {
        throw Win32Exception(System::GetLastError());
    }

    std::string System::WideToUTF8(std::wstring_view wstr)
    {
        if(wstr.empty())
            return {};

        auto req_size =
            WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), nullptr, 0, nullptr, nullptr);
        if(req_size == 0)
            throw GetLastError();

        std::string str(req_size, '\0');
        auto res = WideCharToMultiByte(CP_UTF8,
                                       0,
                                       wstr.data(),
                                       wstr.size(),
                                       str.data(),
                                       req_size,
                                       nullptr,
                                       nullptr);

        if(res == 0)
            throw GetLastError();

        return str;
    }

    std::wstring System::UTF8ToWide(std::string_view str)
    {
        auto req_size = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), nullptr, 0);
        if(req_size == 0)
            throw GetLastError();

        std::wstring wstr(req_size, L'\0');
        auto res =
            MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), wstr.data(), wstr.size());
        if(res == 0)
            throw GetLastError();

        return wstr;
    }
#endif
};