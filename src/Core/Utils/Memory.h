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
        constexpr static InterfaceVersion VERSION = 1;

        virtual ~Allocator1() override = 0;

        virtual void* Allocate(const MemoryRequirements& req) noexcept = 0;
        virtual void Deallocate(void* ptr) noexcept = 0;
    };

    class CORE_API Allocator
    {
    public:
        Allocator(InterfacePointer<Allocator1> handle = nullptr) noexcept;
        ~Allocator() = default;
        Allocator(const Allocator&) = default;
        Allocator(Allocator&&) = default;
        Allocator& operator=(const Allocator&) = default;
        Allocator& operator=(Allocator&&) = default;

        //Interface
        InterfaceVersion GetVersion() const noexcept;

        //Allocator1
        void* Allocate(const MemoryRequirements& req) noexcept;
        void Deallocate(void* ptr) noexcept;
    private:
        InterfacePointer<Allocator1> handle;
    };

    CORE_API Allocator GetGlobalAllocator() noexcept;
};