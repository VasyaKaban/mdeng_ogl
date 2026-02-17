#include "WGLPhysicalDevice.h"
#include <stdexcept>
#include <map>
#include "hrs/expected.hpp"
#include "hrs/scoped_call.hpp"
#include "../../Instance/Instance.h"
#include "../../Surface/Surface.h"
#include "../../Device/Device.h"
#include "glad/wgl.h"
#include "Core/Utils/System.h"
#include "../../../WGL.h"
#include "Core/Render/Format.h"

namespace OpenGL
{
    constexpr static wchar_t WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS_NAME[] =
        L"WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS";
    constexpr static wchar_t WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_TITLE[] =
        L"WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_TITLE";

    struct WindowParams
    {
        Instance* input_instance;
        HDC dc;
        HGLRC glrc;
        PhysicalDeviceSurfaceDesc surface_desc;
        std::vector<std::uint32_t> pixelformat_indices;
        GladGLContext loader;
    };

    struct FormatDesc
    {
        /*
        WGL_NO_ACCELERATION_ARB                 0x2025
        WGL_GENERIC_ACCELERATION_ARB            0x2026
        WGL_FULL_ACCELERATION_ARB               0x2027
        */
        int acceleration;
        int index;
    };

    static LRESULT CALLBACK WGLPhysicalDeviceDummyProc(HWND handle,
                                                       UINT message,
                                                       WPARAM w_param,
                                                       LPARAM l_param)
    {
        switch(message)
        {
            case WM_CREATE:
            {
                hrs::expected<WindowParams, std::runtime_error>* window_params_exp =
                    reinterpret_cast<hrs::expected<WindowParams, std::runtime_error>*>(
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
                    *window_params_exp = std::move(*ex_opt);
                    return -1;
                }

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

                bool debug_messenger_enabled =
                    window_params_exp->value()
                        .input_instance->GetEnabledFeatures()
                        .validation_layer ||
                    window_params_exp->value().input_instance->GetEnabledFeatures().debug_messenger;

                auto glrc_exp = CreateContext(_dc, debug_messenger_enabled, false);
                if(!glrc_exp.has_value())
                {
                    *window_params_exp = std::move(glrc_exp.error());
                    return -1;
                }

                _glrc = glrc_exp.value();

                wglMakeCurrent(_dc, _glrc);

                GladGLContext loader;

                int glad_ver =
                    gladLoadGLContext(&loader, reinterpret_cast<GLADloadfunc>(wglGetProcAddress));
                if(glad_ver == 0)
                {
                    *window_params_exp = std::runtime_error("Failed to load GLAD");
                    return -1;
                }

                constexpr static int format_number_query_input = WGL_NUMBER_PIXEL_FORMATS_ARB;
                int format_number_query_output = 0;

                std::vector<Render::Format> surface_formats;
                std::vector<std::uint32_t> pixelformat_indices;
                glad_wglGetPixelFormatAttribivARB(_dc,
                                                  0,
                                                  0,
                                                  1,
                                                  &format_number_query_input,
                                                  &format_number_query_output);

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
                    WGL_RED_SHIFT_ARB, //10
                    WGL_GREEN_SHIFT_ARB, //11
                    WGL_BLUE_SHIFT_ARB, //12
                    WGL_ALPHA_SHIFT_ARB, //13
                    WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, //14
                };

                int format_query_output[std::size(format_query_input)] = {};

                std::size_t format_query_input_size =
                    (GLAD_WGL_ARB_framebuffer_sRGB ? std::size(format_query_input) :
                                                     std::size(format_query_input) - 1);

                struct std::map<Render::Format, FormatDesc> formats_set;

                for(std::size_t i = 0; i < format_number_query_output; i++)
                {
                    glad_wglGetPixelFormatAttribivARB(_dc,
                                                      i,
                                                      0,
                                                      format_query_input_size,
                                                      format_query_input,
                                                      format_query_output);

                    if(format_query_output[0 /*WGL_DRAW_TO_WINDOW_ARB*/] == FALSE ||
                       format_query_output[2 /*WGL_SUPPORT_OPENGL_ARB*/] == FALSE ||
                       //format_query_output[3 /*WGL_DOUBLE_BUFFER_ARB*/] == FALSE ||
                       format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] == WGL_TYPE_COLORINDEX_ARB)
                    {
                        continue;
                    }

                    auto format_opt = DecodeSurfaceFormat(
                        loader,
                        SurfaceConfig{
                            .red_bits = static_cast<std::uint8_t>(
                                format_query_output[6 /*WGL_RED_BITS_ARB*/]),
                            .red_shift = static_cast<std::uint8_t>(
                                format_query_output[10 /*WGL_RED_SHIFT_ARB*/]),
                            .green_bits =
                                static_cast<uint8_t>(format_query_output[7 /*WGL_GREEN_BITS_ARB*/]),
                            .green_shift = static_cast<uint8_t>(
                                format_query_output[11 /*WGL_GREEN_SHIFT_ARB*/]),
                            .blue_bits =
                                static_cast<uint8_t>(format_query_output[8 /*WGL_BLUE_BITS_ARB*/]),
                            .blue_shift = static_cast<uint8_t>(
                                format_query_output[12 /*WGL_BLUE_SHIFT_ARB*/]),
                            .alpha_bits =
                                static_cast<uint8_t>(format_query_output[9 /*WGL_ALPHA_BITS_ARB*/]),
                            .alpha_shift = static_cast<uint8_t>(
                                format_query_output[13 /*WGL_ALPHA_SHIFT_ARB*/]),
                            .color_bits =
                                static_cast<uint8_t>(format_query_output[5 /*WGL_COLOR_BITS_ARB*/]),
                            .is_srgb = (GLAD_WGL_ARB_framebuffer_sRGB ?
                                            static_cast<bool>(
                                                format_query_output
                                                    [14 /*WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB*/]) :
                                            false),
                            .is_float = (format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] ==
                                         WGL_TYPE_RGBA_FLOAT_ARB)});

                    if(!format_opt)
                        continue;

                    FormatDesc format_desc =
                        FormatDesc{.acceleration = format_query_output[1 /*WGL_ACCELERATION_ARB*/],
                                   .index = static_cast<int>(i)};
                    auto [it, inserted] = formats_set.insert({*format_opt, format_desc});
                    if(!inserted)
                    {
                        if(it->second.acceleration < format_desc.acceleration)
                            it->second = format_desc;
                    }
                }

