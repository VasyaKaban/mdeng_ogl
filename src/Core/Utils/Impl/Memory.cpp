#include "../Memory.h"
#include "../Binary.hpp"
#include "../CommonExceptions.h"
#include <malloc.h>

namespace Core
{
    Void* RuntimeAllocateMemory(const MemoryRequirements& req) noexcept
    {
#ifdef _MSC_VER
        return _aligned_malloc(req.size, req.alignment);
#else
        return aligned_alloc(req.alignment, req.size)
#endif
    }

    Void RuntimeDeallocateMemory(Void* ptr) noexcept
    {
#ifdef _MSC_VER
        return _aligned_free(ptr);
#else
        return free(ptr);
#endif
    }

    Allocator1::~Allocator1()
    {}

    const Void* Allocator1::Cast(const UUID& id) const noexcept
    {
        if(id == CORE_INTERFACE_GET_ID(Allocator1))
            return this;

        return this->Interface::Cast(id);
    }

    class GlobalAllocator final : public Allocator1
    {
    public:
        GlobalAllocator() = default;
        virtual ~GlobalAllocator() = default;

        virtual Void Acquire() noexcept override
        {
            //noop
        }

        virtual Void Release() noexcept override
        {
            //noop
        }

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

    Allocator::Allocator() noexcept
        : handle()
    {}
    Allocator::Allocator(InterfacePointer<Allocator1> handle) noexcept
        : handle(handle)
    {}

    InterfacePointer<Allocator1> Allocator::GetHandle() const noexcept
    {
        return this->handle;
    }

    Allocator::operator Bool() const noexcept
    {
        return static_cast<Bool>(this->handle);
    }

    Void* Allocator::Allocate(const MemoryRequirements& req)
    {
        return this->handle->Allocate(req);
    }

    Void Allocator::Deallocate(Void* ptr) noexcept
    {
        return this->handle->Deallocate(ptr);
    }

    Bool Allocator::Grow(Void* ptr, DeviceSize size) noexcept
    {
        return this->handle->Grow(ptr, size);
    }

    Bool Allocator::Trim(Void* ptr, DeviceSize size) noexcept
    {
        return this->handle->Trim(ptr, size);
    }

    static GlobalAllocator GlobalAllocatorObject;
    Allocator GetGlobalAllocator() noexcept
    {
        return Allocator(&GlobalAllocatorObject);
    }
};