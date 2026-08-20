#pragma once

#include "Types.hpp"
#include "Traits.hpp"

namespace Core
{
    template<typename T>
    class SharedBlock
    {
    public:
        virtual ~SharedBlock()
        {}

        virtual Void AcquireShared() noexcept = 0; //use when constructing SharedPointer
        virtual Void ReleaseShared() noexcept = 0; //use when destructing SharedPointer

        virtual Void AcquireWeak() noexcept = 0; //use when constructing WeakPointer
        virtual Void ReleaseWeak() noexcept = 0; //use when destructing WeakPointer

        virtual Bool
        TryAcquireShared() noexcept = 0; //use when constructing SharedPointer from WeakPointer. If 'shared_counter' is already hit 0 then return false, otherwise increment 'shared_counter' and return true
        virtual Bool IsSharedless() noexcept = 0; //use when checking WeakPointer for expired block. It will return true if 'shared_counter' hit 0, and true otherwise

        virtual T* GetObject() noexcept = 0;
    };

    enum class SharedBlockAcquirePolicy : UInt32
    {
        Acquire, //on shared pointer construct explicitly call 'AcquireShared()'
        NoOp //on shared pointer construct don not explicitly call 'AcquireShared()'
    };

    template<typename T>
    class SharedPointer
    {
    public:
        SharedPointer() noexcept
            : block(nullptr),
              obj(nullptr)
        {}

        SharedPointer(SharedBlock<T>* block, SharedBlockAcquirePolicy policy = SharedBlockAcquirePolicy::Acquire) noexcept
            : SharedPointer()
        {
            Reset(block, policy);
        }

        ~SharedPointer()
        {
            if(!IsEmpty())
                this->block->ReleaseShared();
        }

        SharedPointer(const SharedPointer& ptr) noexcept
            : SharedPointer()
        {
            Reset(ptr.block);
        }

        SharedPointer(SharedPointer&& ptr) noexcept
            : block(Exchange(ptr.block, nullptr)),
              obj(Exchange(ptr.obj, nullptr))
        {}

        SharedPointer& operator=(const SharedPointer& ptr) noexcept
        {
            Reset(ptr.block);

            return *this;
        }

        SharedPointer& operator=(SharedPointer&& ptr) noexcept
        {
            this->~SharedPointer();

            this->block = Exchange(ptr.block, nullptr);
            this->obj = Exchange(ptr.obj, nullptr);

            return *this;
        }

        Bool IsEmpty() const noexcept
        {
            return this->block == nullptr;
        }

        T* GetObject() const noexcept
        {
            return this->obj;
        }

        SharedBlock<T>* GetBlock() const noexcept
        {
            return this->block;
        }

        Void Reset(SharedBlock<T>* block, SharedBlockAcquirePolicy policy = SharedBlockAcquirePolicy::Acquire) noexcept
        {
            this->~SharedPointer();

            this->block = block;
            if(this->block != nullptr)
            {
                if(policy == SharedBlockAcquirePolicy::Acquire)
                    this->block->AcquireShared();

                this->obj = this->block->GetObject();
            }
            else
                this->obj = nullptr;
        }

        T& operator*() const noexcept
        {
            return *this->obj;
        }

        T* operator->() const noexcept
        {
            return this->obj;
        }
    private:
        SharedBlock<T>* block;
        T* obj;
    };

    template<typename T>
    class WeakPointer
    {
    public:
        WeakPointer() noexcept
            : block(nullptr)
        {}

        WeakPointer(SharedBlock<T>* block, SharedBlockAcquirePolicy policy = SharedBlockAcquirePolicy::Acquire) noexcept
            : WeakPointer()
        {
            Reset(block, policy);
        }

        WeakPointer(const SharedPointer<T>& ptr) noexcept
            : WeakPointer()
        {
            Reset(ptr.GetBlock());
        }

        ~WeakPointer()
        {
            if(this->block != nullptr)
                this->block->ReleaseWeak();
        }

        WeakPointer(const WeakPointer& ptr) noexcept
            : WeakPointer()
        {
            Reset(ptr.block);
        }

        WeakPointer(WeakPointer&& ptr) noexcept
            : block(Exchange(ptr.block, nullptr))
        {}

        WeakPointer& operator=(const WeakPointer& ptr) noexcept
        {
            Reset(ptr.block);

            return *this;
        }

        WeakPointer& operator=(WeakPointer&& ptr) noexcept
        {
            this->~WeakPointer();

            this->block = Exchange(ptr.block, nullptr);

            return *this;
        }

        Bool IsEmpty() const noexcept
        {
            return this->block == nullptr;
        }

        Bool IsSharedless() const noexcept
        {
            return IsEmpty() || this->block->IsSharedless();
        }

        SharedPointer<T> Share() const noexcept
        {
            if(IsEmpty())
                return SharedPointer<T>{};

            if(this->block->TryAcquireShared())
                return SharedPointer<T>(this->block, SharedBlockAcquirePolicy::NoOp);

            return SharedPointer<T>{};
        }

        Void Reset(SharedBlock<T>* block, SharedBlockAcquirePolicy policy = SharedBlockAcquirePolicy::Acquire) noexcept
        {
            this->~WeakPointer();

            this->block = block;
            if(this->block != nullptr)
            {
                if(policy == SharedBlockAcquirePolicy::Acquire)
                    this->block->AcquireWeak();
            }
        }
    private:
        SharedBlock<T>* block;
    };
};