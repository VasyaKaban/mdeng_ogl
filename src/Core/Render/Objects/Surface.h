#pragma once

#include "../Render.h"

namespace Render
{
    class Surface
    {
    public:
        virtual ~Surface() {};

        virtual Instance* GetParent() const noexcept = 0;
    };
};