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

        int profile_attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, //0
            4, //1
            WGL_CONTEXT_MINOR_VERSION_ARB, //2
            5, //3
            WGL_CONTEXT_FLAGS_ARB, //4
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, //5
            WGL_CONTEXT_PROFILE_MASK_ARB, //6
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //7
            /*WGL_CONTEXT_OPENGL_NO_ERROR_ARB*/
            0, //8 -> "WGL_ARB_create_context_no_error"#enable when WGL_CONTEXT_DEBUG_BIT_ARB(validation_layer) is not set
            0 //9
        };

        //"WGL_ARB_create_context_no_error"#enable when WGL_CONTEXT_DEBUG_BIT_ARB(validation_layer) is not set
        //"WGL_ARB_create_context_robustness"#enable when robustBufferAccess is enabled(+ add none notification)

        Instance* impl_instace = static_cast<Instance*>(info.physical_device->GetParent());
        if(!(impl_instace->GetEnabledFeatures().debug_messenger ||
             impl_instace->GetEnabledFeatures().validation_layer))
            profile_attributes[8] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;

        if(info.robust_buffer_access_enabled)
            profile_attributes[5] |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;

        glrc = glad_wglCreateContextAttribsARB(win32_info.hdc, nullptr, profile_attributes);
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