#include "Memory.h"
#include <new>

namespace Core
{
    Allocator1::~Allocator1()
    {}

    const void* Allocator1::Cast(ClassID id) const noexcept
    {
        if(id == ClassIdentity<Allocator1>::ID)
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

        virtual void* Allocate(const MemoryRequirements& req) noexcept override
        {
            return ::operator new(req.size, std::align_val_t(req.alignment), std::nothrow_t{});
        }

        virtual void Deallocate(void* ptr) noexcept override
        {
            ::operator delete(ptr);
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

    void* Allocator::Allocate(const MemoryRequirements& req) noexcept
    {
        return this->handle->Allocate(req);
    }

    void Allocator::Deallocate(void* ptr) noexcept
    {
        return this->handle->Deallocate(ptr);
    }

    static GlobalAllocator GLOBAL_ALLOCATOR;

    Allocator GetGlobalAllocator() noexcept
    {
        return Allocator(&GLOBAL_ALLOCATOR);
    }
};