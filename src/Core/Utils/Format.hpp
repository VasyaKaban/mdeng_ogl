#pragma once

#include "Types.hpp"
#include "Memory.h"
#include "String.h"
#include "CommonExceptions.h"

namespace Core
{
    template<typename T>
    struct Formatter
    {
        Formatter(const T& value);
        DeviceSize GetSize();
        Void Format(UTF8Char* output);
    };

    namespace Detail
    {
        template<typename T, typename... Args>
        String FormatImpl(Allocator allocator, DeviceSize size, const T& arg, const Args&... args)
        {
            Formatter<T> fmt(arg);
            DeviceSize new_size = size + fmt.GetSize();

            if constexpr(sizeof...(args) == 0)
            {
                String str(new_size, allocator);
                fmt.Format(str.GetData() + size);
                return str;
            }
            else
            {
                auto str = FormatImpl(allocator, new_size, args...);
                fmt.Format(str.GetData() + size);
                return str;
            }
        }
    };

    template<typename... Args>
    String Format(Allocator allocator, const Args&... args)
    {
        if constexpr(sizeof...(args) == 0)
            return String(allocator);
        else
            return Detail::FormatImpl(allocator, 0, args...);
    }

    template<typename... Args>
    String Format(const Args&... args)
    {
        return Format(GetGlobalAllocator(), Forward(args)...);
    }

    //for integers
    template<Integral I>
    struct Formatter<I>
    {
        static_assert(NumericLimits<UInt64>::Max == 18'446'744'073'709'551'615ull); //6 * 3 + 2 = 20

        Formatter(const I& value)
        {
            this->length = 0;
            if(value == 0)
                this->length = 1;
            else
            {
                if constexpr(SignedIntegral<I>)
                {
                    if(value < 0)
                        this->length++;
                }

                I value_copy = value;

                while(value_copy != 0)
                {
                    this->length++;
                    value_copy /= 10;
                }
            }
        }

        DeviceSize GetSize()
        {
            return this->length;
        }

        Void Format(UTF8Char* output)
        {
            CopyNonOverlappedMemory(this->data, output, this->length);
        }

        UTF8Char data[20];
        UInt8 length;
    };

    //for char arrays
    template<Character C, DeviceSize N>
    struct Formatter<const C[N]>
    {
        Formatter(const C (&data)[N])
            : data(data),
              size(N - 1)
        {
            auto res = StringEncoder::GetLength(data, size);
            if(res.input_offset != size)
                throw StringEncoderBadCharacterException(this->size, res.input_offset);

            this->output_size = res.output_size;
        }

        DeviceSize GetSize()
        {
            return output_size;
        }

        Void Format(UTF8Char* output)
        {
            StringEncoder::Convert(this->data, this->size, output);
        }

        const C* data;
        DeviceSize size;
        DeviceSize output_size;
    };
};