#pragma once

#include "../Render.h"

namespace Render
{
    class Object
    {
    public:
        virtual Context* GetContext() const noexcept = 0;
    };
};