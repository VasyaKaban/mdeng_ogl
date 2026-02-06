#pragma once

#include "SystemBase.h"
#include "hrs/non_creatable.hpp"

namespace Core
{
    class CORE_API DynamicLibraryBase : hrs::non_copyable
    {
    public:
        using VoidPFN = void (*)();

        DynamicLibraryBase() noexcept;
        ~DynamicLibraryBase();
        DynamicLibraryBase(DynamicLibraryBase&& lib) noexcept;
        DynamicLibraryBase& operator=(DynamicLibraryBase&& lib) noexcept;

        std::optional<std::runtime_error> Open(const std::filesystem::path& path);

        bool IsOpen() const noexcept;
        void Close() noexcept;

        VoidPFN GetProcAddress(const char* name) const noexcept;
    private:
        void Destroy() noexcept;
    private:
        HMODULE handle;
    };
};