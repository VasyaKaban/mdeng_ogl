#pragma once

#include <memory>
#include "../Render.h"
#include "Object.h"

namespace Render
{
    class DescriptorPool : public Object
    {
    public:
        virtual ~DescriptorPool()
        {}

        virtual DescriptorSet* Allocate(const DescriptorSetAllocateInfo& info) = 0;

        std::unique_ptr<DescriptorSet> AllocateUnique(const DescriptorSetAllocateInfo& info);
    };
};