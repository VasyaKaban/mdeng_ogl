#include "SystemBase.h"

namespace Core
{
    static std::string WideToUTF8(std::wstring_view wstr)
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

    static const std::filesystem::path EXECUTABLE_PATH = []()
    {
        wchar_t module_filename[MAX_PATH];
        auto size = GetModuleFileNameW(nullptr, module_filename, MAX_PATH);
        if(size == 0)
            throw GetLastError();

        return std::filesystem::path(module_filename, module_filename + size);
    }();

    const std::filesystem::path& SystemBase::GetExecutablePath()
    {
        return EXECUTABLE_PATH;
    }

    std::string SystemBase::DecorateDynamicLibraryName(std::string_view name)
    {
        return std::format("{}.dll", name);
    }

    std::runtime_error SystemBase::GetLastError()
    {
        DWORD error = ::GetLastError();
        wchar_t* buffer = nullptr;
        auto size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                   nullptr,
                                   error,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   reinterpret_cast<LPWSTR>(&buffer),
                                   0,
                                   nullptr);

        if(size == 0)
            throw std::runtime_error(std::format("WinAPI error. Code: {}", ::GetLastError()));

        auto message = WideToUTF8(std::wstring_view(buffer, size));

        LocalFree(buffer);

        return std::runtime_error(message);
    }
};