#include "CommandPool.h"
#include "../CommandBuffer/CommandBuffer.h"
#include "../Device/Device.h"

namespace OpenGL
{
    CommandPool::CommandPool(Device* _parent, const Render::CommandPoolInfo& info) noexcept
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

    Render::Device* CommandPool::GetParent() const noexcept
    {
        return parent;
    }

};