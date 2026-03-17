#pragma once

#include <optional>
#include <stdexcept>
#include <filesystem>
#include "../API.h"
#include "NonCreatable.hpp"
#include "System.h"

namespace Core
{
    class CORE_API DynamicLibrary : Core::NonCopyable
    {
    public:
        using VoidPFN = void (*)();

        DynamicLibrary() noexcept;
        ~DynamicLibrary();
        DynamicLibrary(DynamicLibrary&& lib) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&& lib) noexcept;

        std::optional<std::runtime_error> Open(const std::filesystem::path& path);

        bool IsOpen() const noexcept;
        void Close() noexcept;

        VoidPFN GetProcAddress(const char* name) const noexcept;
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