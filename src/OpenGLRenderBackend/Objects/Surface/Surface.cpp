#include "Surface.h"
#include <stdexcept>
#include "glad/wgl.h"
#include "../PhysicalDevice/PhysicalDevice.h"

namespace OpenGL
{

    Surface::Surface(const Render::SurfaceWin32Info& info) noexcept
        : win32_info(info),
          glrc(nullptr)
    {}

    Surface::~Surface()
    {
        if(glrc)
            wglDeleteContext(glrc);
    }

    Render::SurfaceCapabilities Surface::GetConnectedCapabilities() const
    {
        return connected_capabilities;
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
        if(!info.validation_layer_enabled)
            profile_attributes[8] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;

        if(info.robust_buffer_access_enabled)
            profile_attributes[5] |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;

        glrc = glad_wglCreateContextAttribsARB(win32_info.hdc, nullptr, profile_attributes);
        if(!glrc)
            throw hrs::winapi_get_last_error();

        connected_capabilities = static_cast<PhysicalDevice*>(info.physical_device)
                                     ->GetSurfaceCapabilitiesByIndex(info.config_index);
    }

    bool Surface::IsConnected() const noexcept
    {
        return glrc != nullptr;
    }
};