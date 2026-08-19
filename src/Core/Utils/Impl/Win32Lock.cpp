#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "Win32Lock.h"

namespace Core
{
    Win32Lock::Win32Lock()
        : handle(SRWLOCK_INIT)
    {}

    Win32Lock::~Win32Lock()
    {}

    Void Win32Lock::Acquire()
    {
        AcquireSRWLockExclusive(&this->handle);
    }

    Bool Win32Lock::TryAcquire()
    {
        return TryAcquireSRWLockExclusive(&this->handle) != 0;
    }

    Void Win32Lock::Release()
    {
        ReleaseSRWLockExclusive(&this->handle);
    }
};

#endif