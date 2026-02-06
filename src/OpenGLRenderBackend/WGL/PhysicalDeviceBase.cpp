#include "PhysicalDeviceBase.h"
#include <stdexcept>
#include "hrs/expected.hpp"
#include "hrs/scoped_call.hpp"
#include "../Objects/Instance/Instance.h"
#include "glad/wgl.h"

namespace OpenGL
{
    constexpr static wchar_t DUMMY_WINDOW_CLASS_NAME[] = L"DUMMY_WINDOW_CLASS_PHYSICAL_DEVICE";
    constexpr static wchar_t DUMMY_WINDOW_TITLE[] = L"DUMMY_WINDOW_TITLE_PHYSICAL_DEVICE";

    struct WindowParams
    {
        Instance* input_instance;
        HDC dc;
        HGLRC glrc;
        Render::SurfaceCapabilities surface_capabilities;
        std::vector<std::uint32_t> pixelformat_indices;
    };

    static LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
            case WM_CREATE:
            {
                hrs::expected<WindowParams, std::runtime_error>* window_params_exp =
                    reinterpret_cast<hrs::expected<WindowParams, std::runtime_error>*>(
                        reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);

                HDC _dc = nullptr;
                HGLRC _glrc = nullptr;
                hrs::scoped_call cleanup = [&_dc, &_glrc, hwnd]()
                {
                    if(_glrc)
                        wglDeleteContext(_glrc);

                    if(_dc)
                        ReleaseDC(hwnd, _dc);
                };

                _dc = GetDC(hwnd);

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

                constexpr static int format_number_query_input[1] = {WGL_NUMBER_PIXEL_FORMATS_ARB};
                int format_number_query_output[1] = {};

                std::vector<Render::SurfaceConfig> surface_configs;
                std::vector<std::uint32_t> pixelformat_indices;
                glad_wglGetPixelFormatAttribivARB(_dc,
                                                  0,
                                                  0,
                                                  1,
                                                  format_number_query_input,
                                                  format_number_query_output);
                surface_configs.reserve(format_number_query_output[0]);
                pixelformat_indices.reserve(format_number_query_output[0]);

                constexpr static int format_query_input[] = {
                    WGL_DRAW_TO_WINDOW_ARB, //0
                    WGL_ACCELERATION_ARB, //1
                    WGL_SUPPORT_OPENGL_ARB, //2
                    WGL_DOUBLE_BUFFER_ARB, //3
                    WGL_PIXEL_TYPE_ARB, //4
                    WGL_COLOR_BITS_ARB, //5
                    WGL_RED_BITS_ARB, //6
                    WGL_GREEN_BITS_ARB, //7
                    WGL_BLUE_BITS_ARB, //8
                    WGL_ALPHA_BITS_ARB, //9
                    WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB //10
                };

                int format_query_output[std::size(format_query_input)] = {};

                std::size_t format_query_input_size =
                    (GLAD_WGL_ARB_framebuffer_sRGB ? std::size(format_query_input) :
                                                     std::size(format_query_input) - 1);

                for(std::size_t i = 0; i < format_number_query_output[0]; i++)
                {
                    glad_wglGetPixelFormatAttribivARB(_dc,
                                                      i,
                                                      0,
                                                      format_query_input_size,
                                                      format_query_input,
                                                      format_query_output);

                    if(format_query_output[0 /*WGL_DRAW_TO_WINDOW_ARB*/] == FALSE ||
                       format_query_output[1 /*WGL_ACCELERATION_ARB*/] !=
                           WGL_FULL_ACCELERATION_ARB ||
                       format_query_output[2 /*WGL_SUPPORT_OPENGL_ARB*/] == FALSE ||
                       format_query_output[3 /*WGL_DOUBLE_BUFFER_ARB*/] == FALSE ||
                       format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] == WGL_TYPE_COLORINDEX_ARB)
                    {
                        continue;
                    }

                    pixelformat_indices.push_back(i);

                    Render::SurfaceConfig cfg = {
                        .red_bits_size =
                            static_cast<std::uint8_t>(format_query_output[6 /*WGL_RED_BITS_ARB*/]),
                        .green_bits_size = static_cast<std::uint8_t>(
                            format_query_output[7 /*WGL_GREEN_BITS_ARB*/]),
                        .blue_bits_size =
                            static_cast<std::uint8_t>(format_query_output[8 /*WGL_BLUE_BITS_ARB*/]),
                        .alpha_bits_size = static_cast<std::uint8_t>(
                            format_query_output[9 /*WGL_ALPHA_BITS_ARB*/]),
                        .color_buffer_bits_size = static_cast<std::uint8_t>(
                            format_query_output[5 /*WGL_COLOR_BITS_ARB*/]),
                        .format_type = (format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] ==
                                                WGL_TYPE_RGBA_FLOAT_ARB ?
                                            Render::FormatType::SFLOAT :
                                            Render::FormatType::UNORM),
                        .srgb_format =
                            (GLAD_WGL_ARB_framebuffer_sRGB ?
                                 static_cast<bool>(
                                     format_query_output[10 /*WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB*/]) :
                                 false)};

