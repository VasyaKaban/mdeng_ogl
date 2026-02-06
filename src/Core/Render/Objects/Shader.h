#pragma once

#include "../Render.h"

namespace Render
{
    class CORE_API Shader
    {
    public:
        virtual ~Shader() = 0;

        virtual Device* GetParent() const noexcept = 0;
    };
};