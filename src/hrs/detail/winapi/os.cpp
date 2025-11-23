#include "os.h"
#include <format>
#include <cassert>

namespace hrs
{
    static std::runtime_error get_last_error()
    {
        DWORD error = GetLastError();
        char* buffer = nullptr;
        auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                   nullptr,
                                   error,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   reinterpret_cast<LPSTR>(&buffer),
                                   0,
                                   nullptr);

        std::string message(buffer, size);

        LocalFree(buffer);

        return std::runtime_error(message);
    }

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

        handle = LoadLibrary(native.c_str());
        if(handle != nullptr)
            return std::nullopt;

        return get_last_error();
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

        return reinterpret_cast<void_pfn>(GetProcAddress(handle, name));
    }

    void dynamic_library::destroy() noexcept
    {
        if(handle)
            FreeLibrary(handle);
    }

    std::filesystem::path exe_path()
    {
        TCHAR module_filename[MAX_PATH];
        auto size = GetModuleFileName(nullptr, module_filename, MAX_PATH);
        return std::filesystem::path(module_filename, module_filename + size);
    }

    std::string decorate_shared_library_name(std::string_view name)
    {
        return std::format("{}.dll", name);
    }
};