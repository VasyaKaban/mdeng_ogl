#pragma once

#include "Render.h"

namespace Render
{
    class CORE_API Resolve
    {
    public:
        virtual ~Resolve() = 0;

        virtual void Init() = 0;

        virtual const InstanceFeatures& GetInstanceFeatures() const noexcept = 0;

        virtual Backend GetBackend() const noexcept = 0;

        virtual Instance* CreateInstance(const InstanceInfo& info) = 0;
    };
};