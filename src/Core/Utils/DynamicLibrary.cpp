#include "DynamicLibrary.h"
#include <cassert>

namespace Core
{
    DynamicLibrary::DynamicLibrary() noexcept
        : handle(nullptr)
    {}

    DynamicLibrary::~DynamicLibrary()
    {
        Destroy();
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary&& lib) noexcept
        : handle(std::exchange(lib.handle, nullptr))
    {}

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& lib) noexcept
    {
        Destroy();

        handle = std::exchange(lib.handle, nullptr);

        return *this;
    }

    std::exception_ptr DynamicLibrary::Open(const std::filesystem::path& path)
    {
        if(IsOpen())
            Close();

        auto native = path.native();

#ifdef _WIN32
        handle = LoadLibraryW(native.c_str());
#elif defined(linux)
        handle = dlopen(native.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif
        if(handle != nullptr)
            return nullptr;

#ifdef _WIN32

        return std::make_exception_ptr(Win32Exception(Core::System::GetLastError()));
#elif defined(linux)
        return std::runtime_error(dlerror());
#endif
    }

    bool DynamicLibrary::IsOpen() const noexcept
    {
        return handle != nullptr;
    }

    void DynamicLibrary::Close() noexcept
    {
        Destroy();
        handle = nullptr;
    }

    DynamicLibrary::PFN_VoidFunction DynamicLibrary::GetProcAddress(const char* name) const noexcept
    {
        assert(handle != nullptr);

#ifdef _WIN32
        return reinterpret_cast<PFN_VoidFunction>(::GetProcAddress(handle, name));
#elif defined(linux)
        return reinterpret_cast<PFN_VoidFunction>(dlsym(handle, name));
#endif
    }

    void DynamicLibrary::Destroy() noexcept
    {
        if(handle)
        {
#ifdef _WIN32
            FreeLibrary(handle);
#elif defiend(linux)
            dlclose(handle);
#endif
        }
    }
};