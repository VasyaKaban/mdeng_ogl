#pragma once

#include <span>
#include "../Render.h"
#include "Object.h"

namespace Render
{
    class DescriptorPool : public Object
    {
    public:
        virtual ~DescriptorPool()
        {}

        virtual std::vector<DescriptorSet*> Allocate(const DescriptorSetAllocateInfo& info) = 0;
    };
};