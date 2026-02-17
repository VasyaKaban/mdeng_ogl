#include "WGLSurface.h"
#include "glad/wgl.h"
#include "../../PhysicalDevice/PhysicalDevice.h"
#include "../../../WGL.h"

namespace OpenGL
{
    WGLSurface::WGLSurface(Instance* _parent, const Render::Win32SurfaceInfo& _info) noexcept
        : dc(GetDC(_info.window)),
          glrc(nullptr),
          parent(_parent),
          connected_physical_device(nullptr)
    {}

    WGLSurface::~WGLSurface()
    {
        if(glrc)
            wglDeleteContext(glrc);

        ReleaseDC(WindowFromDC(dc), dc);
    }

    Render::Instance* WGLSurface::GetParent() const noexcept
    {
        return parent;
    }

    const Render::SurfaceCapabilities& WGLSurface::GetConnectedCapabilities() const
    {
        return connected_capabilities;
    }

    PhysicalDevice* WGLSurface::GetConnectedPhysicalDevice() const noexcept
    {
        return connected_physical_device;
    }

    void WGLSurface::Connect(const SurfaceConnectInfo& connect_info)
    {
        if(glrc)
            throw std::runtime_error("Failed to create already created WGL context");

        //select pixel format for current DC
        auto descibe_pixelformat_index =
            connect_info.physical_device->GetDescribePixelFormatIndex(connect_info.format);

        PIXELFORMATDESCRIPTOR pfd;
        int res =
            DescribePixelFormat(dc, descibe_pixelformat_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if(res == 0)
            throw Core::System::GetLastError();

        if(SetPixelFormat(dc, descibe_pixelformat_index, &pfd) == FALSE)
            throw Core::System::GetLastError();

        Instance* impl_instace = static_cast<Instance*>(connect_info.physical_device->GetParent());
        bool debug_messenger_enabled = impl_instace->GetEnabledFeatures().validation_layer ||
                                       impl_instace->GetEnabledFeatures().debug_messenger;

        auto glrc_exp =
            CreateContext(dc, debug_messenger_enabled, connect_info.robust_buffer_access_enabled);
        if(!glrc_exp.has_value())
            throw glrc_exp.error();

        glrc = glrc_exp.value();

        wglMakeCurrent(dc, glrc);

        connected_capabilities = Render::SurfaceCapabilities{
            .min_image_count = SURFACE_MIN_IMAGE_COUNT,
            .max_image_count = SURFACE_MAX_IMAGE_COUNT,
            .supported_present_modes = connect_info.physical_device->GetSupportedPresentModes(),
            .supported_formats = {connect_info.format},
            .min_extent = SURFACE_MIN_EXTENT,
            .current_extent = SURFACE_CURRENT_EXTENT,
            .max_extent = SURFACE_MAX_EXTENT,
            .extent_mode = SURFACE_EXTENT_MODE};

        connected_physical_device = connect_info.physical_device;
    }

    bool WGLSurface::IsConnected() const noexcept
    {
        return glrc != nullptr;
    }

    void WGLSurface::SetSwapInterval(Render::PresentModeFlagBits present_mode)
    {
        int interval;
        switch(present_mode)
        {
            case Render::PresentModeFlagBits::FIFO:
                interval = 1;
                break;
            case Render::PresentModeFlagBits::Immediate:
                interval = 0;
                break;
            case Render::PresentModeFlagBits::RelaxedFIFO:
                interval = -1;
                break;
            case Render::PresentModeFlagBits::Mailbox:
                throw std::runtime_error("Mailbox present mode is not supported");
                break;
        }

        if(!GLAD_WGL_EXT_swap_control) //only FIFO
        {
            if(present_mode != Render::PresentModeFlagBits::FIFO)
                throw std::runtime_error("Only FIFO present mode is supported");
        }
        else if(!GLAD_WGL_EXT_swap_control_tear) //Immediate and FIFO
        {
            if(present_mode == Render::PresentModeFlagBits::RelaxedFIFO)
                throw std::runtime_error("RelaxedFIFO present mode is not supported");

            glad_wglSwapIntervalEXT(interval);
        }
        else //all
        {
            glad_wglSwapIntervalEXT(interval);
        }
    }

    void WGLSurface::SwapWindow()
    {
        wglSwapLayerBuffers(dc, WGL_SWAP_MAIN_PLANE);
    }

    void WGLSurface::MakeCurrent()
    {
        wglMakeCurrent(dc, glrc);
    }
};