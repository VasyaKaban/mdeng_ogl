#pragma once

#include "../Render.h"

namespace Render
{
    class Surface
    {
    public:
        virtual ~Surface() {};

        virtual bool IsPresentable() const noexcept = 0;

        virtual Instance* GetParent() const noexcept = 0;
    };
};