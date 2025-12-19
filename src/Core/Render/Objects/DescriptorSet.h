#pragma once

#include <span>
#include "../Render.h"

namespace Render
{
    class DescriptorSet
    {
    public:
        virtual ~DescriptorSet()
        {}

        virtual void Write(std::span<const UpdateDescriptorDesc> descs) = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};