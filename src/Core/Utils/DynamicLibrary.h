#pragma once

#include <stdexcept>
#include <filesystem>
#include "../API.h"
#include "NonCreatable.hpp"
#include "System.h"

namespace Core
{
    class CORE_API DynamicLibrary
    {
    public:
        CORE_NON_COPYABLE(DynamicLibrary)

        using PFN_VoidFunction = void (*)();

        DynamicLibrary() noexcept;
        ~DynamicLibrary();
        DynamicLibrary(DynamicLibrary&& lib) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&& lib) noexcept;

        std::exception_ptr Open(const std::filesystem::path& path);

        bool IsOpen() const noexcept;
        void Close() noexcept;

        PFN_VoidFunction GetProcAddress(const char* name) const noexcept;
    private:
        void Destroy() noexcept;
    private:
#ifdef _WIN32
        HMODULE handle;
#elif defined(linux)
        void* handle;
#endif
    };
};