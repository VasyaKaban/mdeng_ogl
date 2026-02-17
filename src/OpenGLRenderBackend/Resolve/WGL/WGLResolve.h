#pragma once

#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class WGLResolve : public Render::Resolve
    {
    public:
        WGLResolve();

        virtual ~WGLResolve() override;

        virtual void Init() override;

        virtual const Render::InstanceFeatures& GetInstanceFeatures() const noexcept override;
        virtual Render::Backend GetBackend() const noexcept override;
        virtual const std::span<const Render::SurfaceBackend>
        GetAvailableSurfaceBackends() const override;

        virtual Render::Instance* CreateInstance(const Render::InstanceInfo& info) override;
    private:
        Render::InstanceFeatures instance_features;
    };
};