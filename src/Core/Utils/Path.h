#pragma once

#include "Impl/PathCommon.h"
#include "String.h"

namespace Core
{
    class CORE_API Path
    {
    public:
        using Iterator = Detail::PathPartIterator;

        Path(SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(DeviceSize reserve, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        ~Path();
        Path(const Path& path);
        Path(Path&& path) noexcept;
        Path& operator=(const Path& path);
        Path& operator=(Path&& path) noexcept;

        Path(const StringView& str, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path& operator=(StringView str);

        Path(StringView::Iterator begin, StringView::Iterator end, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(Iterator begin, Iterator end, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(const Char* input, DeviceSize input_size, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(const WideChar* input, DeviceSize input_size, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(const UTF8Char* input, DeviceSize input_size, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(const UTF16Char* input, DeviceSize input_size, SharedPointer<Allocator> allocator = GetGlobalAllocator());
        Path(const UTF32Char* input, DeviceSize input_size, SharedPointer<Allocator> allocator = GetGlobalAllocator());

        Path& Append(const Path& path);
        Path& Append(Iterator begin, Iterator end);
        Path& Append(const Char* input, DeviceSize input_size);
        Path& Append(const WideChar* input, DeviceSize input_size);
        Path& Append(const UTF8Char* input, DeviceSize input_size);
        Path& Append(const UTF16Char* input, DeviceSize input_size);
        Path& Append(const UTF32Char* input, DeviceSize input_size);

        Path operator/(const Path& path) const;
        Path& operator/=(const Path& path);

        Path& Back();
        Path operator<<(DeviceSize steps) const;
        Path& operator<<=(DeviceSize steps);

        StringView GetExtension() const noexcept;
        StringView GetFileName() const noexcept;
        Bool IsAbsolute() const noexcept;

        Bool IsEmpty() const noexcept;
        DeviceSize GetSize() const noexcept;
        DeviceSize GetCapacity() const noexcept;
        SharedPointer<Allocator> GetAllocator() const noexcept;
        StringView GetData() const noexcept;

        Void Reserve(DeviceSize reserve);
        Void Clear() noexcept;
        Bool FlushUnusedReserve() noexcept;

        Iterator GetIterator() const noexcept;
        Iterator GetSentinel() const noexcept;

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept;

        //for all (&input)[N] erase last character -> we do not hold null-terminated character
        template<Character C, DeviceSize N>
        Path(const C (&input)[N], SharedPointer<Allocator> allocator = GetGlobalAllocator())
            : Path(input, N - 1, allocator)
        {}

        template<Character C, DeviceSize N>
        Path& operator=(const C (&input)[N]) noexcept
        {
            this->operator=(StringView(input));

            return *this;
        }

        template<Range R>
        requires Constructible<Path::Iterator, RangeIterator<R>>
        Path(R&& rng, SharedPointer<Allocator> allocator = GetGlobalAllocator()) noexcept
            : Path(Path::Iterator(Forward(rng).GetIterator()), Path::Iterator(Forward(rng).GetSentinel()), allocator)
        {}

        template<Range R>
        requires Constructible<Path::Iterator, RangeIterator<R>>
        Path& operator=(R&& rng) noexcept
        {
            *this = Path(this->data.GetAllocator());

            this->Append(Path::Iterator(Forward(rng).GetIterator()), Path::Iterator(Forward(rng).GetSentinel()));

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