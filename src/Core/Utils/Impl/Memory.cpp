#include "../Memory.h"
#include "../Binary.hpp"
#include "../CommonExceptions.h"
#include "../SharedBlocks.hpp"
#include <malloc.h>

namespace Core
{
    Void* RuntimeAllocateMemory(const MemoryRequirements& req) noexcept
    {
#if CORE_COMPILER_CURRENT == CORE_COMPILER_MSVC
        return _aligned_malloc(req.size, req.alignment);
#else
        return aligned_alloc(req.alignment, req.size)
#endif
    }

    Void RuntimeDeallocateMemory(Void* ptr) noexcept
    {
#if CORE_COMPILER_CURRENT == CORE_COMPILER_MSVC
        return _aligned_free(ptr);
#else
        return free(ptr);
#endif
    }

    Allocator::~Allocator()
    {}

    Void* Allocator::Cast(const UUID& id) noexcept
    {
        if(id == CORE_INTERFACE_GET_ID(Allocator))
            return this;

        return this->Interface::Cast(id);
    }

    class GlobalAllocator final : public Allocator
    {
    public:
        GlobalAllocator() = default;
        virtual ~GlobalAllocator() = default;

        virtual Void* Allocate(const MemoryRequirements& req) override
        {
            if(!IsPowerOf2(req.alignment))
                throw AllocationException(req, AllocationExceptionType::BadMemoryRequirements);

            MemoryRequirements new_req(req);
            if(!Align(new_req.size, new_req.alignment))
                throw AllocationException(req, AllocationExceptionType::OutOfMemory);

            Void* ptr = RuntimeAllocateMemory(new_req);
            if(!ptr)
                throw AllocationException(req, AllocationExceptionType::OutOfMemory);

            return ptr;
        }

        virtual Void Deallocate(Void* ptr) noexcept override
        {
            RuntimeDeallocateMemory(ptr);
        }

        virtual Bool Grow(Void* ptr, DeviceSize size) noexcept override
        {
            return false;
        }

        virtual Bool Trim(Void* ptr, DeviceSize size) noexcept override
        {
            return false;
        }
    };

    static NoOpSharedBlock<GlobalAllocator, Allocator> GlobalAllocatorSharedBlockInstance{};

    SharedPointer<Allocator> GetGlobalAllocator() noexcept
    {
        return SharedPointer<Allocator>(&GlobalAllocatorSharedBlockInstance);
    }
};