#pragma once

#ifdef _WIN32

#    include "Win32System.h"

/*
ThreadProc(Void* data):
    Win32ThreadInfo* info = data
    info->caller(info.func, info.data):
        func(data)
*/

namespace Core
{
    namespace Detail
    {
        struct Win32ThreadInfo
        {
            HANDLE event;
            Void (*caller)(Win32ThreadInfo* info);
            Void* func;
            Void* data;
        };

        template<typename F, typename Arg>
        requires Invocable<F, Arg>
        Void Win32ThreadCaller(Win32ThreadInfo* info)
        {
            DropReference<F>* func_ptr = reinterpret_cast<DropReference<F>>(info->func);
            DropReference<Arg>* data_ptr = reinterpret_cast<DropReference<Arg>>(info->data);

            DropReference<F> func = static_cast<F>(*func_ptr);
            DropReference<Arg> data = static_cast<Arg>(*data_ptr);

            SetEvent(info->event);

            func(data);
        }
    };

    class CORE_API Win32Thread
    {
        Win32Thread(HANDLE handle) noexcept;
    public:
        Win32Thread() noexcept;
        ~Win32Thread();
        Win32Thread(const Win32Thread&) = delete;
        Win32Thread(Win32Thread&& thread) noexcept;
        Win32Thread& operator=(const Win32Thread&) = delete;
        Win32Thread& operator=(Win32Thread&& thread) noexcept;

        Void Wait();

        Void Detach();

        template<typename F, typename Arg>
        requires Invocable<F, Arg> && (!RValueReference<F> && !RValueReference<Arg>)
        static Win32Thread Execute(F&& func, Arg&& arg)
        {
            Detail::Win32ThreadInfo info = {.event = nullptr, .caller = Detail::Win32ThreadCaller<F, Arg>, .func = &func, .data = &arg};

            info.event = CreateEventExW(nullptr, nullptr, 0, 0);
            if(info.event == nullptr)
                throw SystemException(GetLastError());

            return ExecuteImpl(&info);
        }
    private:
        static Win32Thread ExecuteImpl(Detail::Win32ThreadInfo* info);
    private:
        HANDLE handle;
    };

    using Thread = Win32Thread;
};

#endif