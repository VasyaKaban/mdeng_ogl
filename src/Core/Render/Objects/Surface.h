#pragma once

#include "../Render.h"

namespace Render
{
    class Surface
    {
    public:
        virtual ~Surface() {};

        virtual Extent2D GetCurrentExtent() const noexcept = 0;

        virtual Instance* GetParent() const noexcept = 0;
    };
};