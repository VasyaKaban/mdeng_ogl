#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "Win32Thread.h"

namespace Core
{
    static DWORD WINAPI Win32ThreadProc(LPVOID lp_parameter)
    {
        Detail::Win32ThreadInfo* info = static_cast<Detail::Win32ThreadInfo*>(lp_parameter);

        info->caller(info);

        return 0;
    }

    Win32Thread::Win32Thread(HANDLE handle) noexcept
        : handle(handle)
    {}

    Win32Thread::Win32Thread() noexcept
        : handle(nullptr)
    {}

    Win32Thread::~Win32Thread()
    {
        if(this->handle != nullptr)
            CloseHandle(this->handle);
    }

    Win32Thread::Win32Thread(Win32Thread&& thread) noexcept
        : handle(Exchange(thread.handle, nullptr))
    {}

    Win32Thread& Win32Thread::operator=(Win32Thread&& thread) noexcept
    {
        this->~Win32Thread();

        this->handle = Exchange(thread.handle, nullptr);

        return *this;
    }

    Void Win32Thread::Wait()
    {
        if(this->handle == nullptr)
            return;

        auto res = WaitForSingleObject(&this->handle, INFINITE);
        if(res != WAIT_OBJECT_0)
            throw SystemException(GetLastError());
    }

    Void Win32Thread::Detach()
    {
        this->handle = nullptr;
    }

    Win32Thread Win32Thread::ExecuteImpl(Detail::Win32ThreadInfo* info)
    {
#    pragma message("To ScopedCall")

        HANDLE handle = CreateThread(nullptr, 0, Win32ThreadProc, info, 0, nullptr);
        if(handle == nullptr)
        {
            CloseHandle(info->event);
            throw SystemException(GetLastError());
        }

        auto res = WaitForSingleObject(&info->event, INFINITE);
        CloseHandle(info->event);

        if(res != WAIT_OBJECT_0)
            throw SystemException(GetLastError());

        return Win32Thread(handle);
    }

};

#endif