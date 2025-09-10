#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Framebuffer : public Object
    {
    public:
        virtual ~Framebuffer() = 0;
    };

    inline Framebuffer::~Framebuffer()
    {}
};