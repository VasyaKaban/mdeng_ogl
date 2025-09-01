#pragma once

#include "../Render.h"

namespace Render
{
    class Framebuffer
    {
    public:
        virtual ~Framebuffer()
        {}

        virtual Context* GetContext() const noexcept = 0;
    };
};