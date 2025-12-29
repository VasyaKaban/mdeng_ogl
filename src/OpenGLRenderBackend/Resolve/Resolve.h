#pragma once

#include "Core/Render/Resolve.h"

#ifdef _WIN32
#    include "../WGL/ResolveBase.h"
#else
#    error "Only WIN32 is supported"
#endif

namespace OpenGL
{
    class Resolve : public Render::Resolve, private ResolveBase
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