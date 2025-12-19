#pragma once

#include "hrs/non_creatable.hpp"
#include "Core/Render/Objects/Surface.h"
#include "../../Render.h"

namespace OpenGL
{
    struct SurfaceConnectInfo
    {
        PhysicalDevice* physical_device;
        std::uint32_t config_index;
        bool validation_layer_enabled;
        bool robust_buffer_access_enabled;
    };

    class Surface : public Render::Surface, hrs::non_copyable, hrs::non_movable
    {
    public:
        Surface(const Render::SurfaceWin32Info& info) noexcept;

        virtual ~Surface() override;

        Render::SurfaceCapabilities GetConnectedCapabilities() const;

        void Connect(const SurfaceConnectInfo& info);

        bool IsConnected() const noexcept;
    private:
        Render::SurfaceWin32Info win32_info;

        HGLRC glrc; //real

        Render::SurfaceCapabilities connected_capabilities;
    };
};