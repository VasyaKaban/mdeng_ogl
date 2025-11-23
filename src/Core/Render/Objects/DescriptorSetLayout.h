#pragma once

#include <span>
#include "../Render.h"
#include "Object.h"

namespace Render
{
    class DescriptorSetLayout : public Object
    {
    public:
        virtual ~DescriptorSetLayout()
        {}
    };
};