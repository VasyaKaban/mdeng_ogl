#pragma once

#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <Windows.h>

#undef CreateWindow
#undef CreateSemaphore
#undef MemoryBarrier
#undef GetMessage

#include <filesystem>
#include "Core/API.h"

namespace Core
{
    class CORE_API SystemBase
    {
    public:
        static const std::filesystem::path& GetExecutablePath();

        static std::string DecorateDynamicLibraryName(std::string_view name);

        static std::runtime_error GetLastError();
    };
};