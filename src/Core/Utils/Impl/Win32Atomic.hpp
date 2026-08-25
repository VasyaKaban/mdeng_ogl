#pragma once

#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "../Types.hpp"
#    include "../Traits.hpp"
#    include "AtomicCommon.h"
#    include "Win32System.h"

namespace Core
{
#    pragma message("Use MemoryOrder on ARM64 or Itanium. Or just emit error on non-X86 arch")

    //Releaxed -> NoFence
    //Acquire -> MakeVisible + Acquire
    //Release -> MakeAvailable + Release
    //ReleaseAcquire -> MakeVisible + MakeAvailable + ReleaseAcquire
    //SequentialConsistency -> FullFence

    template<MemoryOrder Order, Integral I>
    requires(Order == MemoryOrder::Relaxed || Order == MemoryOrder::Release || Order == MemoryOrder::SequentialConsistency)
    Void AtomicStore(I& dst, Identity<I> value) noexcept
    {
        if constexpr(Order != MemoryOrder::Relaxed)
            _ReadWriteBarrier();

        if constexpr(sizeof(I) == 8)
        {
            __iso_volatile_store8(reinterpret_cast<__int8*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            __iso_volatile_store16(reinterpret_cast<__int16*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            __iso_volatile_store32(reinterpret_cast<__int32*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            __iso_volatile_store64(reinterpret_cast<__int64*>(&dst), value);
        }
    }

    template<MemoryOrder Order, Integral I>
    requires(Order == MemoryOrder::Relaxed || Order == MemoryOrder::Acquire || Order == MemoryOrder::SequentialConsistency)
    I AtomicLoad(I& src) noexcept
    {
        if constexpr(sizeof(I) == 8)
        {
            return __iso_volatile_load8(reinterpret_cast<__int8*>(&src));
        }
        else if constexpr(sizeof(I) == 16)
        {
            return __iso_volatile_load16(reinterpret_cast<__int16*>(&src));
        }
        else if constexpr(sizeof(I) == 32)
        {
            return __iso_volatile_load32(reinterpret_cast<__int32*>(&src));
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return __iso_volatile_load64(reinterpret_cast<__int64*>(&src));
        }

        if constexpr(Order != MemoryOrder::Relaxed)
            _ReadWriteBarrier();
    }

    template<MemoryOrder Order, Integral I>
    I AtomicAdd(I& dst, Identity<I> value) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            return _InterlockedExchangeAdd8(reinterpret_cast<CHAR*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            return _InterlockedExchangeAdd16(reinterpret_cast<SHORT*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            return _InterlockedExchangeAdd(reinterpret_cast<LONG*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return _InterlockedExchangeAdd64(reinterpret_cast<LONGLONG*>(&dst), value);
        }
    }

    template<MemoryOrder Order, Integral I>
    I AtomicSubtract(I& dst, Identity<I> value) noexcept
    {
        //There is no Subtract in WinAPI??? -> use two's complement negation with add
        return AtomicAdd<Order>(dst, I(0) - value);
    }

    template<MemoryOrder Order, Integral I>
    I AtomicAnd(I& dst, Identity<I> value) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            return _InterlockedAnd8(reinterpret_cast<CHAR*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            return _InterlockedAnd16(reinterpret_cast<SHORT*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            return _InterlockedAnd(reinterpret_cast<LONG*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return _InterlockedAnd64(reinterpret_cast<LONGLONG*>(&dst), value);
        }
    }

    template<MemoryOrder Order, Integral I>
    I AtomicOr(I& dst, Identity<I> value) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            return _InterlockedOr8(reinterpret_cast<CHAR*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            return _InterlockedOr16(reinterpret_cast<SHORT*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            return _InterlockedOr(reinterpret_cast<LONG*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return _InterlockedOr64(reinterpret_cast<LONGLONG*>(&dst), value);
        }
    }

    template<MemoryOrder Order, Integral I>
    I AtomicXor(I& dst, Identity<I> value) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            return _InterlockedXor8(reinterpret_cast<CHAR*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            return _InterlockedXor16(reinterpret_cast<SHORT*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            return _InterlockedXor(reinterpret_cast<LONG*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return _InterlockedXor64(reinterpret_cast<LONGLONG*>(&dst), value);
        }
    }

    template<MemoryOrder Order, Integral I>
    I AtomicExchange(I& dst, Identity<I> value) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            return _InterlockedExchange8(reinterpret_cast<CHAR*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 16)
        {
            return _InterlockedExchange16(reinterpret_cast<SHORT*>(&dst), value);
        }
        else if constexpr(sizeof(I) == 32)
        {
            return _InterlockedExchange(reinterpret_cast<LONG*>(&dst), value);
        }
        else //if constexpr(sizeof(I) == 64)
        {
            return _InterlockedExchange64(reinterpret_cast<LONGLONG*>(&dst), value);
        }
    }

    //dst == expected -> dst = desired
    //dst != expected -> expected = dst
    template<MemoryOrder SuccessOrder, MemoryOrder FailureOrder, Integral I>
    requires(FailureOrder == MemoryOrder::Relaxed || FailureOrder == MemoryOrder::Acquire || FailureOrder == MemoryOrder::SequentialConsistency)
    Bool AtomicCompareExchangeStrong(I& dst, Identity<I>& expected, Identity<I> desired) noexcept
    {
        //Ignore Order -> no fences on X86
        if constexpr(sizeof(I) == 8)
        {
            auto res = _InterlockedCompareExchange8(reinterpret_cast<CHAR*>(&dst), desired, expected);
            if(res == expected)
                return true;

            expected = res;
            return false;
        }
        else if constexpr(sizeof(I) == 16)
        {
            auto res = _InterlockedCompareExchange16(reinterpret_cast<SHORT*>(&dst), desired, expected);
            if(res == expected)
                return true;

            expected = res;
            return false;
        }
        else if constexpr(sizeof(I) == 32)
        {
            auto res = _InterlockedCompareExchange(reinterpret_cast<LONG*>(&dst), desired, expected);
            if(res == expected)
                return true;

            expected = res;
            return false;
        }
        else //if constexpr(sizeof(I) == 64)
        {
            auto res = _InterlockedCompareExchange64(reinterpret_cast<LONGLONG*>(&dst), desired, expected);
            if(res == expected)
                return true;

            expected = res;
            return false;
        }
    }

    template<MemoryOrder SuccessOrder, MemoryOrder FailureOrder, Integral I>
    requires(FailureOrder == MemoryOrder::Relaxed || FailureOrder == MemoryOrder::Acquire || FailureOrder == MemoryOrder::SequentialConsistency)
    Bool AtomicCompareExchangeWeak(I& dst, Identity<I>& expected, Identity<I> desired) noexcept
    {
        return AtomicCompareExchangeStrong<SuccessOrder, FailureOrder>(dst, expected, desired);
    }
};

#endif