#include "WGLResolve.h"
#include "glad/wgl.h"
#include "hrs/scoped_call.hpp"
#include <stdexcept>
#include "../../Objects/Instance/Instance.h"
#include "../../WGL.h"

namespace OpenGL
{
    constexpr static wchar_t WGL_DUMMY_WINDOW_CLASS_NAME[] = L"WGL_DUMMY_WINDOW_CLASS_RESOLVE";
    constexpr static wchar_t WGL_DUMMY_WINDOW_TITLE[] = L"WGL_DUMMY_WINDOW_TITLE_RESOLVE";

    constexpr static Render::SurfaceBackend WIN32_AVAILABLE_SURFACE_BACKENDS[] = {
        Render::SurfaceBackend::Win32};

    static LRESULT CALLBACK WGLDummyProc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
    {
        switch(message)
        {
            case WM_CREATE:
            {
                std::optional<std::runtime_error>* window_param_opt =
                    reinterpret_cast<std::optional<std::runtime_error>*>(
                        reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                HDC _dc = nullptr;
                HGLRC _glrc = nullptr;
                hrs::scoped_call cleanup = [&_dc, &_glrc, handle]()
                {
                    if(_glrc)
                        wglDeleteContext(_glrc);

                    if(_dc)
                        ReleaseDC(handle, _dc);
                };

                _dc = GetDC(handle);
                auto ex_opt = SetDefaultPixelFormat(_dc);
                if(ex_opt)
                {
                    *window_param_opt = std::move(*ex_opt);
                    return -1;
                }

                _glrc = wglCreateContext(_dc);
                if(_glrc == nullptr)
                {
                    *window_param_opt = Core::System::GetLastError();
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

    WGLResolve::WGLResolve()
        : instance_features{.validation_layer = true, .debug_messenger = true} //we use OGL4.5+
    {}

    WGLResolve::~WGLResolve()
    {}

    void WGLResolve::Init()
    {
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

        std::optional<std::runtime_error> window_param_opt;

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
                                       &window_param_opt);

        if(_window)
            DestroyWindow(_window);

        UnregisterClassW(WGL_DUMMY_WINDOW_CLASS_NAME, _instance);

        if(window_param_opt.has_value())
            throw window_param_opt.value();
    }

    const Render::InstanceFeatures& WGLResolve::GetInstanceFeatures() const noexcept
    {
        return instance_features;
    }

    Render::Backend WGLResolve::GetBackend() const noexcept
    {
        return Render::Backend::OpenGL;
    }

    const std::span<const Render::SurfaceBackend> WGLResolve::GetAvailableSurfaceBackends() const
    {
        return WIN32_AVAILABLE_SURFACE_BACKENDS;
    }

    Render::Instance* WGLResolve::CreateInstance(const Render::InstanceInfo& info)
    {
        if(std::ranges::find(WIN32_AVAILABLE_SURFACE_BACKENDS, info.surface_backend) ==
           std::end(WIN32_AVAILABLE_SURFACE_BACKENDS))
            throw std::runtime_error("Only Win32 surface is supported");

        return new Instance(info);
    }
}