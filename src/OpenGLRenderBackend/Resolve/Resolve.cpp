#include "Resolve.h"
#include "OpenGLRenderBackend/API.h"
#include "glad/wgl.h"
#include "Core/Utils/ScopedCall.hpp"
#include <stdexcept>
#include "../Objects/Instance/Instance.h"

namespace OpenGL
{
    extern "C" OGL_RENDER_BACKEND_API_EXPORT Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }

#ifdef _WIN32
    constexpr static wchar_t WGL_DUMMY_WINDOW_CLASS_NAME[] = L"WGL_DUMMY_WINDOW_CLASS_RESOLVE";
    constexpr static wchar_t WGL_DUMMY_WINDOW_TITLE[] = L"WGL_DUMMY_WINDOW_TITLE_RESOLVE";
#endif

    constexpr static Render::SurfaceBackend AVAILABLE_SURFACE_BACKENDS[] = {
#ifdef _WIN32
        Render::SurfaceBackend::Win32
#else
#    error TODO!
#endif
    };

#ifdef _WIN32
    static LRESULT CALLBACK WGLDummyProc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
    {
        switch(message)
        {
            case WM_CREATE:
            {
                std::exception_ptr* window_param = reinterpret_cast<std::exception_ptr*>(
                    reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                HDC _dc = nullptr;
                HGLRC _glrc = nullptr;
                Core::ScopedCall cleanup = [&_dc, &_glrc, handle]()
                {
                    if(_glrc)
                        wglDeleteContext(_glrc);

                    if(_dc)
                        ReleaseDC(handle, _dc);
                };

                _dc = GetDC(handle);

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

                int format_index = ChoosePixelFormat(_dc, &pfd);
                if(format_index == 0)
                {
                    *window_param = Core::System::GetLastError();
                    return -1;
                }

                if(SetPixelFormat(_dc, format_index, &pfd) == FALSE)
                {
                    *window_param = Core::System::GetLastError();
                    return -1;
                }

                _glrc = wglCreateContext(_dc);
                if(_glrc == nullptr)
                {
                    *window_param = Core::System::GetLastError();
                    return -1;
                }

                wglMakeCurrent(_dc, _glrc);

                int wgl_version =
                    gladLoadWGL(_dc, reinterpret_cast<GLADloadfunc>(wglGetProcAddress));
                if(wgl_version == 0)
                {
                    *window_param =
                        std::make_exception_ptr(std::runtime_error("Failed to load WGL context"));
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
                    *window_param = std::make_exception_ptr(std::runtime_error(
                        "WGL core context or pixel format selection is not available"));
                    return -1;
                }
                break;
            }
        }

        return -1;
    }
#endif

    Resolve::Resolve()
        : instance_features{.validation_layer = true, .debug_messenger = true} //we use OGL4.5+
    {}

    Resolve::~Resolve()
    {}

    void Resolve::Init()
    {
#ifdef _WIN32
        HINSTANCE _instance = GetModuleHandleW(nullptr);

        WNDCLASSEXW window_class = {.cbSize = sizeof(WNDCLASSEXW),
                                    .style = CS_OWNDC,
                                    .lpfnWndProc = WGLDummyProc,
                                    .cbClsExtra = 0,
                                    .cbWndExtra = 0,
                                    .hInstance = _instance,
                                    .hIcon = nullptr,
                                    .hCursor = nullptr,
                                    .hbrBackground = nullptr,
                                    .lpszMenuName = nullptr,
                                    .lpszClassName = WGL_DUMMY_WINDOW_CLASS_NAME,
                                    .hIconSm = nullptr};
        auto register_res = RegisterClassExW(&window_class);
        if(register_res == 0)
            throw std::runtime_error("Failed to create dummy OpenGL window");

        std::exception_ptr window_param;

        HWND _window = CreateWindowExW(0,
                                       WGL_DUMMY_WINDOW_CLASS_NAME,
                                       WGL_DUMMY_WINDOW_TITLE,
                                       0,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       nullptr,
                                       nullptr,
                                       _instance,
                                       &window_param);

        if(_window)
            DestroyWindow(_window);

        UnregisterClassW(WGL_DUMMY_WINDOW_CLASS_NAME, _instance);

        if(window_param)
            std::rethrow_exception(window_param);
#else
#    error TODO!
#endif
    }

    const Render::InstanceFeatures& Resolve::GetInstanceFeatures() const noexcept
    {
        return instance_features;
    }

    Render::Backend Resolve::GetBackend() const noexcept
    {
        return Render::Backend::OpenGL;
    }

    const std::span<const Render::SurfaceBackend> Resolve::GetAvailableSurfaceBackends() const
    {
        return AVAILABLE_SURFACE_BACKENDS;
    }

    Render::Instance* Resolve::CreateInstance(const Render::InstanceInfo& info)
    {
        if(std::ranges::find(AVAILABLE_SURFACE_BACKENDS, info.surface_backend) ==
           std::end(AVAILABLE_SURFACE_BACKENDS))
            throw std::runtime_error("Unsupported surface backend");

        return new Instance(info);
    }
}