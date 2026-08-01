#pragma once

#ifdef _WIN32

#    include "Win32System.h"
#    include "../Expected.hpp"
#    include "../Answer.hpp"
#    include "../Span.hpp"
#    include "FileCommon.h"

namespace Core
{
    class CORE_API Win32File
    {
    public:
        Win32File() noexcept;
        ~Win32File();
        Win32File(const Win32File&) = delete;
        Win32File(Win32File&& file) noexcept;
        Win32File& operator=(const Win32File&) = delete;
        Win32File& operator=(Win32File&& file) noexcept;

        Answer<SystemException> Open(PathView path, FileOpenFlags flags);
        Void Close() noexcept;

        Bool IsOpen() const noexcept;

        Expected<Int64, SystemException> GetPosition() const noexcept;
        Answer<SystemException> SetPosition(FilePointerStartingPoint starting_point, Int64 offset) const noexcept;

        Expected<UInt64, SystemException> Read(Span<UInt8> output) const noexcept;
        Expected<UInt64, SystemException> Write(Span<const UInt8> input) const noexcept;
    private:
        HANDLE handle;
    };

    using File = Win32File;
};

#endif