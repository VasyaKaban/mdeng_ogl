#pragma once

#ifdef _WIN32
#    define UNICODE
#    define _UNICODE
#    define _CRT_SECURE_NO_WARNINGS
#    define NOMINMAX
#    include <Windows.h>
#    include <ShellScalingApi.h>
#    include <dbghelp.h>
#    include <ntstatus.h>
#    include <NTSecAPI.h>
#    include <intrin.h>
#    include <winnt.h>

#    undef CreateWindow
#    undef CreateSemaphore
#    undef MemoryBarrier
#    undef GetMessage

#    include "../../API.h"
#    include "../Types.hpp"
#    include "../Path.h"
#    include "../Exception.h"
#    include "../PathView.h"
#    include "../Sequence.hpp"

namespace Core
{
    class CORE_API Win32Exception : public Exception
    {
    public:
        Win32Exception(DWORD error);

        virtual ~Win32Exception() override;

        Win32Exception(const Win32Exception&) = default;
        Win32Exception(Win32Exception&&) = default;
        Win32Exception& operator=(const Win32Exception&) = default;
        Win32Exception& operator=(Win32Exception&&) = default;

        virtual StringView GetMessage() const noexcept override;
    private:
        DWORD error;
        mutable String lazy_message;
    };

    struct Win32SystemInitInfo
    {
        Int32 cmd_show;
        DWORD main_thread_id;
    };

    class CORE_API Win32System
    {
        static Void Init(const Win32SystemInitInfo& info);
        friend int WINAPI ::wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show);
    public:
        static Void GetRandomBytes(UInt8* output, DeviceSize size);
        static const Path& GetExecutablePath();
        static String DecorateDynamicLibraryName(StringView name);
        static String UndecorateSymbol(StringView name);

        //only for absolute paths

        constexpr static DeviceSize AbsolutePathPrefixSize = 4;
        constexpr static WideChar AbsolutePathPrefix[AbsolutePathPrefixSize] = {L'\\', L'\\', L'?', L'\\'};
        constexpr static DeviceSize AbsolutePathImplementationReserve = AbsolutePathPrefixSize + 1; //prefix + null-term

        static Sequence<WideChar>* GetThreadLocalWideCharBuffer() noexcept;

        static Void TranslateAbsolutePathToWin32Path(const PathView& path, Sequence<WideChar>& output); //only for absolute paths
        static Void TranslateFromWin32PathToAbsolutePath(WideChar* input, DeviceSize input_size, Path& output); //changes \ to / and removes prefix
    };

    using System = Win32System;
    using SystemException = Win32Exception;
};

#endif