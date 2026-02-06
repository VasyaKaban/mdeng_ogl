#pragma once

#include <span>
#include "../Render.h"

namespace Render
{
    class CORE_API DescriptorSet
    {
    public:
        virtual ~DescriptorSet() = 0;

        virtual void Update(std::span<const WriteDescriptorDesc> writes,
                            std::span<const CopyDescriptorDesc> copies) = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};