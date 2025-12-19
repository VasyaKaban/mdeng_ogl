#pragma once

#include "../Render.h"

namespace OpenGL
{
    class Loader
    {
    public:
        virtual ~Loader()
        {}

        virtual const Render::ContextInitProperties& GetInitProperties() const noexcept = 0;

        virtual Context* CreateContext(const Render::SelectedContextDesc& desc) = 0;
    };
};