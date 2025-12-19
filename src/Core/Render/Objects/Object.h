#pragma once

#include "../Render.h"

namespace Render
{
#error ERASE!
    class Object
    {
    public:
        virtual Context* GetContext() const noexcept = 0;
    };
};