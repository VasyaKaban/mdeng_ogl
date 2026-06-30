#pragma once

inline namespace CoreTypes
{
    using Int8 = signed char;
    using Int16 = short int;
    using Int32 = int;
    using Int64 = long long int;

    using UInt8 = unsigned char;
    using UInt16 = unsigned short int;
    using UInt32 = unsigned int;
    using UInt64 = unsigned long long int;

    using Float32 = float;
    using Float64 = double;

    using UTF8Char = char8_t;
    using UTF16Char = char16_t;
    using UTF32Char = char32_t;

    using Char = char;
    using WideChar = wchar_t;

    using Bool = bool;
    static_assert(sizeof(Bool) == 1);

    using Bool32 = Int32;

    using DeviceSize = UInt64;

    using Void = void;
};

namespace Core
{
    template<typename T>
    struct NumericLimits;

    template<>
    struct NumericLimits<Int8>
    {
        constexpr static Int8 Min = -128;
        constexpr static Int8 Max = 127;
        constexpr static DeviceSize Bits = 8 * sizeof(Int8);
    };

    template<>
    struct NumericLimits<Int16>
    {
        constexpr static Int16 Min = -32'768;
        constexpr static Int16 Max = 32'767;
        constexpr static DeviceSize Bits = 8 * sizeof(Int16);
    };

    template<>
    struct NumericLimits<Int32>
    {
        constexpr static Int32 Min = -2'147'483'648;
        constexpr static Int32 Max = 2'147'483'647;
        constexpr static DeviceSize Bits = 8 * sizeof(Int32);
    };

    template<>
    struct NumericLimits<Int64>
    {
        constexpr static Int64 Min = -9'223'372'036'854'775'807ll - 1;
        constexpr static Int64 Max = 9'223'372'036'854'775'807ll;
        constexpr static DeviceSize Bits = 8 * sizeof(Int64);
    };

    template<>
    struct NumericLimits<UInt8>
    {
        constexpr static UInt8 Min = 0;
        constexpr static UInt8 Max = 255;
        constexpr static DeviceSize Bits = 8 * sizeof(UInt8);
    };

    template<>
    struct NumericLimits<UInt16>
    {
        constexpr static UInt16 Min = 0;
        constexpr static UInt16 Max = 65'535;
        constexpr static DeviceSize Bits = 8 * sizeof(UInt16);
    };

    template<>
    struct NumericLimits<UInt32>
    {
        constexpr static UInt32 Min = 0;
        constexpr static UInt32 Max = 4'294'967'295;
        constexpr static DeviceSize Bits = 8 * sizeof(UInt32);
    };

    template<>
    struct NumericLimits<UInt64>
    {
        constexpr static UInt64 Min = 0;
        constexpr static UInt64 Max = 18'446'744'073'709'551'615ull;
        constexpr static DeviceSize Bits = 8 * sizeof(UInt64);
    };

    template<>
    struct NumericLimits<Float32>
    {
        constexpr static Float32 Min = 1.175494351e-38f;
        constexpr static Float32 Max = 3.402823466e+38f;
        constexpr static DeviceSize Bits = 8 * sizeof(Float32);
    };

    template<>
    struct NumericLimits<Float64>
    {
        constexpr static Float64 Min = 2.2250738585072014e-308;
        constexpr static Float64 Max = 1.7976931348623158e+308;
        constexpr static DeviceSize Bits = 8 * sizeof(Float64);
    };

    template<>
    struct NumericLimits<UTF8Char>
    {
        constexpr static UTF8Char Min = 0;
        constexpr static UTF8Char Max = 255;
        constexpr static DeviceSize Bits = 8 * sizeof(UTF8Char);
    };

    template<>
    struct NumericLimits<UTF16Char>
    {
        constexpr static UTF16Char Min = 0;
        constexpr static UTF16Char Max = 65'535;
        constexpr static DeviceSize Bits = 8 * sizeof(UTF16Char);
    };

    template<>
    struct NumericLimits<UTF32Char>
    {
        constexpr static UTF32Char Min = 0;
        constexpr static UTF32Char Max = 0x10'ff'ff;
        constexpr static DeviceSize Bits = 8 * sizeof(UTF32Char);
    };

#if ((Char) - 1) < 0
    template<>
    struct NumericLimits<Char>
    {
        constexpr static Char Min = -128;
        constexpr static Char Max = 127;
        constexpr static DeviceSize Bits = 8 * sizeof(Char);
    };
#else
    template<>
    struct NumericLimits<Char>
    {
        constexpr static Char Min = 0;
        constexpr static Char Max = 255;
        constexpr static DeviceSize Bits = 8 * sizeof(Char);
    };
#endif

#ifdef _WIN32
    template<>
    struct NumericLimits<WideChar>
    {
        constexpr static WideChar Min = 0;
        constexpr static WideChar Max = 65'535;
        constexpr static DeviceSize Bits = 8 * sizeof(WideChar);
    };
#elif defined(__linux__)
    template<>
    struct NumericLimits<WideChar>
    {
        constexpr static WideChar Min = 0;
        constexpr static WideChar Max = 0x10'ff'ff;
        constexpr static DeviceSize Bits = 8 * sizeof(WideChar);
    };
#endif

    template<>
    struct NumericLimits<Bool>
    {
        constexpr static Bool Min = false;
        constexpr static Bool Max = true;
        constexpr static DeviceSize Bits = 8 * sizeof(Bool);
    };
};