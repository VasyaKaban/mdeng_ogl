#include "Surface.h"
#include <stdexcept>
#include "glad/wgl.h"
#include "../PhysicalDevice/PhysicalDevice.h"
#include "../Instance/Instance.h"

namespace OpenGL
{

    Surface::Surface(Instance* _parent, const Render::SurfaceWin32Info& info) noexcept
        : parent(_parent),
          win32_info(info),
          glrc(nullptr),
          connected_physical_device(nullptr)
    {}

    Surface::~Surface()
    {
        if(glrc)
            wglDeleteContext(glrc);
    }

    bool Surface::IsPresentable() const noexcept
    {
        return true; //in OpenGL we always have presentable contexts???
    }

    Render::Instance* Surface::GetParent() const noexcept
    {
        return parent;
    }

    Render::SurfaceCapabilities Surface::GetConnectedCapabilities() const
    {
        return connected_capabilities;
    }

    PhysicalDevice* Surface::GetConnectedPhysicalDevice() const noexcept
    {
        return connected_physical_device;
    }

    void Surface::Connect(const SurfaceConnectInfo& info)
    {
        if(glrc)
            throw std::runtime_error("Failed to create already created WGL context");

        //select pixel format for current DC
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(win32_info.hdc, info.config_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if(SetPixelFormat(win32_info.hdc, info.config_index, &pfd) == FALSE)
            throw hrs::winapi_get_last_error();

        Instance* impl_instace = static_cast<Instance*>(info.physical_device->GetParent());
        bool debug_messenger_enabled = impl_instace->GetEnabledFeatures().validation_layer ||
                                       impl_instace->GetEnabledFeatures().debug_messenger;

        std::vector<int> profile_attributes = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            4,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            5,
            WGL_CONTEXT_FLAGS_ARB,
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
                (debug_messenger_enabled ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) |
                (info.robust_buffer_access_enabled ? WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB : 0),
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB};

        if(!debug_messenger_enabled && GLAD_WGL_ARB_create_context_no_error)
            profile_attributes.push_back(WGL_CONTEXT_OPENGL_NO_ERROR_ARB);

        if(GLAD_WGL_ARB_context_flush_control)
        {
            profile_attributes.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB);
            profile_attributes.push_back(WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
        }

        profile_attributes.push_back(0);

        glrc = glad_wglCreateContextAttribsARB(win32_info.hdc, nullptr, profile_attributes.data());
        if(!glrc)
            throw hrs::winapi_get_last_error();

        wglMakeCurrent(win32_info.hdc, glrc);

        connected_capabilities = static_cast<PhysicalDevice*>(info.physical_device)
                                     ->GetSurfaceCapabilitiesByIndex(info.config_index);

        connected_physical_device = info.physical_device;
    }

    bool Surface::IsConnected() const noexcept
    {
        return glrc != nullptr;
    }

    void Surface::SetSwapInterval(Render::PresentModeFlagBits present_mode)
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

    void Surface::SwapWindow()
    {
        wglSwapLayerBuffers(win32_info.hdc, WGL_SWAP_MAIN_PLANE);
    }

    void Surface::MakeCurrent()
    {
        wglMakeCurrent(win32_info.hdc, glrc);
    }
};