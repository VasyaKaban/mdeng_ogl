#pragma once

#include "../Render.h"

namespace Render
{
    class Shader
    {
    public:
        virtual ~Shader()
        {}

        virtual Context* GetContext() const noexcept = 0;
    };
};