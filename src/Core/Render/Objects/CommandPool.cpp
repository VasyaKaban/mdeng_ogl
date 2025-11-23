#include "CommandPool.h"
#include "CommandBuffer.h"

namespace Render
{
    std::unique_ptr<CommandBuffer> CommandPool::AllocateUnique()
    {
        return std::unique_ptr<CommandBuffer>(Allocate());
    }
};