#pragma once

#include <concepts>
#include <cassert>
#include "non_creatable.hpp"

namespace hrs
{
    class rc;

    template<typename T>
    class rc_ptr;

    class rc : hrs::non_copyable, hrs::non_movable
    {
        template<typename T>
        friend class rc_ptr;
    public:
        constexpr rc() noexcept
            : refs(0)
        {}

        ~rc() = default;
    private:
        constexpr void inc_refs() noexcept
        {
            refs++;
        }

        constexpr void dec_refs() noexcept
        {
            assert(refs != 0);
            refs--;
        }

        constexpr bool is_alive() const noexcept
        {
            return refs != 0;
        }
    private:
        std::size_t refs;
    };

    template<typename T>
    class rc_ptr
    {
        template<typename U>
        friend class rc_ptr;
    public:
        constexpr rc_ptr() noexcept
        requires std::derived_from<T, rc>
            : ptr(nullptr)
        {}

        constexpr rc_ptr(T* p) noexcept
        requires std::derived_from<T, rc>
            : ptr(p)
        {
            if(ptr)
                ptr->rc::inc_refs();
        }

        constexpr ~rc_ptr()
        {
            drop();
        }

        constexpr rc_ptr(const rc_ptr& p) noexcept
            : ptr(p.ptr)
        {
            if(ptr)
                ptr->rc::inc_refs();
        }

        constexpr rc_ptr& operator=(const rc_ptr& p) noexcept
        {
            drop();
            ptr = p.ptr;
            if(ptr)
                ptr->rc::inc_refs();

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr rc_ptr(const rc_ptr<U>& p) noexcept
            : ptr(p.ptr)
        {
            if(ptr)
                ptr->rc::inc_refs();
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr rc_ptr& operator=(const rc_ptr<U>& p) noexcept
        {
            drop();
            ptr = p.ptr;
            if(ptr)
                ptr->rc::inc_refs();

            return *this;
        }

        constexpr rc_ptr(rc_ptr&& p) noexcept
            : ptr(std::exchange(p.ptr, nullptr))
        {}

        constexpr rc_ptr& operator=(rc_ptr&& p) noexcept
        {
            drop();

            ptr = std::exchange(p.ptr, nullptr);

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr rc_ptr(rc_ptr<U>&& p) noexcept
            : ptr(std::exchange(p.ptr, nullptr))
        {}

        template<typename U>
        requires std::convertible_to<U*, T*>
        constexpr rc_ptr& operator=(rc_ptr<U>&& p) noexcept
        {
            drop();

            ptr = std::exchange(p.ptr, nullptr);

            return *this;
        }

        constexpr void reset(T* new_ptr = nullptr) noexcept
        {
            drop();
            ptr = new_ptr;
            if(ptr)
                ptr->rc::inc_refs();
        }

        constexpr std::size_t get_refs() const noexcept
        {
            if(!*this)
                return 0;

            return ptr->rc::refs;
        }

        constexpr T* get() const noexcept
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
        constexpr void drop() noexcept
        {
            if(*this)
            {
                ptr->rc::dec_refs();
                if(!ptr->rc::is_alive())
                    delete ptr;
            }
        }
    private:
        T* ptr;
    };
};