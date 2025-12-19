#pragma once

#include "../Render.h"

namespace Render
{
    class DescriptorPool
    {
    public:
        virtual ~DescriptorPool()
        {}

        virtual DescriptorSet* Allocate(const DescriptorSetAllocateInfo& info) = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};