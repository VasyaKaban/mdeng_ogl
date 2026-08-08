#pragma once

#include "../API.h"
#include "Types.hpp"
#include "Memory.h"
#include "Span.hpp"

namespace Core
{
    //We do not implement Allocator1 interface due to the different purposes.
    //If you need to use MemoryPool as Allocator then create wrapper over this class
    class CORE_API MemoryPool
    {
    public:
        struct FreeNode;
        struct AllocatedNode;

        constexpr static DeviceSize MemoryRequirementsMinAlignment = 64;
        constexpr static DeviceSize MemoryRequirementsMinSize = 64; //check!

        MemoryPool(Allocator allocator, DeviceSize size);
        ~MemoryPool();
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&& pool) noexcept;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool& operator=(MemoryPool&& pool) noexcept;

        Void* Allocate(const MemoryRequirements& req) noexcept;
        Void Deallocate(Void* ptr) noexcept;

        DeviceSize GetSize() const noexcept;
    private:
        FreeNode* free_list;
        Allocator allocator;
        Span<UInt8> memory;
    };
};