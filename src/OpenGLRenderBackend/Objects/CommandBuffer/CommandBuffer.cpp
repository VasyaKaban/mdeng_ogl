#include "CommandBuffer.h"
#include "../../Context/Context.h"

namespace OpenGL
{
    CommandBuffer::CommandBuffer(Context* _parent, CommandPool* _pool) noexcept
        : parent(_parent),
          pool(_pool)
    {}

    CommandBuffer::~CommandBuffer()
    {
        //noop
    }

    void CommandBuffer::Reset()
    {
        //noop
    }
    void CommandBuffer::Begin()
    {
        //noop
    }
    void CommandBuffer::End()
    {
        //noop
    }

    Render::Context* CommandBuffer::GetContext() const noexcept
    {
        return parent;
    }
};

//Commands:
/*void CommandBuffer::SetMemoryBarrier(MemoryBarrierFlags flags, bool by_region) const noexcept
{
    if(!by_region)
        parent->GetLoader().MemoryBarrier(static_cast<GLbitfield>(flags));
    else
        parent->GetLoader().MemoryBarrierByRegion(static_cast<GLbitfield>(flags));
}*/