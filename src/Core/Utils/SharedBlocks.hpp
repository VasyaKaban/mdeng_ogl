#pragma once

#include "Shared.hpp"
#include "Debug.h"
#include "../API.h"
#include "Memory.h"

namespace Core
{
    template<typename T>
    class NoOpSharedBlock : public SharedBlock<T>
    {
    public:
        NoOpSharedBlock() noexcept(NoexceptDefaultConstructible<T>)
        requires DefaultConstructible<T>
            : obj()
        {}

        template<typename... Args>
        requires Constructible<T, Args...>
        NoOpSharedBlock(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
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

        virtual T* GetObject() noexcept override
        {
            return &this->obj;
        }
    private:
        T obj;
    };

    template<typename T>
    class PlainCounterSharedBlock : public SharedBlock<T>
    {
    public:
        PlainCounterSharedBlock() noexcept(NoexceptDefaultConstructible<T>)
        requires DefaultConstructible<T>
            : obj(),
              shared_counter(0),
              weak_counter(0)
        {}

        template<typename... Args>
        requires Constructible<T, Args...>
        PlainCounterSharedBlock(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
            : obj(Forward(args)...),
              shared_counter(0),
              weak_counter(0)
        {}

        PlainCounterSharedBlock(const PlainCounterSharedBlock&) = delete;
        PlainCounterSharedBlock(PlainCounterSharedBlock&&) = delete;
        PlainCounterSharedBlock& operator=(const PlainCounterSharedBlock&) = delete;
        PlainCounterSharedBlock& operator=(PlainCounterSharedBlock&&) = delete;

        virtual ~PlainCounterSharedBlock() override = default;

        virtual Void AcquireShared() noexcept override
        {
            this->shared_counter++;
        }

        virtual Void ReleaseShared() noexcept override
        {
            CORE_DEBUG_ASSERTION(this->shared_counter != 0)

            this->shared_counter--;
        }

        virtual Void AcquireWeak() noexcept override
        {
            this->weak_counter++;
        }

        virtual Void ReleaseWeak() noexcept override
        {
            CORE_DEBUG_ASSERTION(this->weak_counter != 0)

            this->weak_counter--;
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

        virtual T* GetObject() noexcept override
        {
            CORE_DEBUG_ASSERTION(this->shared_counter != 0)

            return &this->obj;
        }
    private:
        T obj;
        DeviceSize shared_counter;
        DeviceSize weak_counter;
    };
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