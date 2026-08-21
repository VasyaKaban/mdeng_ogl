#pragma once

#include "Interface.h"
#include "Shared.hpp"

namespace Core
{
    struct MemoryRequirements
    {
        DeviceSize alignment;
        DeviceSize size;
    };

    CORE_API Void* RuntimeAllocateMemory(const MemoryRequirements& req) noexcept;
    CORE_API Void RuntimeDeallocateMemory(Void* ptr) noexcept;

    class CORE_API Allocator : public Interface
    {
    public:
        CORE_INTERFACE_ID("4f256e68-169d-48f0-8229-936d97facf1d")

        virtual ~Allocator() override = 0;

        //Interface
        virtual Void* Cast(const UUID& id) noexcept override;

        virtual Void* Allocate(const MemoryRequirements& req) = 0;
        virtual Void Deallocate(Void* ptr) noexcept = 0;
        virtual Bool Grow(Void* ptr, DeviceSize size /*grow allocation to the 'size' bytes. without realloc*/) noexcept = 0;
        virtual Bool Trim(Void* ptr, DeviceSize size /*trim allocation to the 'size' bytes*/) noexcept = 0;
    };

    CORE_API SharedPointer<Allocator> GetGlobalAllocator() noexcept;
};

//placement new workaround
constexpr Void* operator new(DeviceSize count, Void* ptr) noexcept
{
    return ptr;
}