#pragma once

#include "../Render.h"

namespace Render
{
    class Object
    {
    public:
        virtual Render::Context* GetContext() const noexcept = 0;
    };
};