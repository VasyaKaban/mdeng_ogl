#include "SurfaceBase.h"
#include <stdexcept>
#include "glad/wgl.h"
#include "../Objects/PhysicalDevice/PhysicalDevice.h"
#include "../Objects/Instance/Instance.h"

namespace OpenGL
{

    SurfaceBase::SurfaceBase(const Render::SurfaceWin32Info& _info) noexcept
        : info(_info),
          glrc(nullptr)
    {}

    SurfaceBase::~SurfaceBase()
    {
        if(glrc)
            wglDeleteContext(glrc);
    }

    void SurfaceBase::Connect(const SurfaceConnectInfo& connect_info)
    {
        if(glrc)
            throw std::runtime_error("Failed to create already created WGL context");

        //select pixel format for current DC
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(info.hdc,
                            connect_info.config_index,
                            sizeof(PIXELFORMATDESCRIPTOR),
                            &pfd);

        if(SetPixelFormat(info.hdc, connect_info.config_index, &pfd) == FALSE)
            throw hrs::winapi_get_last_error();

        Instance* impl_instace = static_cast<Instance*>(connect_info.physical_device->GetParent());
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
                (connect_info.robust_buffer_access_enabled ? WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB : 0),
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

        glrc = glad_wglCreateContextAttribsARB(info.hdc, nullptr, profile_attributes.data());
        if(!glrc)
            throw hrs::winapi_get_last_error();

        wglMakeCurrent(info.hdc, glrc);
    }

    bool SurfaceBase::IsConnected() const noexcept
    {
        return glrc != nullptr;
    }

    void SurfaceBase::SetSwapInterval(Render::PresentModeFlagBits present_mode)
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

    void SurfaceBase::SwapWindow()
    {
        wglSwapLayerBuffers(info.hdc, WGL_SWAP_MAIN_PLANE);
    }

    void SurfaceBase::MakeCurrent()
    {
        wglMakeCurrent(info.hdc, glrc);
    }

    Render::Extent2D SurfaceBase::GetCurrentExtent() const noexcept
    {
        LPRECT rect;
        if(!GetClientRect(info.window, rect))
            return Render::Extent2D{.width = 0, .height = 0};

        return Render::Extent2D{.width = static_cast<std::uint32_t>(rect->right),
                                .height = static_cast<std::uint32_t>(rect->bottom)};
    }
};