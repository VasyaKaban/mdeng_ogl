#pragma once

#include "../Render.h"

namespace Render
{
    class Shader
    {
    public:
        virtual ~Shader() {};

        virtual Device* GetParent() const noexcept = 0;
    };
};