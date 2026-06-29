#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API DescriptorSetLayout
    {
    public:
        virtual ~DescriptorSetLayout() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};