#pragma once

#include <span>
#include "../Render.h"
#include "Object.h"

namespace Render
{
    class DescriptorSet : public Object
    {
    public:
        virtual ~DescriptorSet()
        {}

        virtual void Write(std::span<const UpdateDescriptorDesc> descs) = 0;
    };
};