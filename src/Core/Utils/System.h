#pragma once

#ifdef _WIN32
#    define UNICODE
#    define _UNICODE
#    define _CRT_SECURE_NO_WARNINGS
#    define NOMINMAX
#    include <Windows.h>

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
    class CORE_API System
    {
    public:
        static const std::filesystem::path& GetExecutablePath();

        static std::string DecorateDynamicLibraryName(std::string_view name);

#ifdef _WIN32
        static std::runtime_error GetLastError();
        static std::string WideToUTF8(std::wstring_view wstr);
        static std::wstring UTF8ToWide(std::string_view str);
#endif
    };
};