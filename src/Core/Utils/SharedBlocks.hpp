#pragma once

#include "Shared.hpp"
#include "Debug.h"
#include "../API.h"
#include "Memory.h"
#include "System.h"
#include "Atomic.hpp"

namespace Core
{
    template<typename Type, typename Represent = Type>
    class NoOpSharedBlock : public SharedBlock<Represent>
    {
    public:
        NoOpSharedBlock() noexcept(NoexceptDefaultConstructible<Type>)
        requires DefaultConstructible<Type>
            : obj()
        {}

        template<typename... Args>
        requires Constructible<Type, Args...>
        NoOpSharedBlock(Args&&... args) noexcept(NoexceptConstructible<Type, Args...>)
            : obj(Forward(args)...)
        {}

        NoOpSharedBlock(const NoOpSharedBlock&) = delete;
        NoOpSharedBlock(NoOpSharedBlock&&) = delete;
        NoOpSharedBlock& operator=(const NoOpSharedBlock&) = delete;
        NoOpSharedBlock& operator=(NoOpSharedBlock&&) = delete;

        virtual ~NoOpSharedBlock() override = default;

        virtual Void AcquireShared() noexcept override
        {}

        virtual Void ReleaseShared() noexcept override
        {}

        virtual Void AcquireWeak() noexcept override
        {}

        virtual Void ReleaseWeak() noexcept override
        {}

        virtual Bool TryAcquireShared() noexcept override
        {
            return true;
        }

        virtual Bool IsSharedless() noexcept override
        {
            return false;
        }

        virtual Represent* GetObject() noexcept override
        {
            static_assert(BaseOf<Represent, Type>);

            return &this->obj;
        }
    private:
        Type obj;
    };

    template<typename Type, typename Represent = Type>
    class AllocatedPlainCounterSharedBlock : public SharedBlock<Represent>
    {
        AllocatedPlainCounterSharedBlock(SharedPointer<Allocator> allocator = GetGlobalAllocator()) noexcept(NoexceptDefaultConstructible<Type>)
        requires DefaultConstructible<Type>
            : obj(),
              allocator(allocator),
              shared_counter(0),
              weak_counter(0)
        {
            new(this->obj) Type;
        }

        template<typename U>
        requires Constructible<Type, U>
        AllocatedPlainCounterSharedBlock(U&& value, SharedPointer<Allocator> allocator = GetGlobalAllocator()) noexcept(NoexceptConstructible<Type, U>)
            : AllocatedPlainCounterSharedBlock(allocator)
        {
            new(this->obj) Type(Forward(value));
        }

        template<typename... Args>
        requires Constructible<Type, Args...>
        AllocatedPlainCounterSharedBlock(SharedPointer<Allocator> allocator, Args&&... args) noexcept(NoexceptConstructible<Type, Args...>)
            : AllocatedPlainCounterSharedBlock(allocator)
        {
            new(this->obj) Type(Forward(args)...);
        }

        virtual ~AllocatedPlainCounterSharedBlock() override = default;
    public:
        static SharedPointer<Represent> Create(SharedPointer<Allocator> allocator = GetGlobalAllocator())
        requires DefaultConstructible<Type>
        {
            Void* ptr = allocator->Allocate(MemoryRequirements{.alignment = alignof(AllocatedPlainCounterSharedBlock), .size = sizeof(AllocatedPlainCounterSharedBlock)});

            try
            {
                return SharedPointer<Represent>(new(ptr) AllocatedPlainCounterSharedBlock(allocator));
            }
            catch(...)
            {
                allocator->Deallocate(ptr);
                throw;
            }
        }

        template<typename U>
        requires Constructible<Type, U>
        static AllocatedPlainCounterSharedBlock* Create(U&& value, SharedPointer<Allocator> allocator = GetGlobalAllocator())
        {
            Void* ptr = allocator->Allocate(MemoryRequirements{.alignment = alignof(AllocatedPlainCounterSharedBlock), .size = sizeof(AllocatedPlainCounterSharedBlock)});

            try
            {
                return SharedPointer<Represent>(new(ptr) AllocatedPlainCounterSharedBlock(Forward(value), allocator));
            }
            catch(...)
            {
                allocator->Deallocate(ptr);
                throw;
            }
        }

