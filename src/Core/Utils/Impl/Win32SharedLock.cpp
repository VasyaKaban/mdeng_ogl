#ifdef _WIN32

#    include "Win32SharedLock.h"

namespace Core
{
    Win32SharedLock::Win32SharedLock()
        : handle(SRWLOCK_INIT)
    {}

    Win32SharedLock::~Win32SharedLock()
    {}

    Void Win32SharedLock::AcquireExclusive()
    {
        AcquireSRWLockExclusive(&this->handle);
    }

    Bool Win32SharedLock::TryAcquireExclusive()
    {
        return TryAcquireSRWLockExclusive(&this->handle) != 0;
    }

    Void Win32SharedLock::ReleaseExclusive()
    {
        ReleaseSRWLockExclusive(&this->handle);
    }

    Void Win32SharedLock::AcquireShared()
    {
        AcquireSRWLockShared(&this->handle);
    }

    Bool Win32SharedLock::TryAcquireShared()
    {
        return TryAcquireSRWLockShared(&this->handle) != 0;
    }

    Void Win32SharedLock::ReleaseShared()
    {
        ReleaseSRWLockShared(&this->handle);
    }
};

#endif