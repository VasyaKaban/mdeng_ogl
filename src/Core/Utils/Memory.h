#pragma once

#include "Interface.h"

namespace Core
{
    struct MemoryRequirements
    {
        DeviceSize alignment;
        DeviceSize size;
    };

    CORE_API Void* RuntimeAllocateMemory(const MemoryRequirements& req) noexcept;
    CORE_API Void RuntimeDeallocateMemory(Void* ptr) noexcept;

    class CORE_API Allocator1 : public Interface
    {
    public:
        CORE_INTERFACE_ID("4f256e68-169d-48f0-8229-936d97facf1d")

        virtual ~Allocator1() override = 0;

        //Interface
        virtual const Void* Cast(const UUID& id) const noexcept override;

        virtual Void* Allocate(const MemoryRequirements& req) = 0;
        virtual Void Deallocate(Void* ptr) noexcept = 0;
        virtual Bool Grow(Void* ptr, DeviceSize size /*grow allocation to the 'size' bytes. without realloc*/) noexcept = 0;
        virtual Bool Trim(Void* ptr, DeviceSize size /*trim allocation to the 'size' bytes*/) noexcept = 0;
    };

    class CORE_API Allocator
    {
    public:
        Allocator() noexcept;
        Allocator(InterfacePointer<Allocator1> handle) noexcept;
        ~Allocator() = default;
        Allocator(const Allocator&) = default;
        Allocator(Allocator&&) = default;
        Allocator& operator=(const Allocator&) = default;
        Allocator& operator=(Allocator&&) = default;

        InterfacePointer<Allocator1> GetHandle() const noexcept;
        explicit operator Bool() const noexcept;

        //Allocator1
        Void* Allocate(const MemoryRequirements& req);
        Void Deallocate(Void* ptr) noexcept;
        Bool Grow(Void* ptr, DeviceSize size) noexcept;
        Bool Trim(Void* ptr, DeviceSize size) noexcept;
    private:
        InterfacePointer<Allocator1> handle;
    };

    CORE_API Allocator GetGlobalAllocator() noexcept;
};