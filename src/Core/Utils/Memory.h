#pragma once

#include <cstddef>
#include "Core/API.h"

namespace Core
{
    struct MemoryRequirements
    {
        size_t alignment;
        size_t size;
    };

    class CORE_API Allocator
    {
    public:
        virtual ~Allocator() = 0;

        virtual void* Allocate(const MemoryRequirements& req) noexcept = 0;
        virtual void Deallocate(void* ptr) noexcept = 0;
    };

    CORE_API Allocator* GetGlobalAllocator() noexcept;

};