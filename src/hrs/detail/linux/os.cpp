#include "os.h"
#include <format>
#include <cassert>
#include <dlfcn.h>
#include <unistd.h>

namespace hrs
{
    dynamic_library::dynamic_library() noexcept
        : handle(nullptr)
    {}

    dynamic_library::~dynamic_library()
    {
        destroy();
    }

    dynamic_library::dynamic_library(dynamic_library&& lib) noexcept
        : handle(std::exchange(lib.handle, nullptr))
    {}

    dynamic_library& dynamic_library::operator=(dynamic_library&& lib) noexcept
    {
        destroy();

        handle = std::exchange(lib.handle, nullptr);

        return *this;
    }

    std::optional<std::runtime_error> dynamic_library::open(const std::filesystem::path& path)
    {
        if(is_open())
            close();

        auto native = path.native();

        handle = dlopen(native.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if(handle != nullptr)
            return std::nullopt;

        return std::runtime_error(dlerror());
    }

    bool dynamic_library::is_open() const noexcept
    {
        return handle != nullptr;
    }

    void dynamic_library::close() noexcept
    {
        destroy();
        handle = nullptr;
    }

    dynamic_library::void_pfn dynamic_library::get_proc_address(const char* name) const noexcept
    {
        assert(handle != nullptr);

        return reinterpret_cast<void_pfn>(dlsym(handle, name));
    }

    void dynamic_library::destroy() noexcept
    {
        if(handle)
            dlclose(handle);
    }

    std::filesystem::path exe_path()
    {
        char _exe_path[PATH_MAX];
        auto exe_path_length = readlink("/proc/self/exe", _exe_path, PATH_MAX);
        if(exe_path_length == -1)
            throw std::runtime_error(
                std::format("Failed to retrieve executable path. {}", strerror(errno)));

        return std::filesystem::path(_exe_path, _exe_path + exe_path_length);
    }

    std::string decorate_shared_library_name(std::string_view name)
    {
        return std::format("lib{}.so", name);
    }
};