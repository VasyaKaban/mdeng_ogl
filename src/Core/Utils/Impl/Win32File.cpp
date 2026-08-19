#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "Win32File.h"

namespace Core
{
    template<typename T>
    static Span<T> CorrectBufferSize(Span<T> data) noexcept
    {
        static_assert(sizeof(DWORD) >= 4);

        constexpr DeviceSize MaxWin32ImplReadSize = (sizeof(DWORD) == 8 ? NumericLimits<UInt64>::Max : NumericLimits<UInt32>::Max);

        if(data.GetSize() > MaxWin32ImplReadSize)
            data = Span(data.GetIterator(), data.GetIterator() + MaxWin32ImplReadSize);

        return data;
    }

    Win32File::Win32File() noexcept
        : handle(INVALID_HANDLE_VALUE)
    {}

    Win32File::~Win32File()
    {
        if(IsOpen())
            CloseHandle(this->handle);
    }

    Win32File::Win32File(Win32File&& file) noexcept
        : handle(Exchange(file.handle, INVALID_HANDLE_VALUE))
    {}

    Win32File& Win32File::operator=(Win32File&& file) noexcept
    {
        this->~Win32File();

        this->handle = Exchange(file.handle, INVALID_HANDLE_VALUE);

        return *this;
    }

    Answer<SystemException> Win32File::Open(PathView path, FileOpenFlags flags)
    {
        if(IsOpen())
            Close();

        auto seq = System::GetThreadLocalWideCharBuffer();
        if(path.IsAbsolute())
        {
            System::TranslateAbsolutePathToWin32Path(path, *seq);
        }
        else
        {
            Path abs_path(path.GetSize() + System::GetExecutablePath().GetSize());
            abs_path /= System::GetExecutablePath();
            abs_path /= path;

            System::TranslateAbsolutePathToWin32Path(abs_path, *seq);
        }

        DWORD access = 0;
        if(flags & FileOpenFlagBits::Read)
            access |= GENERIC_READ;

        if(flags & FileOpenFlagBits::Write)
            access |= GENERIC_WRITE;

        if(flags & FileOpenFlagBits::Append)
        {
            access |= GENERIC_WRITE;
            access |= FILE_APPEND_DATA;
        }

        DWORD create_disposition = (flags & FileOpenFlagBits::Create ? CREATE_NEW : OPEN_EXISTING);
        DWORD share_mode = FILE_SHARE_READ;
        DWORD flags_and_attributes = FILE_ATTRIBUTE_NORMAL;

        this->handle = CreateFileW(seq->GetData(), access, share_mode, nullptr, create_disposition, flags_and_attributes, nullptr);
        if(this->handle == INVALID_HANDLE_VALUE)
        {
            auto error = GetLastError();
            if((flags & FileOpenFlagBits::Create) && error == ERROR_FILE_EXISTS) //it is OK -> call without CREATE_NEW flag
            {
                create_disposition = OPEN_EXISTING;
                this->handle = CreateFileW(seq->GetData(), access, share_mode, nullptr, create_disposition, flags_and_attributes, nullptr);
            }

            if(this->handle == INVALID_HANDLE_VALUE)
                return Win32Exception(GetLastError());
        }

        return {};
    }

    Void Win32File::Close() noexcept
    {
        if(IsOpen())
        {
            CloseHandle(this->handle);
            this->handle = INVALID_HANDLE_VALUE;
        }
    }

    Bool Win32File::IsOpen() const noexcept
    {
        return this->handle != INVALID_HANDLE_VALUE;
    }

    Expected<Int64, SystemException> Win32File::GetPosition() const noexcept
    {
        assert(IsOpen());

        LARGE_INTEGER distance{.QuadPart = 0};
        LARGE_INTEGER output;

        auto res = SetFilePointerEx(this->handle, distance, &output, FILE_CURRENT);
        if(res == 0)
            return Win32Exception(GetLastError());

        return output.QuadPart;
    }

    Answer<SystemException> Win32File::SetPosition(FilePointerStartingPoint starting_point, Int64 offset) const noexcept
    {
        assert(IsOpen());

        LARGE_INTEGER distance{.QuadPart = offset};

        DWORD move_method;
        switch(starting_point)
        {
            case FilePointerStartingPoint::Begin:
                move_method = FILE_BEGIN;
                break;
            case FilePointerStartingPoint::End:
                move_method = FILE_END;
                break;
            case FilePointerStartingPoint::Current:
                move_method = FILE_CURRENT;
                break;
        }

        auto res = SetFilePointerEx(this->handle, distance, nullptr, move_method);
        if(res == 0)
            return Win32Exception(GetLastError());

        return {};
    }

    Expected<UInt64, SystemException> Win32File::Read(Span<UInt8> output) const noexcept
    {
        assert(IsOpen());

        output = CorrectBufferSize(output);

        DWORD read_bytes_count = 0;
        auto res = ReadFile(this->handle, output.GetData(), output.GetSize(), &read_bytes_count, nullptr);
        if(res == FALSE)
        {
            auto error = GetLastError();
            if(error == ERROR_HANDLE_EOF)
                return UInt64(0);

            return Win32Exception(GetLastError());
        }

        return UInt64(read_bytes_count);
    }

    Expected<UInt64, SystemException> Win32File::Write(Span<const UInt8> input) const noexcept
    {
        assert(IsOpen());

        input = CorrectBufferSize(input);

        DWORD written_bytes_count = 0;
        auto res = WriteFile(this->handle, input.GetData(), input.GetSize(), &written_bytes_count, nullptr);
        if(res == FALSE)
            return Win32Exception(GetLastError());

        return UInt64(written_bytes_count);
    }
};

#endif