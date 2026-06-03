#pragma once

#include <concepts>
#include <cassert>
#include "NonCreatable.hpp"

namespace Core
{
    class RC;

    template<typename T>
    class RCPointer;

    class RC : NonCopyable, NonMovable
    {
        template<typename T>
        friend class RCPointer;
    public:
        constexpr RC() noexcept
            : refs(0)
        {}

        ~RC() = default;
    private:
        constexpr void IncrementRefs() noexcept
        {
            refs++;
        }

        constexpr void DecrementRefs() noexcept
        {
            assert(refs != 0);
            refs--;
        }

        constexpr bool IsAlive() const noexcept
        {
            return refs != 0;
        }
    private:
        std::size_t refs;
    };

    template<typename T>
    class RCPointer
    {
        template<typename U>
        friend class RCPointer;
    public:
        constexpr RCPointer() noexcept
            : ptr(nullptr)
        {
            static_assert(std::is_base_of_v<RC, T>);
        }

        constexpr RCPointer(T* p) noexcept
            : ptr(p)
        {
            static_assert(std::is_base_of_v<RC, T>);

            if(ptr)
                ptr->RC::IncrementRefs();
        }

        constexpr ~RCPointer()
        {
            Drop();
        }

        constexpr RCPointer(const RCPointer& p) noexcept
            : ptr(p.ptr)
        {
            if(ptr)
                ptr->rc::IncrementRefs();
        }

        constexpr RCPointer& operator=(const RCPointer& p) noexcept
        {
            Drop();

            ptr = p.ptr;
            if(ptr)
                ptr->rc::IncrementRefs();

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr RCPointer(const RCPointer<U>& p) noexcept
            : ptr(p.ptr)
        {
            if(ptr)
                ptr->rc::IncrementRefs();
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr RCPointer& operator=(const RCPointer<U>& p) noexcept
        {
            Drop();

            ptr = p.ptr;
            if(ptr)
                ptr->rc::IncrementRefs();

            return *this;
        }

        constexpr RCPointer(RCPointer&& p) noexcept
            : ptr(std::exchange(p.ptr, nullptr))
        {}

        constexpr RCPointer& operator=(RCPointer&& p) noexcept
        {
            Drop();

            ptr = std::exchange(p.ptr, nullptr);

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr RCPointer(RCPointer<U>&& p) noexcept
            : ptr(std::exchange(p.ptr, nullptr))
        {}

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr RCPointer& operator=(RCPointer<U>&& p) noexcept
        {
            Drop();

            ptr = std::exchange(p.ptr, nullptr);

            return *this;
        }

        constexpr void Reset(T* new_ptr = nullptr) noexcept
        {
            Drop();

            ptr = new_ptr;
            if(ptr)
                ptr->rc::IncrementRefs();
        }

        constexpr std::size_t GetRefCount() const noexcept
        {
            if(ptr == nullptr)
                return 0;

            return ptr->rc::refs;
        }

        constexpr T* Get() const noexcept
        {
            return ptr;
        }

        constexpr explicit operator bool() const noexcept
        {
            return ptr != nullptr;
        }

        constexpr T& operator*() const noexcept
        {
            return *ptr;
        }

        constexpr T* operator->() const noexcept
        {
            return ptr;
        }
    private:
        constexpr void Drop() noexcept
        {
            if(ptr != nullptr)
            {
                ptr->rc::DecrementRefs();
                if(!ptr->rc::IsAlive())
                    delete ptr;
            }
        }
    private:
        T* ptr;
    };
};