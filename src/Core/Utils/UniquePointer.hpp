#pragma once

#include "Traits.hpp"

namespace Core
{
    template<typename T>
    struct UniquePointerDefaultDeleter
    {
        constexpr Void operator()(T* ptr) noexcept
        {
            ptr->~T();
            RuntimeDeallocateMemory(ptr);
        }
    };

    template<typename D, typename T>
    concept DeleterFor = requires(D& deleter, T* ptr) { deleter(ptr); };

    template<typename T, typename D = UniquePointerDefaultDeleter<T>>
    requires SameAs<DropConstVolatileReference<D>, D> && (!Array<T>)
    class UniquePointer
    {
        static_assert(DeleterFor<D, T>);
    public:
        constexpr UniquePointer() noexcept(NoexceptDefaultConstructible<D>)
        requires DefaultConstructible<D>
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
        constexpr UniquePointer(UniquePointer&& p) noexcept(NoexceptMoveConstructible<D>)
        requires MoveConstructible<D>
            : deleter(Move(p.deleter)),
              ptr(Exchange(p.ptr, nullptr))
        {}

        UniquePointer& operator=(const UniquePointer&) = delete;

        constexpr UniquePointer& operator=(UniquePointer&& p) noexcept(NoexceptMoveAssignable<D>)
        requires MoveAssignable<D>
        {
            Reset();

            this->deleter = Move(p.deleter);
            this->ptr = Exchange(p.ptr, nullptr);

            return *this;
        }

        constexpr UniquePointer(T* ptr) noexcept(NoexceptDefaultConstructible<D>)
        requires DefaultConstructible<D>
            : ptr(ptr),
              deleter()
        {}

        constexpr UniquePointer(T* ptr, const D& deleter) noexcept(NoexceptCopyConstructible<D>)
            : ptr(ptr),
              deleter(deleter)
        {}

        constexpr UniquePointer(T* ptr, D&& deleter) noexcept(NoexceptMoveConstructible<D>)
            : ptr(ptr),
              deleter(Move(deleter))
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

        constexpr Void Reset(T* new_ptr = nullptr)
        {
            if(this->ptr)
                deleter(this->ptr);

            this->ptr = new_ptr;
        }

        constexpr T* Release() noexcept
        {
            return Exchange(this->ptr, nullptr);
        }

        constexpr explicit operator Bool() const noexcept
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
    UniquePointer(T*, D&& deleter) -> UniquePointer<T, DropConstVolatileReference<D>>;
};