        template<typename... Args>
        requires Constructible<Type, Args...>
        static AllocatedPlainCounterSharedBlock* Create(SharedPointer<Allocator> allocator, Args&&... args)
        {
            Void* ptr = allocator->Allocate(MemoryRequirements{.alignment = alignof(AllocatedPlainCounterSharedBlock), .size = sizeof(AllocatedPlainCounterSharedBlock)});

            try
            {
                return SharedPointer<Represent>(new(ptr) AllocatedPlainCounterSharedBlock(allocator, Forward(args)...));
            }
            catch(...)
            {
                allocator->Deallocate(ptr);
                throw;
            }
        }

        AllocatedPlainCounterSharedBlock(const AllocatedPlainCounterSharedBlock&) = delete;
        AllocatedPlainCounterSharedBlock(AllocatedPlainCounterSharedBlock&&) = delete;
        AllocatedPlainCounterSharedBlock& operator=(const AllocatedPlainCounterSharedBlock&) = delete;
        AllocatedPlainCounterSharedBlock& operator=(AllocatedPlainCounterSharedBlock&&) = delete;

        virtual Void AcquireShared() noexcept override
        {
            this->shared_counter++;
        }

        virtual Void ReleaseShared() noexcept override
        {
            CORE_DEBUG_ASSERTION(this->shared_counter != 0)

            this->shared_counter--;

            if(this->shared_counter == 0)
            {
                reinterpret_cast<Type*>(this->obj)->~Type();
                if(this->weak_counter == 0)
                    Destroy();
            }
        }

        virtual Void AcquireWeak() noexcept override
        {
            this->weak_counter++;
        }

        virtual Void ReleaseWeak() noexcept override
        {
            CORE_DEBUG_ASSERTION(this->weak_counter != 0)

            this->weak_counter--;

            if(this->weak_counter == 0 && this->shared_counter == 0)
                Destroy();
        }

        virtual Bool TryAcquireShared() noexcept override
        {
            AcquireShared();

            return true;
        }

        virtual Bool IsSharedless() noexcept override
        {
            return this->shared_counter == 0;
        }

        virtual Represent* GetObject() noexcept override
        {
            static_assert(BaseOf<Represent, Type>);

            CORE_DEBUG_ASSERTION(this->shared_counter != 0)

            return reinterpret_cast<Type*>(this->obj);
        }
    private:
        Void Destroy() noexcept
        {
            SharedPointer<Allocator> moved_allocator = Move(this->allocator);

            this->~AllocatedPlainCounterSharedBlock();
            moved_allocator->Deallocate(this);
        }
    private:
        alignas(alignof(Type)) UInt8 obj[sizeof(Type)];
        SharedPointer<Allocator> allocator;
        DeviceSize shared_counter;
        DeviceSize weak_counter;
    };

    /*
    T obj;
    SharedPointer<Allocator> allocator;
    ...share pad
    DeviceSize shared_counter;
    ...share pad
    DeviceSize weak_counter;
    ...alignment pad
    
    */
    /*struct AtomicCounterSharedBlockDesc
    {
        MemoryRequirements storage_memory_requirements;
        DeviceSize allocator_offset;
        DeviceSize shared_counter_offset;
        DeviceSize weak_counter_offset;
    };

    namespace Detail
    {
        template<typename T>
        AtomicCounterSharedBlockDesc GetAtomicCounterSharedBlockMetrics() noexcept
        {
            DeviceSize share_alignment = Max(System::GetConcurrentShareGranularityAlignment(), sizeof(DeviceSize));

            DeviceSize size = sizeof(T);
            DeviceSize alignment = Max({alignof(T), alignof(SharedPointer<Allocator>), share_alignment});

            //allocator
            Align(size, alignof(SharedPointer<Allocator>));
            DeviceSize allocator_offset = size;
            size += sizeof(SharedPointer<Allocator>);

            //shared counter
            Align(size, share_alignment);
            DeviceSize shared_counter_offset = size;
            size += sizeof(DeviceSize);

            //weak counter
            Align(size, share_alignment);
            DeviceSize weak_counter_offset = size;
            size += sizeof(DeviceSize);

            Align(size, alignment);

            return AtomicCounterSharedBlockDesc{.storage_memory_requirements = MemoryRequirements{.alignment = alignment, .size = size},
                                                .allocator_offset = allocator_offset,
                                                .shared_counter_offset = shared_counter_offset,
                                                .weak_counter_offset = weak_counter_offset};
        }
    };*/

