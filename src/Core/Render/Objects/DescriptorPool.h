#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API DescriptorPool
    {
    public:
        virtual ~DescriptorPool() = 0;

        virtual DescriptorSet* Allocate(const DescriptorSetAllocateInfo& info) = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};