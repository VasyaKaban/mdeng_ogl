#pragma once

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