                    surface_configs.push_back(cfg);
                }

                Render::PresentModeFlags supported_present_modes =
                    Render::PresentModeFlagBits::FIFO;
                if(GLAD_WGL_EXT_swap_control)
                    supported_present_modes |= Render::PresentModeFlagBits::Immediate;

                if(GLAD_WGL_EXT_swap_control_tear)
                    supported_present_modes |= Render::PresentModeFlagBits::RelaxedFIFO;

                Render::SurfaceCapabilities surface_capabilities =
                    Render::SurfaceCapabilities{.min_image_count = SWAPCHAIN_IMAGE_COUNT,
                                                .max_image_count = SWAPCHAIN_IMAGE_COUNT,
                                                .supported_present_modes = supported_present_modes,
                                                .supported_configs = std::move(surface_configs)};

                bool debug_messenger_enabled =
                    window_params_exp->value()
                        .input_instance->GetEnabledFeatures()
                        .validation_layer ||
                    window_params_exp->value().input_instance->GetEnabledFeatures().debug_messenger;

                std::vector<int> profile_attributes = {
                    WGL_CONTEXT_MAJOR_VERSION_ARB,
                    4,
                    WGL_CONTEXT_MINOR_VERSION_ARB,
                    5,
                    WGL_CONTEXT_FLAGS_ARB,
                    WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
                        (debug_messenger_enabled ? WGL_CONTEXT_DEBUG_BIT_ARB : 0),
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
                    *window_params_exp = hrs::winapi_get_last_error();
                    return -1;
                }

                if(SetPixelFormat(_dc, format_index, &pfd) == FALSE)
                {
                    *window_params_exp = hrs::winapi_get_last_error();
                    return -1;
                }

                _glrc = glad_wglCreateContextAttribsARB(_dc, nullptr, profile_attributes.data());
                if(_glrc == nullptr)
                {
                    *window_params_exp = hrs::winapi_get_last_error();
                    return -1;
                }

                wglMakeCurrent(_dc, _glrc);

                *window_params_exp =
                    WindowParams{.dc = _dc,
                                 .glrc = _glrc,
                                 .surface_capabilities = std::move(surface_capabilities),
                                 .pixelformat_indices = std::move(pixelformat_indices)};

                cleanup.drop();
                break;
            }
            default:
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
                break;
        }

        return 0;
    }

    PhysicalDeviceBase::PhysicalDeviceBase(Instance* instance)
    {
        HINSTANCE _instance = GetModuleHandleW(nullptr);

        WNDCLASSW window_class = {};
        window_class.lpszClassName = DUMMY_WINDOW_CLASS_NAME;
        window_class.hInstance = _instance;
        window_class.lpfnWndProc = DummyWindowProc;
        auto register_res = RegisterClassW(&window_class);
        if(register_res == 0)
            throw std::runtime_error("Failed to create dummy OpenGL window");

        hrs::expected<WindowParams, std::runtime_error> window_param_exp =
            WindowParams{.input_instance = instance};

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
                                       &window_param_exp);

        if(!window_param_exp.has_value())
        {
            if(_window)
                DestroyWindow(_window);

            UnregisterClassW(DUMMY_WINDOW_CLASS_NAME, _instance);

            throw window_param_exp.error();
        }

        hinstance = _instance;
        window = _window;
        dc = window_param_exp->dc;
        glrc = window_param_exp->glrc;
        surface_capabilities = std::move(window_param_exp->surface_capabilities);
        pixelformat_indices = std::move(window_param_exp->pixelformat_indices);
    }

    PhysicalDeviceBase::~PhysicalDeviceBase()
    {
        wglDeleteContext(glrc);
        ReleaseDC(window, dc);
        DestroyWindow(window);
        UnregisterClassW(DUMMY_WINDOW_CLASS_NAME, hinstance);
    }

    GLADloadfunc PhysicalDeviceBase::GetProcAddressResolver() const noexcept
    {
        return reinterpret_cast<GLADloadfunc>(wglGetProcAddress);
    }

    const Render::SurfaceCapabilities& PhysicalDeviceBase::GetSurfaceCapabilities() const noexcept
    {
        return surface_capabilities;
    }

    const Render::SurfaceCapabilities
    PhysicalDeviceBase::GetSurfaceCapabilitiesByIndex(std::uint32_t index) const noexcept
    {
        return Render::SurfaceCapabilities{
            .min_image_count = surface_capabilities.min_image_count,
            .max_image_count = surface_capabilities.max_image_count,
            .supported_present_modes = surface_capabilities.supported_present_modes,
            .supported_configs = {
                surface_capabilities.supported_configs[pixelformat_indices[index]]}};
    }

    std::uint32_t
    PhysicalDeviceBase::GetDescribePixelFormatIndex(std::uint32_t index) const noexcept
    {
        return pixelformat_indices[index];
    }

    void PhysicalDeviceBase::MakeCurrent()
    {
        wglMakeCurrent(dc, glrc);
    }

    bool PhysicalDeviceBase::IsRobustContextSupported() const noexcept
    {
        return GLAD_WGL_ARB_create_context_robustness != 0;
    }
};