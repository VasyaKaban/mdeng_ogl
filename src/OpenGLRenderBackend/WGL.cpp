#include "WGL.h"
#include "glad/wgl.h"

namespace OpenGL
{
    std::optional<std::runtime_error> SetDefaultPixelFormat(HDC dc) noexcept
    {
        PIXELFORMATDESCRIPTOR pfd = {.nSize = sizeof(PIXELFORMATDESCRIPTOR),
                                     .nVersion = 1,
                                     .dwFlags =
                                         PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                     .iPixelType = PFD_TYPE_RGBA,
                                     .cColorBits = 32,
                                     .cRedBits = 0,
                                     .cRedShift = 0,
                                     .cGreenBits = 0,
                                     .cGreenShift = 0,
                                     .cBlueBits = 0,
                                     .cBlueShift = 0,
                                     .cAlphaBits = 0,
                                     .cAlphaShift = 0,
                                     .cAccumBits = 0,
                                     .cAccumRedBits = 0,
                                     .cAccumGreenBits = 0,
                                     .cAccumBlueBits = 0,
                                     .cAccumAlphaBits = 0,
                                     .cDepthBits = 0,
                                     .cStencilBits = 0,
                                     .cAuxBuffers = 0,
                                     .iLayerType = PFD_MAIN_PLANE,
                                     .bReserved = 0,
                                     .dwLayerMask = 0,
                                     .dwVisibleMask = 0,
                                     .dwDamageMask = 0};

        int format_index = ChoosePixelFormat(dc, &pfd);
        if(format_index == 0)
            return Core::System::GetLastError();

        if(SetPixelFormat(dc, format_index, &pfd) == FALSE)
            return Core::System::GetLastError();

        return std::nullopt;
    }

    hrs::expected<HGLRC, std::runtime_error>
    CreateContext(HDC dc, bool debug_messenger_enabled, bool robust_buffer_access_enabled) noexcept
    {
        int profile_attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, //0
            4, //1
            WGL_CONTEXT_MINOR_VERSION_ARB, //2
            5, //3
            WGL_CONTEXT_FLAGS_ARB, //4
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
                (debug_messenger_enabled ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) |
                (robust_buffer_access_enabled ? WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB : 0), //5
            WGL_CONTEXT_PROFILE_MASK_ARB, //6
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //7
            0, //8
            0, //9
            0, //10
            0, //11
        };

        std::size_t offset = 8;

        if(!debug_messenger_enabled && GLAD_WGL_ARB_create_context_no_error)
            profile_attributes[offset++] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;

        if(GLAD_WGL_ARB_context_flush_control)
        {
            profile_attributes[offset++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
            profile_attributes[offset++] = WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB;
        }

        HGLRC glrc = glad_wglCreateContextAttribsARB(dc, nullptr, profile_attributes);
        if(!glrc)
            return Core::System::GetLastError();

        return glrc;
    }
};