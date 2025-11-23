#include "CommandPool.h"
#include "../CommandBuffer/CommandBuffer.h"
#include "../../Context/Context.h"

namespace OpenGL
{
    CommandPool::CommandPool(Context* _parent, const Render::CommandPoolInfo& info) noexcept
        : parent(_parent)
    {}

    CommandPool::~CommandPool()
    {
        //noop
    }

    Render::CommandBuffer* CommandPool::Allocate()
    {
        return new CommandBuffer(parent, this);
    }

    Render::Context* CommandPool::GetContext() const noexcept
    {
        return parent;
    }

};