    /*template<typename T>
    class AtomicCounterSharedBlock : public SharedBlock<T>
    {
    public:
        inline static AtomicCounterSharedBlockDesc StorageDesc = Detail::GetAtomicCounterSharedBlockMetrics<T>();
    private:
        T* Getobject() const noexcept
        {
            return reinterpret_cast<T*>(this);
        }

        DeviceSize* GetSharedCounter() noexcept
        {
            return reinterpret_cast<DeviceSize*>((reinterpret_cast<UInt8*>(this) + StorageDesc.shared_counter_offset));
        }

        DeviceSize* GetWeakCounter() noexcept
        {
            return reinterpret_cast<DeviceSize*>((reinterpret_cast<UInt8*>(this) + StorageDesc.weak_counter_offset));
        }
    public:
        AtomicCounterSharedBlock() noexcept(NoexceptDefaultConstructible<T>)
        requires DefaultConstructible<T>
            : obj()
        {
            AtomicStore<MemoryOrder::Relaxed>(*GetSharedCounter(), 0);
            AtomicStore<MemoryOrder::Relaxed>(*GetWeakCounter(), 0);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        AtomicCounterSharedBlock(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
            : obj(Forward(args)...)
        {
            AtomicStore<MemoryOrder::Relaxed>(*GetSharedCounter(), 0);
            AtomicStore<MemoryOrder::Relaxed>(*GetWeakCounter(), 0);
        }

        AtomicCounterSharedBlock(const AtomicCounterSharedBlock&) = delete;
        AtomicCounterSharedBlock(AtomicCounterSharedBlock&&) = delete;
        AtomicCounterSharedBlock& operator=(const AtomicCounterSharedBlock&) = delete;
        AtomicCounterSharedBlock& operator=(AtomicCounterSharedBlock&&) = delete;

        virtual ~AtomicCounterSharedBlock() override = default;

        virtual Void AcquireShared() noexcept override
        {
            AtomicAdd<MemoryOrder::Relaxed>(*GetSharedCounter(), 1);
        }

        virtual Void ReleaseShared() noexcept override
        {
            DeviceSize prev = AtomicSubtract<MemoryOrder::AcquireRelease>(*GetSharedCounter(), 1);

            CORE_DEBUG_ASSERTION(prev != 0)
        }

        virtual Void AcquireWeak() noexcept override
        {
            AtomicAdd<MemoryOrder::Relaxed>(*GetWeakCounter(), 1);
        }

        virtual Void ReleaseWeak() noexcept override
        {
            DeviceSize prev = AtomicSubtract<MemoryOrder::AcquireRelease>(*GetWeakCounter(), 1);

            CORE_DEBUG_ASSERTION(prev != 0)
        }

        virtual Bool TryAcquireShared() noexcept override
        {
            AcquireShared();

            return true;
        }

        virtual Bool IsSharedless() noexcept override
        {
            return AtomicLoad<MemoryOrder::Relaxed>(*GetSharedCounter()) == 0;
        }

        virtual T* GetObject() noexcept override
        {
            return &this->obj;
        }
    private:
        T obj;
    };*/
};

/*
SystemInfo:
    DeviceSize concurrent_share_granularity;

Atomic:
    T obj;
    ...pad to granularity
    DeviceSize shared_counter;
    ...pad to granularity
    DeviceSize weak_counter;

    size = Align(Align(sizeof(T), SystemInfo.concurrent_share_granularity) + sizeof(DeviceSize), SystemInfo.concurrent_share_granularity) + sizeof(DeviceSize); 
    
AtomicCounterSharedBlock<T>:
    const MemoryRequirements = ...;
    const SharedCounterOffset = ...;
    const WeakCounterOffset = ...;

    static Allocated
*/