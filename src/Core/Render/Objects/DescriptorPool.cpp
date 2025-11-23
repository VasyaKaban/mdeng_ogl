#include "DescriptorPool.h"
#include "DescriptorSet.h"

namespace Render
{
    std::unique_ptr<DescriptorSet>
    DescriptorPool::AllocateUnique(const DescriptorSetAllocateInfo& info)
    {
        return std::unique_ptr<DescriptorSet>(Allocate(info));
    }
};