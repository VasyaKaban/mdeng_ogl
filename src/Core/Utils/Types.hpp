#pragma once

#include <cfloat>
#include <cstdint>
#include <climits>

inline namespace CoreTypes
{
    using Int8 = int8_t;
    using Int16 = int16_t;
    using Int32 = int32_t;
    using Int64 = int64_t;

    using UInt8 = uint8_t;
    using UInt16 = uint16_t;
    using UInt32 = uint32_t;
    using UInt64 = uint64_t;

    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);

    using Float32 = float;
    using Float64 = double;

    using UTF8Char = char8_t;
    using UTF16Char = char16_t;
    using UTF32Char = char32_t;

    static_assert(sizeof(char8_t) == 1);
    static_assert(sizeof(char16_t) == 2);
    static_assert(sizeof(char32_t) == 4);

    using Char = char;
    using WideChar = wchar_t;

    static_assert(sizeof(bool) == 1);
    using Bool = bool;
    using Bool32 = Int32;

#if defined(_WIN64) || defined(__LP64__)
    using DeviceSize = UInt64;
#else
#    error "Only 64-bit arch is supported"
#endif

    using Void = void;

    constexpr Int8 operator""_i8(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr Int16 operator""_i16(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr Int32 operator""_i32(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr Int64 operator""_i64(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr UInt8 operator""_ui8(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr UInt16 operator""_ui16(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr UInt32 operator""_ui32(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr UInt64 operator""_ui64(unsigned long long value) noexcept
    {
        return value;
    }

    constexpr Float32 operator""_f32(long double value) noexcept
    {
        return value;
    }

    constexpr Float64 operator""_f64(long double value) noexcept
    {
        return value;
    }

    constexpr Bool operator""_b(unsigned long long value) noexcept
    {
        return value > 0;
    }

    constexpr Bool32 operator""_b32(unsigned long long value) noexcept
    {
        return value > 0;
    }

    constexpr DeviceSize operator""_ds(unsigned long long value) noexcept
    {
        return value;
    }
};

namespace Core
{
    template<typename T>
    struct NumericLimits;

    template<>
    struct NumericLimits<Int8>
    {
        constexpr static Int8 Min = INT8_MIN;
        constexpr static Int8 Max = INT8_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Int8);
    };

    template<>
    struct NumericLimits<Int16>
    {
        constexpr static Int16 Min = INT16_MIN;
        constexpr static Int16 Max = INT16_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Int16);
    };

    template<>
    struct NumericLimits<Int32>
    {
        constexpr static Int32 Min = INT32_MIN;
        constexpr static Int32 Max = INT32_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Int32);
    };

    template<>
    struct NumericLimits<Int64>
    {
        constexpr static Int64 Min = INT64_MIN;
        constexpr static Int64 Max = INT64_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Int64);
    };

    template<>
    struct NumericLimits<UInt8>
    {
        constexpr static UInt8 Min = 0;
        constexpr static UInt8 Max = INT8_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UInt8);
    };

    template<>
    struct NumericLimits<UInt16>
    {
        constexpr static UInt16 Min = 0;
        constexpr static UInt16 Max = INT16_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UInt16);
    };

    template<>
    struct NumericLimits<UInt32>
    {
        constexpr static UInt32 Min = 0;
        constexpr static UInt32 Max = INT32_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UInt32);
    };

    template<>
    struct NumericLimits<UInt64>
    {
        constexpr static UInt64 Min = 0;
        constexpr static UInt64 Max = INT64_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UInt64);
    };

    template<>
    struct NumericLimits<Float32>
    {
        constexpr static Float32 Min = FLT_MIN;
        constexpr static Float32 Max = FLT_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Float32);
    };

    template<>
    struct NumericLimits<Float64>
    {
        constexpr static Float64 Min = DBL_MIN;
        constexpr static Float64 Max = DBL_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Float64);
    };

    template<>
    struct NumericLimits<UTF8Char>
    {
        constexpr static UTF8Char Min = 0;
        constexpr static UTF8Char Max = UINT8_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UTF8Char);
    };

    template<>
    struct NumericLimits<UTF16Char>
    {
        constexpr static UTF16Char Min = 0;
        constexpr static UTF16Char Max = UINT16_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UTF16Char);
    };

    template<>
    struct NumericLimits<UTF32Char>
    {
        constexpr static UTF32Char Min = 0;
        constexpr static UTF32Char Max = UINT32_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(UTF32Char);
    };

    template<>
    struct NumericLimits<Char>
    {
        constexpr static Char Min = CHAR_MIN;
        constexpr static Char Max = CHAR_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(Char);
    };

    template<>
    struct NumericLimits<WideChar>
    {
        constexpr static WideChar Min = WCHAR_MIN;
        constexpr static WideChar Max = WCHAR_MAX;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(WideChar);
    };

    template<>
    struct NumericLimits<Bool>
    {
        constexpr static Bool Min = false;
        constexpr static Bool Max = true;
        constexpr static DeviceSize Bits = CHAR_BIT * sizeof(bool);
    };
};