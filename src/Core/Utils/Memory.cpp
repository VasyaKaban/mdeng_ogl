#include "Memory.h"
#include <new>
#include "Binary.hpp"

namespace Core
{
    Allocator1::~Allocator1()
    {}

    const void* Allocator1::Cast(const UUID& id) const noexcept
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

        virtual void Acquire() noexcept override
        {
            //noop
        }

        virtual void Release() noexcept override
        {
            //noop
        }

        virtual void* Allocate(const MemoryRequirements& req) override
        {
            if(!IsPowerOf2(req.alignment))
                CORE_THROW_EXCEPTION_MOCK("Alignment is not power of two")

            MemoryRequirements new_req(req);
            if(!Align(new_req.size, new_req.alignment))
                CORE_THROW_EXCEPTION_MOCK("Too many memory requested")

            void* ptr = ::operator new(new_req.size, std::align_val_t(new_req.alignment), std::nothrow_t{});
            if(!ptr)
                CORE_THROW_EXCEPTION_MOCK("Bad alloc")

            return ptr;
        }

        virtual void Deallocate(void* ptr) noexcept override
        {
            ::operator delete(ptr);
        }

        virtual bool Grow(void* ptr, size_t size) noexcept override
        {
            return false;
        }

        virtual bool Trim(void* ptr, size_t size) noexcept override
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

    Allocator::operator bool() const noexcept
    {
        return static_cast<bool>(this->handle);
    }

    void* Allocator::Allocate(const MemoryRequirements& req)
    {
        return this->handle->Allocate(req);
    }

    void Allocator::Deallocate(void* ptr) noexcept
    {
        return this->handle->Deallocate(ptr);
    }

    bool Allocator::Grow(void* ptr, size_t size) noexcept
    {
        return this->handle->Grow(ptr, size);
    }

    bool Allocator::Trim(void* ptr, size_t size) noexcept
    {
        return this->handle->Trim(ptr, size);
    }

    static GlobalAllocator GLOBAL_ALLOCATOR;

    Allocator GetGlobalAllocator() noexcept
    {
        return Allocator(&GLOBAL_ALLOCATOR);
    }
};