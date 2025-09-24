#include "CommandPool.h"

namespace Render
{
    std::unique_ptr<CommandBuffer> CommandPool::AllocateUnique()
    {
        return std::unique_ptr<CommandBuffer>(Allocate());
    }

};