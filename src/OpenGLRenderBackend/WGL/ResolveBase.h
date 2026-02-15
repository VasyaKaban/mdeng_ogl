#pragma once

#include "Core/Render/Render.h"
#include "hrs/non_creatable.hpp"

namespace OpenGL
{
    class ResolveBase : hrs::non_copyable, hrs::non_movable
    {
    public:
        ResolveBase() = default;
        ~ResolveBase() = default;

        void Init();

        const std::span<const Render::SurfaceBackend> GetAvailableSurfaceBackends() const;
    };
};