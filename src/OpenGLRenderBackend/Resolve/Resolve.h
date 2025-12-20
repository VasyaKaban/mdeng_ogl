#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class Resolve : public Render::Resolve, hrs::non_copyable, hrs::non_movable
    {
    public:
        Resolve();

        virtual ~Resolve() override;

        virtual void Init() override;

        virtual const Render::InstanceFeatures& GetInstanceFeatures() const noexcept override;

        virtual Render::Backend GetBackend() const noexcept override;

        virtual Render::Instance* CreateInstance(const Render::InstanceInfo& info) override;
    private:
        Render::InstanceFeatures instance_features;
    };
};