#pragma once

#ifdef _WIN32
#    include "WinAPI/DynamicLibraryBase.h"
#elif defined(linux)
#    include "Linux/DynamicLibraryBase.h"
#else
#    error "Not implemented yet.."
#endif

namespace Core
{
    class CORE_API DynamicLibrary : public DynamicLibraryBase
    {
    public:
        using DynamicLibraryBase::GetProcAddress;

        DynamicLibrary() noexcept;
        ~DynamicLibrary() noexcept;

        template<typename T>
        T* GetProcAddress(const char* name) const noexcept
        {
            return reinterpret_cast<T*>(this->DynamicLibraryBase::GetProcAddress(name));
        }
    };
};