#include "Surface.h"
#include <map>
#include "glad/wgl.h"
#include "Core/Utils/ScopedCall.hpp"
#include "Core/Render/Format.h"

namespace OpenGL
{
#ifdef _WIN32
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

    Surface::Surface(Instance* _parent, const Render::Win32SurfaceInfo& _info)
        : parent(_parent),
          connected_device(nullptr),
          window(_info.window),
          dc(nullptr),
          glrc(nullptr)
    {
        HDC _dc = nullptr;
        Core::ScopedCall cleanup(
            [&_dc, window = _info.window]()
            {
                if(_dc)
                    ReleaseDC(window, _dc);
            });

        _dc = GetDC(_info.window);
        if(_dc == nullptr)
            Core::System::ThrowLastError();

        //fill legacy_physical_device_features
        legacy_physical_device_features = Render::LegacyPhysicalDeviceFeatures{
            .robust_buffer_access = static_cast<bool>(GLAD_WGL_ARB_create_context_robustness)};

        //fill connected_capabilities
        Render::PresentModeFlags supported_present_modes = Render::PresentModeFlagBits::FIFO;
        if(GLAD_WGL_EXT_swap_control)
            supported_present_modes |= Render::PresentModeFlagBits::Immediate;

        if(GLAD_WGL_EXT_swap_control_tear)
            supported_present_modes |= Render::PresentModeFlagBits::RelaxedFIFO;

        constexpr static int FORMAT_NUMBER_QUERY_INPUT = WGL_NUMBER_PIXEL_FORMATS_ARB;
        int format_number_query_output = 0;

        std::vector<Render::Format> surface_formats;
        std::vector<std::uint32_t> pixelformat_indices;
        glad_wglGetPixelFormatAttribivARB(_dc,
                                          0,
                                          0,
                                          1,
                                          &FORMAT_NUMBER_QUERY_INPUT,
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

            auto format_opt = DecodeSurfaceFormat(SurfaceConfig{
                .red_bits = static_cast<std::uint8_t>(format_query_output[6 /*WGL_RED_BITS_ARB*/]),
                .red_shift =
                    static_cast<std::uint8_t>(format_query_output[10 /*WGL_RED_SHIFT_ARB*/]),
                .green_bits = static_cast<uint8_t>(format_query_output[7 /*WGL_GREEN_BITS_ARB*/]),
                .green_shift =
                    static_cast<uint8_t>(format_query_output[11 /*WGL_GREEN_SHIFT_ARB*/]),
                .blue_bits = static_cast<uint8_t>(format_query_output[8 /*WGL_BLUE_BITS_ARB*/]),
                .blue_shift = static_cast<uint8_t>(format_query_output[12 /*WGL_BLUE_SHIFT_ARB*/]),
                .alpha_bits = static_cast<uint8_t>(format_query_output[9 /*WGL_ALPHA_BITS_ARB*/]),
                .alpha_shift =
                    static_cast<uint8_t>(format_query_output[13 /*WGL_ALPHA_SHIFT_ARB*/]),
                .color_bits = static_cast<uint8_t>(format_query_output[5 /*WGL_COLOR_BITS_ARB*/]),
                .is_srgb = (GLAD_WGL_ARB_framebuffer_sRGB ?
                                static_cast<bool>(
                                    format_query_output[14 /*WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB*/]) :
                                false),
                .is_float =
                    (format_query_output[4 /*WGL_PIXEL_TYPE_ARB*/] == WGL_TYPE_RGBA_FLOAT_ARB)});

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
            throw std::runtime_error("Surface do not support any formats");

        pixelformat_indices.reserve(formats_set.size());
        surface_formats.reserve(formats_set.size());

        for(const auto& [format, desc]: formats_set)
        {
            pixelformat_indices.push_back(desc.index);
            surface_formats.push_back(format);
        }

        connected_capabilities =
            Render::SurfaceCapabilities{.min_image_count = SURFACE_MIN_IMAGE_COUNT,
                                        .max_image_count = SURFACE_MAX_IMAGE_COUNT,
                                        .supported_present_modes = supported_present_modes,
                                        .supported_formats = std::move(surface_formats),
                                        .min_extent = SURFACE_MIN_EXTENT,
                                        .current_extent = SURFACE_CURRENT_EXTENT,
                                        .max_extent = SURFACE_MAX_EXTENT,
                                        .extent_mode = SURFACE_EXTENT_MODE};

        this->dc = _dc;
        this->pixelformat_indices = std::move(pixelformat_indices);

        cleanup.Drop();
    }
#elif defined(linux)
    Surface::Surface(Instance* _parent, const Render::XCBSurfaceInfo& _info)
    {
#    error TODO!
    }
#endif

    Surface::~Surface()
    {
#ifdef _WIN32
        if(glrc)
            wglDeleteContext(glrc);

        ReleaseDC(window, dc);
#else
#    error TODO!
#endif
    }

    Render::Instance* Surface::GetParent() const noexcept
    {
        return parent;
    }

    const Render::LegacyPhysicalDeviceFeatures&
    Surface::GetLegacyPhysicalDeviceFeatures() const noexcept
    {
        return legacy_physical_device_features;
    }

    Render::SurfaceCapabilities Surface::GetLegacySurfaceCapablities() const
    {
        return connected_capabilities;
    }

    Device* Surface::GetConnectedDevice() const noexcept
    {
        return connected_device;
    }

    void Surface::Connect(const Render::LegacyDeviceInfo& info)
    {
        if(IsConnected())
            throw std::runtime_error("Failed to create already created WGL context");

        //select pixel format for current DC
        auto it =
            std::ranges::find(connected_capabilities.supported_formats, info.swapchain_info.format);
        if(it == connected_capabilities.supported_formats.end())
            throw std::runtime_error(
                std::format("{} is not supported by surface",
                            Render::FormatToString(info.swapchain_info.format)));

        auto descibe_pixelformat_index =
            pixelformat_indices[std::distance(connected_capabilities.supported_formats.begin(),
                                              it)];

#ifdef _WIN32
        PIXELFORMATDESCRIPTOR pfd;
        int res =
            DescribePixelFormat(dc, descibe_pixelformat_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if(res == 0)
            Core::System::ThrowLastError();

        if(SetPixelFormat(dc, descibe_pixelformat_index, &pfd) == FALSE)
            Core::System::ThrowLastError();

        bool debug_messenger_enabled = parent->GetEnabledFeatures().validation_layer ||
                                       parent->GetEnabledFeatures().debug_messenger;

        int profile_attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, //0
            4, //1
            WGL_CONTEXT_MINOR_VERSION_ARB, //2
            5, //3
            WGL_CONTEXT_FLAGS_ARB, //4
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
                (debug_messenger_enabled ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) |
                (info.enabled_features.robust_buffer_access ? WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB :
                                                              0), //5
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

        glrc = glad_wglCreateContextAttribsARB(dc, nullptr, profile_attributes);
        if(!glrc)
            Core::System::ThrowLastError();

        wglMakeCurrent(dc, glrc);
#else
#    error TODO!
#endif

        //update supported formats
        connected_capabilities.supported_formats = {info.swapchain_info.format};
    }

    bool Surface::IsConnected() const noexcept
    {
#ifdef _WIN32
        return glrc != nullptr;
#else
#    error TODO!
#endif
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
            case Render::PresentModeFlagBits::Mailbox:
                throw std::runtime_error("Mailbox present mode is not supported");
                break;
        }

#ifdef _WIN32
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
#else
#    error TODO!
#endif
    }

    void Surface::SwapWindow()
    {
#ifdef _WIN32
        wglSwapLayerBuffers(dc, WGL_SWAP_MAIN_PLANE);
#else
#    error TODO!
#endif
    }

    void Surface::MakeCurrent()
    {
#ifdef _WIN32
        wglMakeCurrent(dc, glrc);
#else
#    error TODO!
#endif
    }

    GLADloadfunc Surface::GetProcAddressResolver() const noexcept
    {
#ifdef _WIN32
        return reinterpret_cast<GLADloadfunc>(wglGetProcAddress);
#else
#    error TODO!
#endif
    }
};