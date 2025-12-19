#pragma once

#include "../Render.h"

namespace Render
{
    class DescriptorSetLayout
    {
    public:
        virtual ~DescriptorSetLayout()
        {}

        virtual Device* GetParent() const noexcept = 0;
    };
};