#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class Resolve : public Render::Resolve, hrs::non_copyable, hrs::non_movable
    {
    public:
        Resolve();

        virtual ~Resolve()
        {}

        virtual void Init() override;

        virtual Render::Backend GetBackend() const noexcept = 0;

        virtual Render::Instance* CreateInstance(const Render::InstanceInfo& info) override;
    };
};