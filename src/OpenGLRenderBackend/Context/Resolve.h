#pragma once

#include "hrs/non_creatable.hpp"
#include "../Render.h"
#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class Resolve : public Render::Resolve, hrs::non_copyable, hrs::non_movable
    {
    public:
        Resolve();
        virtual ~Resolve() override;

        virtual std::span<const Core::RenderBackendType> GetAvailableBackends() override;
        virtual void Init(Core::RenderBackend* backend) override;
        virtual std::span<const Render::ContextProperties> GetAvailableContexts() override;
        virtual Render::Context* CreateContext(const Render::SelectedContextDesc& desc) override;

        static void operator delete(void* ptr) noexcept;
    private:
        Context* ctx;
    };
};