                if(formats_set.empty())
                {
                    *window_params_exp =
                        std::runtime_error("Physical device does not contain any formats");
                    return -1;
                }

                pixelformat_indices.reserve(formats_set.size());
                surface_formats.reserve(formats_set.size());

                for(const auto& [format, desc]: formats_set)
                {
                    pixelformat_indices.push_back(desc.index);
                    surface_formats.push_back(format);
                }

                Render::PresentModeFlags supported_present_modes =
                    Render::PresentModeFlagBits::FIFO;
                if(GLAD_WGL_EXT_swap_control)
                    supported_present_modes |= Render::PresentModeFlagBits::Immediate;

                if(GLAD_WGL_EXT_swap_control_tear)
                    supported_present_modes |= Render::PresentModeFlagBits::RelaxedFIFO;

                PhysicalDeviceSurfaceDesc surface_desc = {
                    .supported_present_modes = supported_present_modes,
                    .supported_formats = std::move(surface_formats)};

                *window_params_exp =
                    WindowParams{.dc = _dc,
                                 .glrc = _glrc,
                                 .surface_desc = std::move(surface_desc),
                                 .pixelformat_indices = std::move(pixelformat_indices),
                                 .loader = loader};

                cleanup.drop();
                break;
            }
            default:
                return DefWindowProcW(handle, message, w_param, l_param);
                break;
        }

        return 0;
    }

    WGLPhysicalDevice::WGLPhysicalDevice(Instance* _parent)
        : parent(_parent),
          device(nullptr)
    {
        HINSTANCE _instance = GetModuleHandleW(nullptr);

        WNDCLASSEXW window_class = {.cbSize = sizeof(WNDCLASSEXW),
                                    .style = CS_OWNDC,
                                    .lpfnWndProc = WGLPhysicalDeviceDummyProc,
                                    .cbClsExtra = 0,
                                    .cbWndExtra = 0,
                                    .hInstance = _instance,
                                    .hIcon = nullptr,
                                    .hCursor = nullptr,
                                    .hbrBackground = nullptr,
                                    .lpszMenuName = nullptr,
                                    .lpszClassName = WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS_NAME,
                                    .hIconSm = nullptr};
        auto register_res = RegisterClassExW(&window_class);
        if(register_res == 0)
            throw std::runtime_error("Failed to create dummy OpenGL window");

        hrs::expected<WindowParams, std::runtime_error> window_param_exp =
            WindowParams{.input_instance = parent};

        HWND _window = CreateWindowExW(0,
                                       WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS_NAME,
                                       WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_TITLE,
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

            UnregisterClassW(WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS_NAME, _instance);

            throw window_param_exp.error();
        }

        hinstance = _instance;
        window = _window;
        dc = window_param_exp->dc;
        glrc = window_param_exp->glrc;
        surface_desc = std::move(window_param_exp->surface_desc);
        pixelformat_indices = std::move(window_param_exp->pixelformat_indices);

        loader = window_param_exp->loader;

        bool debug_messenger_enabled = parent->GetEnabledFeatures().validation_layer ||
                                       parent->GetEnabledFeatures().debug_messenger;

        if(debug_messenger_enabled)
        {
            const auto& info = parent->GetDebugMessengerInfo();

            EnableDebugMessenger(loader);
            ::OpenGL::SetDebugMessenger(loader, info);
        }

        properties =
            GetPhysicalDeviceProperties(loader, GLAD_WGL_ARB_create_context_robustness != 0);
    }

    WGLPhysicalDevice::~WGLPhysicalDevice()
    {
        wglDeleteContext(glrc);
        ReleaseDC(window, dc);
        DestroyWindow(window);
        UnregisterClassW(WGL_PHYSICAL_DEVICE_DUMMY_WINDOW_CLASS_NAME, hinstance);
    }

    const Render::PhysicalDeviceProperties& WGLPhysicalDevice::GetProperties() const noexcept
    {
        return properties;
    }

    bool WGLPhysicalDevice::GetSurfaceSupport(Render::Surface* surface,
                                              std::uint32_t queue_family_index) const noexcept
    {
        Surface* impl_surface = static_cast<Surface*>(surface);
        if(!impl_surface->IsConnected())
            return true;

        return impl_surface->GetConnectedPhysicalDevice() == this;
    }

    Render::SurfaceCapabilities
    WGLPhysicalDevice::GetSurfaceCapablities(Render::Surface* surface) const noexcept
    {
        Surface* impl_surface = static_cast<Surface*>(surface);

        if(impl_surface->IsConnected())
            return impl_surface->GetConnectedCapabilities();

        return Render::SurfaceCapabilities{.min_image_count = SURFACE_MIN_IMAGE_COUNT,
                                           .max_image_count = SURFACE_MAX_IMAGE_COUNT,
                                           .supported_present_modes =
                                               surface_desc.supported_present_modes,
                                           .supported_formats = surface_desc.supported_formats,
                                           .min_extent = SURFACE_MIN_EXTENT,
                                           .current_extent = SURFACE_CURRENT_EXTENT,
                                           .max_extent = SURFACE_MAX_EXTENT,
                                           .extent_mode = SURFACE_EXTENT_MODE};
    }

    std::optional<Render::BufferFormatProperties>
    WGLPhysicalDevice::GetBufferFormatProperties(const Render::BufferFormatInfo& info) const
    {
        return GetPhysicalDeviceBufferFormatProperties(loader, info);
    }

    std::optional<Render::ImageFormatProperties>
    WGLPhysicalDevice::GetImageFormatProperties(const Render::ImageFormatInfo& info) const
    {
        return GetPhysicalDeviceImageFormatProperties(loader, info);
    }

    Render::Device* WGLPhysicalDevice::CreateDevice(const Render::DeviceInfo& info)
    {
        if(device != nullptr)
            throw std::runtime_error("Physical device already has created logical device");

        Surface* impl_surface = static_cast<Surface*>(info.surface);
        if(impl_surface->IsConnected())
            throw std::runtime_error("Surface is connected to other device");

        device = new Device(this, info);
        return device;
    }

    Render::Instance* WGLPhysicalDevice::GetParent() const noexcept
    {
        return parent;
    }

    GLADloadfunc WGLPhysicalDevice::GetProcAddressResolver() const noexcept
    {
        return reinterpret_cast<GLADloadfunc>(wglGetProcAddress);
    }

    Render::PresentModeFlags WGLPhysicalDevice::GetSupportedPresentModes() const noexcept
    {
        return surface_desc.supported_present_modes;
    }

    std::uint32_t WGLPhysicalDevice::GetDescribePixelFormatIndex(Render::Format format) const
    {
        auto it = std::ranges::find(surface_desc.supported_formats, format);
        if(it == surface_desc.supported_formats.end())
            throw std::runtime_error(
                std::format("{} is not supported by surface", Render::FormatToString(format)));

        return pixelformat_indices[std::distance(surface_desc.supported_formats.begin(), it)];
    }

    void WGLPhysicalDevice::DeleteDeviceNotify() noexcept
    {
        device = nullptr;

        wglMakeCurrent(dc, glrc);
    }

    void WGLPhysicalDevice::SetDebugMessenger(const Render::DebugMessengerInfo& info)
    {
        wglMakeCurrent(dc, glrc);

        OpenGL::SetDebugMessenger(loader, info);

        if(device)
            device->SetDebugMessenger(info);
    }
};