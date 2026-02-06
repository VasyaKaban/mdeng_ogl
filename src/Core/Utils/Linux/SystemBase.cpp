#include "SystemBase.h"
#include <dlfcn.h>

namespace Core
{
    static const std::filesystem::path EXECUTABLE_PATH = []()
    {
        char exe_path[PATH_MAX];
        auto exe_path_length = readlink("/proc/self/exe", exe_path, PATH_MAX);
        if(exe_path_length == -1)
            throw std::runtime_error(
                std::format("Failed to retrieve executable path. {}", strerror(errno)));

        return std::filesystem::path(exe_path, exe_path + exe_path_length);
    }();

    const std::filesystem::path& SystemBase::GetExecutablePath()
    {
        return EXECUTABLE_PATH;
    }

    std::string SystemBase::DecorateDynamicLibraryName(std::string_view name)
    {
        return std::format("lib{}.so", name);
    }
};