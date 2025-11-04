#pragma once

#include <filesystem>
#include "../../non_creatable.hpp"
#include "winapi.h"

namespace hrs
{
    class dynamic_library : hrs::non_copyable
    {
    public:
        using void_pfn = void (*)();

        dynamic_library() noexcept;
        ~dynamic_library();
        dynamic_library(dynamic_library&& lib) noexcept;
        dynamic_library& operator=(dynamic_library&& lib) noexcept;

        std::optional<std::runtime_error> open(const std::filesystem::path& path);

        bool is_open() const noexcept;

        void close() noexcept;

        void_pfn get_proc_address(const char* name) const noexcept;

        template<typename T>
        T* get_proc_address(const char* name) const noexcept
        {
            return reinterpret_cast<T*>(get_proc_address(name));
        }
    private:
        void destroy() noexcept;
    private:
        HMODULE handle;
    };

    std::filesystem::path exe_path();

    std::string decorate_shared_library_name(std::string_view name);
};