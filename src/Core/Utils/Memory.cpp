#include "Memory.h"
#include <new>

namespace Core
{
    Allocator::~Allocator()
    {}

    class GlobalAllocator : public Allocator
    {
    public:
        GlobalAllocator() = default;
        virtual ~GlobalAllocator() = default;

        virtual void* Allocate(const MemoryRequirements& req) noexcept override
        {
            return ::operator new(req.size, std::align_val_t(req.alignment), std::nothrow_t{});
        }

        virtual void Deallocate(void* ptr) noexcept override
        {
            ::operator delete(ptr);
        }
    };

    static GlobalAllocator GLOBAL_ALLOCATOR;

    Allocator* GetGlobalAllocator() noexcept
    {
        return &GLOBAL_ALLOCATOR;
    }
};