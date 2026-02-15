#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Surface
    {
    public:
        virtual ~Surface() = 0;

        virtual Instance* GetParent() const noexcept = 0;
    };
};