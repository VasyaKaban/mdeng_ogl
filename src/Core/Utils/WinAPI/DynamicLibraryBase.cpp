#include "DynamicLibraryBase.h"
#include <cassert>

namespace Core
{
    DynamicLibraryBase::DynamicLibraryBase() noexcept
        : handle(nullptr)
    {}

    DynamicLibraryBase::~DynamicLibraryBase()
    {
        Destroy();
    }

    DynamicLibraryBase::DynamicLibraryBase(DynamicLibraryBase&& lib) noexcept
        : handle(std::exchange(lib.handle, nullptr))
    {}

    DynamicLibraryBase& DynamicLibraryBase::operator=(DynamicLibraryBase&& lib) noexcept
    {
        Destroy();

        handle = std::exchange(lib.handle, nullptr);

        return *this;
    }

    std::optional<std::runtime_error> DynamicLibraryBase::Open(const std::filesystem::path& path)
    {
        if(IsOpen())
            Close();

        auto native = path.native();

        handle = LoadLibraryW(native.c_str());
        if(handle != nullptr)
            return std::nullopt;

        return SystemBase::GetLastError();
    }

    bool DynamicLibraryBase::IsOpen() const noexcept
    {
        return handle != nullptr;
    }

    void DynamicLibraryBase::Close() noexcept
    {
        Destroy();
        handle = nullptr;
    }

    DynamicLibraryBase::VoidPFN DynamicLibraryBase::GetProcAddress(const char* name) const noexcept
    {
        assert(handle != nullptr);

        return reinterpret_cast<VoidPFN>(::GetProcAddress(handle, name));
    }

    void DynamicLibraryBase::Destroy() noexcept
    {
        if(handle)
            FreeLibrary(handle);
    }
};