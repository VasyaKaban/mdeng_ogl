#pragma once

#include <cassert>
#include "RangeTraits.hpp"
#include "Utility.hpp"

namespace Core
{
    namespace Detail
    {
        template<typename T, typename C>
        requires MoveConstructible<T> && MoveAssignable<T> && InvocableWithResult<C, Bool, const T&, const T&>
        constexpr Void PushInHeap(T* begin, T* inserted, C&& comparator) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T> && NoexceptInvocableWithResult<C, Bool, const T&, const T&>)
        {
            DeviceSize index = inserted - begin;
            while(true)
            {
                DeviceSize parent_index = index / 2;
                if(Forward(comparator(begin[index], begin[parent_index]))) //swap
                {
                    Swap(begin[index], begin[parent_index]);
                }

                if(parent_index == 0)
                    break;

                index = parent_index;
            }
        }

        template<typename T, typename C>
        requires MoveConstructible<T> && MoveAssignable<T> && InvocableWithResult<C, Bool, const T&, const T&>
        constexpr Void MakeHeap(T* begin, T* end, C&& comparator) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T> && NoexceptInvocableWithResult<C, Bool, const T&, const T&>)
        {
            T* next = begin + 1;
            while(next != end)
            {
                PushInHeap(begin, next, Forward(comparator));
                next++;
            }
        }
    };

    /*
    1. make heap
    2. each iteration while(begin != end):
        begin++
        make heap
    */

    template<typename T, typename C>
    requires MoveConstructible<T> && MoveAssignable<T> && InvocableWithResult<C, Bool, const T&, const T&>
    constexpr Void Sort(T* begin, T* end, C&& comparator) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T> && NoexceptInvocableWithResult<C, Bool, const T&, const T&>)
    {
        assert(end > begin);

        while(begin != end)
        {
            Detail::MakeHeap(begin, end, Forward(comparator));

            begin++;
        }
    }

    template<typename T, typename C>
    requires MoveConstructible<T> && MoveAssignable<T> && InvocableWithResult<C, Bool, const T&, const T&>
    constexpr Void Sort(T* begin, DeviceSize size, C&& comparator) noexcept(NoexceptMoveConstructible<T> && NoexceptMoveAssignable<T> && NoexceptInvocableWithResult<C, Bool, const T&, const T&>)
    {
        Sort(begin, begin + size, Forward(comparator));
    }
};