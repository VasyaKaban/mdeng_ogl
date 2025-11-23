#pragma once

#include <memory>
#include "../Render.h"
#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class Resolve : public Render::Resolve
    {
    public:
        Resolve() = default;
        virtual ~Resolve() override;

        virtual std::span<const RenderBackendType> GetAvailableBackends() override;
        virtual void Init(RenderBackend* backend) override;
        virtual std::span<const Render::ContextProperties> GetAvailableContexts() override;
        virtual Render::Context* CreateContext(const Render::SelectedContextDesc& desc) override;
    private:
        std::unique_ptr<Context> ctx;
    };
};