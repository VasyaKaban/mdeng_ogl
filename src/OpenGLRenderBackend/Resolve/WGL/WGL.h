#pragma once

#include "hrs/non_creatable.hpp"
#include "../Loader.h"
#include "Core/Render/Resolve.h"

namespace OpenGL
{
    class WGL : public Loader, hrs::non_copyable, hrs::non_movable
    {
    public:
        WGL(const Render::ResolveWin32Info& _info);
        virtual ~WGL() override;

        virtual const Render::ContextInitProperties& GetInitProperties() const noexcept override;

        virtual Context* CreateContext(const Render::SelectedContextDesc& desc) override;
    private:
        Render::ResolveWin32Info info;
        Render::ContextInitProperties ctx_init_properties;
        std::vector<std::uint32_t> pixelformat_indices;

        HGLRC glrc; //real gl rc
    };
};