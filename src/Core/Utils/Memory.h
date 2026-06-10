#pragma once

#include "Interface.h"

namespace Core
{
    struct MemoryRequirements
    {
        size_t alignment;
        size_t size;
    };

    class CORE_API Allocator1 : public Interface
    {
    public:
        virtual ~Allocator1() override = 0;

        //Interface
        virtual const void* Cast(ClassID id) const noexcept override;

        virtual void* Allocate(const MemoryRequirements& req) noexcept = 0;
        virtual void Deallocate(void* ptr) noexcept = 0;
    };
    CORE_CLASS_ID(CORE_API_TEMPLATE, CORE_API, Allocator1)

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
        explicit operator bool() const noexcept;

        //Allocator1
        void* Allocate(const MemoryRequirements& req) noexcept;
        void Deallocate(void* ptr) noexcept;
    private:
        InterfacePointer<Allocator1> handle;
    };

    CORE_API Allocator GetGlobalAllocator() noexcept;
};