#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include "Traits.hpp"

namespace Core
{
    template<typename T>
    struct UniquePointerDefaulDeleter
    {
        constexpr void operator()(T* ptr) noexcept
        {
            ::delete ptr;
        }
    };

    template<typename D, typename T>
    concept DeleterFor = requires(D& deleter, T* ptr) { deleter(ptr); };

    template<typename T, typename D = UniquePointerDefaulDeleter<T>>
    requires std::same_as<std::remove_cvref_t<D>, D> && (!std::is_array_v<T>)
    class UniquePointer
    {
        static_assert(DeleterFor<D, T>);
    public:
        constexpr UniquePointer() noexcept(std::is_nothrow_default_constructible_v<D>)
        requires std::is_default_constructible_v<D>
            : ptr(nullptr),
              deleter()
        {}

        constexpr ~UniquePointer()
        {
            if(this->ptr)
                deleter(this->ptr);
        }

        UniquePointer(const UniquePointer&) = delete;

        //Yes, firstly we need to move deleter and then ptr!
        constexpr UniquePointer(UniquePointer&& p) noexcept(std::is_nothrow_move_constructible_v<D>)
        requires std::move_constructible<D>
            : deleter(std::move(p.deleter)),
              ptr(std::exchange(p.ptr, nullptr))
        {}

        UniquePointer& operator=(const UniquePointer&) = delete;

        constexpr UniquePointer& operator=(UniquePointer&& p) noexcept(std::is_nothrow_move_assignable_v<D>)
        requires std::is_move_assignable_v<D>
        {
            Reset();

            this->deleter = std::move(p.deleter);
            this->ptr = std::exchange(p.ptr, nullptr);

            return *this;
        }

        constexpr UniquePointer(T* ptr) noexcept(std::is_nothrow_default_constructible_v<D>)
        requires std::is_default_constructible_v<D>
            : ptr(ptr),
              deleter()
        {}

        constexpr UniquePointer(T* ptr, const D& deleter) noexcept(std::is_nothrow_copy_constructible_v<D>)
            : ptr(ptr),
              deleter(deleter)
        {}

        constexpr UniquePointer(T* ptr, D&& deleter) noexcept(std::is_nothrow_move_constructible_v<D>)
            : ptr(ptr),
              deleter(std::move(deleter))
        {}

        constexpr T* GetData() noexcept
        {
            return this->ptr;
        }

        constexpr const T* GetData() const noexcept
        {
            return this->ptr;
        }

        constexpr D& GetDeleter() noexcept
        {
            return this->deleter;
        }

        constexpr const D& GetDeleter() const noexcept
        {
            return this->deleter;
        }

        constexpr void Reset(T* new_ptr = nullptr)
        {
            if(this->ptr)
                deleter(this->ptr);

            this->ptr = new_ptr;
        }

        constexpr T* Release() noexcept
        {
            return std::exchange(this->ptr, nullptr);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->ptr != nullptr;
        }
    private:
        T* ptr;
        D deleter;
    };

    template<typename T>
    UniquePointer(T*) -> UniquePointer<T>;

    template<typename T, typename D>
    UniquePointer(T*, D&& deleter) -> UniquePointer<T, std::remove_cvref_t<D>>;
};