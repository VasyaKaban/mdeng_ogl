#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Framebuffer
    {
    public:
        virtual ~Framebuffer() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};