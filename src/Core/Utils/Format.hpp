#pragma once

#include "Types.hpp"
#include "Memory.h"
#include "String.h"
#include "StringView.h"

namespace Core
{
    template<typename T>
    struct Formatter;

    template<typename T>
    concept Formattable = requires(T& obj, Formatter<T>& fmt, UTF8Char* output) {
        Formatter<T>(Forward(obj));
        { fmt.GetSize() } -> SameAs<DeviceSize>;
        { fmt.Format(output) } -> SameAs<Void>;
    };

    namespace Detail
    {
        template<typename T, typename... Args>
        String FormatImpl(Allocator& allocator, DeviceSize size, T&& arg, Args&&... args)
        {
            //if we pass actual Formatter<...>(for example in case of Core::Fmt) then we should use it instead of creating Formatter<Formatter<...>>
            using ArgType = DropConstVolatileReference<T>;
            Conditional<TypeInstantiation<ArgType, Formatter>, ArgType, Formatter<ArgType>> fmt(Forward(arg));
            DeviceSize new_size = size + fmt.GetSize();

            if constexpr(sizeof...(args) == 0)
            {
                String str(new_size, allocator);
                fmt.Format(str.GetData() + size);
                return str;
            }
            else
            {
                auto str = FormatImpl(allocator, new_size, Forward(args)...);
                fmt.Format(str.GetData() + size);
                return str;
            }
        }
    };

    template<typename... Args>
    requires(Formattable<DropConstVolatileReference<Args>> && ...)
    String Format(Allocator allocator, Args&&... args)
    {
        if constexpr(sizeof...(args) == 0)
            return String(allocator);
        else
            return Detail::FormatImpl(allocator, 0, Forward(args)...);
    }

    template<typename... Args>
    String Format(Args&&... args)
    {
        return Format(GetGlobalAllocator(), Forward(args)...);
    }

    //explicit formatters

    //strings
    template<Character C, DeviceSize N>
    struct Formatter<const C[N]>
    {
        constexpr static DeviceSize Size = N - 1;

        const C* str;

        Formatter(const C (&str)[N])
            : str(str)
        {}

        DeviceSize GetSize() noexcept
        {
            return StringEncoder::GetLength(this->str, Size).output_size;
        }

        Void Format(UTF8Char* output) noexcept
        {
            StringEncoder::Convert(this->str, Size, output);
        }
    };

    //integers
    template<Integral I>
    struct Formatter<I>
    {
        //18'446'744'073'709'551'615 -> 6 * 3 + 2 = 20 chars
        //-9'223'372'036'854'775'808

        constexpr static DeviceSize BufferSize = 20;

        UTF8Char buffer[BufferSize];
        DeviceSize start;

        Formatter(I value)
        {
            Bool is_negative = (value < 0);

            Int8 char_pos = BufferSize - 1;
            for(; value != 0; value /= 10)
            {
                UTF8Char ch = u8'0' + (value % 10);
                this->buffer[char_pos--] = ch;
            }

            if(is_negative)
                this->buffer[char_pos--] = u8'-';

            this->start = char_pos + 1;
        }

        DeviceSize GetSize() noexcept
        {
            return BufferSize - this->start;
        }

        Void Format(UTF8Char* output) noexcept
        {
            CopyNonOverlappedMemory(buffer + this->start, output, GetSize());
        }
    };

    //integers
    template<Pointer P>
    struct Formatter<P>
    {
        constexpr static DeviceSize BufferSize = sizeof(DeviceSize) * 2 + 2; //0x12 34 56 78 90 AB CD EF

        UTF8Char buffer[BufferSize];
        DeviceSize start;

        Formatter(P ptr)
        {
            DeviceSize int_ptr = reinterpret_cast<DeviceSize>(ptr);

            Int8 char_pos = BufferSize - 1;
            for(; int_ptr != 0; int_ptr /= 16)
            {
                auto digit = int_ptr % 16;

                UTF8Char ch = (digit < 10 ? u8'0' + digit : u8'a' + (digit - 10));
                this->buffer[char_pos--] = ch;
            }

            this->buffer[char_pos--] = u8'x';
            this->buffer[char_pos--] = u8'0';

            this->start = char_pos + 1;
        }

        DeviceSize GetSize() noexcept
        {
            return BufferSize - this->start;
        }

        Void Format(UTF8Char* output) noexcept
        {
            CopyNonOverlappedMemory(buffer + this->start, output, GetSize());
        }
    };

    //StringView
    template<>
    struct Formatter<StringView>
    {
        StringView view;

        Formatter(StringView view)
            : view(view)
        {}

        DeviceSize GetSize() noexcept
        {
            return this->view.GetSize();
        }

        Void Format(UTF8Char* output) noexcept
        {
            CopyNonOverlappedMemory(this->view.GetData(), output, GetSize());
        }
    };

    //String
    template<>
    struct Formatter<String>
    {
        StringView view;

        Formatter(const String& str)
            : view(str)
        {}

        DeviceSize GetSize() noexcept
        {
            return this->view.GetSize();
        }

        Void Format(UTF8Char* output) noexcept
        {
            CopyNonOverlappedMemory(this->view.GetData(), output, GetSize());
        }
    };

    template<typename T>
    requires Formattable<DropConstVolatileReference<T>>
    auto Fmt(T&& obj) noexcept(NoexceptConstructible<Formatter<DropConstVolatileReference<T>>, T>)
    {
        return Formatter<DropConstVolatileReference<T>>(Forward(obj));
    }
};