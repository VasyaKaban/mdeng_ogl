#pragma once

#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "Win32System.h"

namespace Core
{
    class CORE_API Win32Lock
    {
    public:
        Win32Lock();
        ~Win32Lock();
        Win32Lock(const Win32Lock&) = delete;
        Win32Lock(Win32Lock&&) = delete;
        Win32Lock& operator=(const Win32Lock&) = delete;
        Win32Lock& operator=(Win32Lock&&) = delete;

        Void Acquire();
        Bool TryAcquire();
        Void Release();
    private:
        SRWLOCK handle;
    };

    using Lock = Win32Lock;
};

#endif