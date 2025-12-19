#pragma once

#include "../Render.h"

namespace Render
{
    class Framebuffer
    {
    public:
        virtual ~Framebuffer() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};