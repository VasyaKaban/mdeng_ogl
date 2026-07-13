#pragma once

#include "Impl/PathCommon.h"
#include "String.h"

namespace Core
{
    class Path
    {
    public:
        using Iterator = Detail::PathPartIterator;

        Path(Allocator allocator = GetGlobalAllocator());
        Path(DeviceSize reserve, Allocator allocator = GetGlobalAllocator());
        ~Path();
        Path(const Path& path);
        Path(Path&& path) noexcept;
        Path& operator=(const Path& path);
        Path& operator=(Path&& path) noexcept;

        Path(const StringView& str, Allocator allocator = GetGlobalAllocator());
        Path& operator=(StringView str);

        Path(StringView::Iterator begin, StringView::Iterator end, Allocator allocator = GetGlobalAllocator());
        Path(const Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        Path(const WideChar* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        Path(const UTF8Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        Path(const UTF16Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
        Path(const UTF32Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());

        Path& Append(const Path& path);

        Path operator/(const Path& path);
        Path& operator/=(const Path& path);

        Path& Back();
        Path operator<<(DeviceSize steps);
        Path& operator<<=(DeviceSize steps);

        StringView GetExtension() const noexcept;
        StringView GetFileName() const noexcept;
        Bool IsAbsolute() const noexcept;

        Bool IsEmpty() const noexcept;
        DeviceSize GetSize() const noexcept;
        DeviceSize GetCapacity() const noexcept;
        Allocator GetAllocator() const noexcept;
        StringView GetData() const noexcept;

        Void Reserve(DeviceSize reserve);
        Void Clear() noexcept;
        Bool FlushUnusedReserve() noexcept;

        Iterator GetIterator() const noexcept;
        Iterator GetSentinel() const noexcept;

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept;

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<Character C, DeviceSize N>
        Path(const C (&input)[N], Allocator allocator = GetGlobalAllocator())
            : Path(input, N - 1, allocator)
        {}

        template<Character C, DeviceSize N>
        Path& operator=(const C (&input)[N]) noexcept
        {
            this->operator=(StringView(input));

            return *this;
        }
    private:
        String data;
    };

    //std compat
    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, Path>
    auto begin(T&& path) noexcept
    {
        return Forward(path).GetIterator();
    }

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, Path>
    auto end(T&& path) noexcept
    {
        return Forward(path).GetSentinel();
    }
};