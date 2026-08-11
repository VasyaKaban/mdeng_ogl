#ifdef _WIN32

#    include "Win32System.h"

namespace Core
{
    class CORE_API Win32SharedLock
    {
    public:
        Win32SharedLock();
        ~Win32SharedLock();
        Win32SharedLock(const Win32SharedLock&) = delete;
        Win32SharedLock(Win32SharedLock&&) = delete;
        Win32SharedLock& operator=(const Win32SharedLock&) = delete;
        Win32SharedLock& operator=(Win32SharedLock&&) = delete;

        Void AcquireExclusive();
        Bool TryAcquireExclusive();
        Void ReleaseExclusive();

        Void AcquireShared();
        Bool TryAcquireShared();
        Void ReleaseShared();
    private:
        SRWLOCK handle;
    };

    using SharedLock = Win32SharedLock;
};

#endif