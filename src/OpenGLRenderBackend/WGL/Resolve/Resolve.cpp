#include "Resolve.h"
#include "hrs/scoped_call.hpp"
#include <optional>
#include "../../Objects/Instance/Instance.h"
#include "glad/wgl.h"

namespace OpenGL
{
    constexpr static wchar_t DUMMY_WINDOW_CLASS_NAME[] = L"DUMMY_WINDOW_CLASS_RESOLVE";
    constexpr static wchar_t DUMMY_WINDOW_TITLE[] = L"DUMMY_WINDOW_TITLE_RESOLVE";

    static LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
            case WM_CREATE:
            {
                std::optional<std::runtime_error>* window_param_opt =
                    reinterpret_cast<std::optional<std::runtime_error>*>(lParam);

                HDC _dc = nullptr;
                HGLRC _glrc = nullptr;
                hrs::scoped_call cleanup = [&_dc, &_glrc, hwnd]()
                {
                    if(_glrc)
                        wglDeleteContext(_glrc);

                    if(_dc)
                        ReleaseDC(hwnd, _dc);
                };

                PIXELFORMATDESCRIPTOR pfd = {.nSize = sizeof(PIXELFORMATDESCRIPTOR),
                                             .nVersion = 1,
                                             .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                                        PFD_DOUBLEBUFFER,
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

                _dc = GetDC(hwnd);
                int format_index = ChoosePixelFormat(_dc, &pfd);
                if(format_index == 0)
                {
                    *window_param_opt = hrs::winapi_get_last_error();
                    return -1;
                }

                if(SetPixelFormat(_dc, format_index, &pfd) == FALSE)
                {
                    *window_param_opt = hrs::winapi_get_last_error();
                    return -1;
                }

                _glrc = wglCreateContext(_dc);
                if(_glrc == nullptr)
                {
                    *window_param_opt = hrs::winapi_get_last_error();
                    return -1;
                }

                wglMakeCurrent(_dc, _glrc);

                int wgl_version =
                    gladLoadWGL(_dc, reinterpret_cast<GLADloadfunc>(wglGetProcAddress));
                if(wgl_version == 0)
                {
                    *window_param_opt = std::runtime_error("Failed to load WGL context");
                    return -1;
                }

                wglDeleteContext(_glrc);
                _glrc = nullptr;

                /*
                GLAD_API_CALL int GLAD_WGL_VERSION_1_0;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_no_error;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_profile;
                GLAD_API_CALL int GLAD_WGL_ARB_create_context_robustness;
                GLAD_API_CALL int GLAD_WGL_ARB_extensions_string;
                GLAD_API_CALL int GLAD_WGL_ARB_framebuffer_sRGB;
                GLAD_API_CALL int GLAD_WGL_ARB_pixel_format;
                GLAD_API_CALL int GLAD_WGL_ARB_pixel_format_float;
                GLAD_API_CALL int GLAD_WGL_EXT_extensions_string;
                GLAD_API_CALL int GLAD_WGL_EXT_swap_control;
                GLAD_API_CALL int GLAD_WGL_EXT_swap_control_tear;
                */

                if(!(GLAD_WGL_ARB_create_context && GLAD_WGL_ARB_create_context_profile &&
                     GLAD_WGL_ARB_pixel_format))
                {
                    *window_param_opt = std::runtime_error(
                        "WGL core context or pixel format selection is not available");
                    return -1;
                }
                break;
            }
        }

        return -1;
    }

    Resolve::Resolve()
        : instance_features{.validation_layer = true, .debug_messenger = true} //we use OGL4.5+
    {}

    Resolve::~Resolve()
    {}

    void Resolve::Init()
    {
        HINSTANCE _instance = GetModuleHandleW(nullptr);

        WNDCLASS window_class = {};
        window_class.lpszClassName = DUMMY_WINDOW_CLASS_NAME;
        window_class.hInstance = _instance;
        window_class.lpfnWndProc = DummyWindowProc;
        auto register_res = RegisterClassW(&window_class);
        if(register_res == 0)
            throw std::runtime_error("Failed to create dummy OpenGL window");

        std::optional<std::runtime_error> window_param_opt;

        HWND _window = CreateWindowExW(0,
                                       DUMMY_WINDOW_CLASS_NAME,
                                       DUMMY_WINDOW_TITLE,
                                       0,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       nullptr,
                                       nullptr,
                                       _instance,
                                       &window_param_opt);

        if(_window)
            DestroyWindow(_window);

        UnregisterClassW(DUMMY_WINDOW_CLASS_NAME, _instance);

        if(!window_param_opt.has_value())
        {
            throw window_param_opt.value();
        }
    }

    const Render::InstanceFeatures& Resolve::GetInstanceFeatures() const noexcept
    {
        return instance_features;
    }

    Render::Backend Resolve::GetBackend() const noexcept
    {
        return Render::Backend::OpenGL;
    }

    Render::Instance* Resolve::CreateInstance(const Render::InstanceInfo& info)
    {
        return new Instance(info);
    }
};