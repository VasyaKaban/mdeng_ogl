#include "Win32Display.h"

namespace Core
{
    static bool VideoModeComparator(const VideoMode& vm1, const VideoMode& vm2) noexcept
    {
        return vm1.width < vm2.width || vm1.height < vm2.height ||
               vm1.refresh_rate < vm2.refresh_rate || vm1.bits_per_pixel < vm2.bits_per_pixel;
    }

    static std::uint32_t GetDisplayDPI(Win32WindowSubsystemConnection* parent, HMONITOR handle)
    {
        std::uint32_t dpi = USER_DEFAULT_SCREEN_DPI;

        auto awareness = parent->GetDPIAwrenessType();
        if(awareness == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ||
           awareness == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
        {
            UINT dpi_x;
            UINT dpi_y;
            auto res =
                parent->GetPublicFunctions().GetDpiForMonitor(handle,
                                                              MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI,
                                                              &dpi_x,
                                                              &dpi_y);

            if(res != S_OK)
                throw Core::Win32Exception(HRESULT_CODE(res));

            dpi = dpi_x;
        }
        else if(awareness == DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)
        {
            HDC dc = GetDC(nullptr);
            dpi = GetDeviceCaps(dc, LOGPIXELSX);
            ReleaseDC(nullptr, dc);
        }

        return dpi;
    }

    static VideoMode GetDisplayVideoMode(HMONITOR handle, const wchar_t* name)
    {
        DEVMODEW dev_mode = {.dmSize = sizeof(DEVMODEW), .dmDriverExtra = 0};
        if(EnumDisplaySettingsExW(name, ENUM_CURRENT_SETTINGS, &dev_mode, 0) == 0)
            Core::System::ThrowLastError();

        return VideoMode{.width = dev_mode.dmPelsWidth,
                         .height = dev_mode.dmPelsHeight,
                         .refresh_rate = dev_mode.dmDisplayFrequency,
                         .bits_per_pixel = dev_mode.dmBitsPerPel};
    }

    static WindowPosition GetDisplayPosition(HMONITOR handle)
    {
        MONITORINFOEXW info = {MONITORINFO{.cbSize = sizeof(MONITORINFOEXW)}};
        if(GetMonitorInfoW(handle, &info) == 0)
            Core::System::ThrowLastError();

        return WindowPosition{.x = info.rcMonitor.left, .y = info.rcMonitor.top};
    }

    Win32Display::Win32Display(Win32WindowSubsystemConnection* parent, HMONITOR handle)
        : parent(parent),
          handle(handle),
          dpi(0),
          video_mode({}),
          position({}),
          dev_modes(),
          video_modes(),
          device_name({}),
          device_description()
    {
        //update dpi
        this->dpi = GetDisplayDPI(this->parent, this->handle);

        //set device name
        MONITORINFOEXW info = {MONITORINFO{.cbSize = sizeof(MONITORINFOEXW)}};
        if(GetMonitorInfoW(this->handle, &info) == 0)
            Core::System::ThrowLastError();

        std::copy(info.szDevice, info.szDevice + CCHDEVICENAME, this->device_name.data());

        //set video mode
        this->video_mode = GetDisplayVideoMode(this->handle, this->device_name.data());

        //set position
        this->position = GetDisplayPosition(this->handle);

        //set device description
        const Win32PublicDynamicFunctions& public_functions = this->parent->GetPublicFunctions();
        if(public_functions.QueryDisplayConfig)
        {
            UINT32 flags = QDC_ONLY_ACTIVE_PATHS;
            LONG result = ERROR_SUCCESS;
            std::vector<DISPLAYCONFIG_PATH_INFO> active_path_infos;
            std::vector<DISPLAYCONFIG_MODE_INFO> active_mode_infos;
            do
            {
                UINT32 active_path_count;
                UINT32 active_mode_count;
                result = public_functions.GetDisplayConfigBufferSizes(flags,
                                                                      &active_path_count,
                                                                      &active_mode_count);

                if(result != ERROR_SUCCESS)
                    throw Win32Exception(result);

                active_path_infos.resize(active_path_count);
                active_mode_infos.resize(active_mode_count);

                result = public_functions.QueryDisplayConfig(flags,
                                                             &active_path_count,
                                                             active_path_infos.data(),
                                                             &active_mode_count,
                                                             active_mode_infos.data(),
                                                             nullptr);

                active_path_infos.resize(active_path_count);
                active_mode_infos.resize(active_mode_count);
            }
            while(result == ERROR_INSUFFICIENT_BUFFER);

            if(result != ERROR_SUCCESS)
                throw Win32Exception(result);

            for(const auto& path: active_path_infos)
            {
                DISPLAYCONFIG_TARGET_DEVICE_NAME target_device_name_info = {
                    DISPLAYCONFIG_DEVICE_INFO_HEADER{
                        .type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME,
                        .size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME),
                        .adapterId = path.targetInfo.adapterId,
                        .id = path.targetInfo.id}};

                result =
                    public_functions.DisplayConfigGetDeviceInfo(&target_device_name_info.header);
                if(result != ERROR_SUCCESS)
                    throw Win32Exception(result);

                DISPLAYCONFIG_SOURCE_DEVICE_NAME source_device_name_info = {
                    DISPLAYCONFIG_DEVICE_INFO_HEADER{
                        .type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                        .size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME),
                        .adapterId = path.sourceInfo.adapterId,
                        .id = path.sourceInfo.id}};

                result =
                    public_functions.DisplayConfigGetDeviceInfo(&source_device_name_info.header);
                if(result != ERROR_SUCCESS)
                    throw Win32Exception(result);

                if(std::wcscmp(source_device_name_info.viewGdiDeviceName,
                               this->device_name.data()) == 0)
                {
                    this->device_description = Core::System::WideToUTF8(
                        {target_device_name_info.monitorFriendlyDeviceName,
                         std::wcslen(target_device_name_info.monitorFriendlyDeviceName)});
                    break;
                }
            }
        }
        else //fallback
        {
            DISPLAY_DEVICEW display_device = {.cb = sizeof(DISPLAY_DEVICEW)};
            if(EnumDisplayDevicesW(info.szDevice, 0, &display_device, 0) != 0)
            {
                this->device_description = Core::System::WideToUTF8(
                    {display_device.DeviceString, std::wcslen(display_device.DeviceString)});
            }
        }

        //set dev modes
        std::map<VideoMode, DEVMODEW, decltype(&VideoModeComparator)> dev_modes_map(
            &VideoModeComparator);

        DEVMODEW dev_mode = {.dmSize = sizeof(DEVMODEW), .dmDriverExtra = 0};
        DWORD dev_mode_count = 0;
        while(EnumDisplaySettingsExW(this->device_name.data(), dev_mode_count, &dev_mode, 0) != 0)
        {
            dev_mode_count++;

            if(!((dev_mode.dmFields & DM_PELSWIDTH) && (dev_mode.dmFields & DM_PELSHEIGHT) &&
                 (dev_mode.dmFields & DM_DISPLAYFREQUENCY) && (dev_mode.dmFields & DM_BITSPERPEL)))
                continue;

            auto [it, inserted] =
                dev_modes_map.insert({VideoMode{.width = dev_mode.dmPelsWidth,
                                                .height = dev_mode.dmPelsHeight,
                                                .refresh_rate = dev_mode.dmDisplayFrequency,
                                                .bits_per_pixel = dev_mode.dmBitsPerPel},
                                      dev_mode});

            if(!inserted)
            {
                auto prev_bit_count = std::popcount(it->second.dmFields);
                auto current_bit_count = std::popcount(dev_mode.dmFields);

                if(current_bit_count < prev_bit_count)
                    it->second = dev_mode;
            }
        }

        //set video modes
        this->dev_modes.reserve(dev_modes_map.size());
        this->video_modes.reserve(dev_modes_map.size());
        for(const auto& [v_mode, dev_mode]: dev_modes_map)
        {
            this->dev_modes.push_back(dev_mode);
            this->video_modes.push_back(v_mode);
        }
    }

    Win32Display::~Win32Display()
    {}

    std::string Win32Display::GetName() const
    {
        return this->device_description;
    }

    std::vector<VideoMode> Win32Display::GetVideoModes() const
    {
        return this->video_modes;
    }

    VideoMode Win32Display::GetCurrentVideoMode() const
    {
        return this->video_mode;
    }

    WindowPosition Win32Display::GetPosition() const
    {
        return this->position;
    }

    float Win32Display::GetScaleFactor() const
    {
        return static_cast<float>(this->dpi) / USER_DEFAULT_SCREEN_DPI;
    }

    void Win32Display::SetVideoMode(std::uint32_t index)
    {
        auto res = ChangeDisplaySettingsExW(this->device_name.data(),
                                            &this->dev_modes[index],
                                            nullptr,
                                            CDS_FULLSCREEN,
                                            nullptr);

        if(res != DISP_CHANGE_SUCCESSFUL)
            throw Core::DisplayException(res);
    }

    Core::WindowSubsystemConnection* Win32Display::GetParent() const noexcept
    {
        return this->parent;
    }

    DisplayChangesFlags Win32Display::Update()
    {
        DisplayChangesFlags flags = 0;
        auto new_dpi = GetDisplayDPI(this->parent, this->handle);
        if(new_dpi != this->dpi)
        {
            this->dpi = new_dpi;
            flags |= DisplayChangesFlagBits::ScaleFactor;
        }

        auto new_video_mode = GetDisplayVideoMode(this->handle, this->device_name.data());
        if(new_video_mode.bits_per_pixel != this->video_mode.bits_per_pixel ||
           new_video_mode.width != this->video_mode.width ||
           new_video_mode.height != this->video_mode.height ||
           new_video_mode.refresh_rate != this->video_mode.refresh_rate)
        {
            this->video_mode = new_video_mode;
            flags |= DisplayChangesFlagBits::VideoMode;
        }

        auto new_position = GetDisplayPosition(this->handle);
        if(new_position.x != this->position.x || new_position.y != this->position.y)
        {
            this->position = new_position;
            flags |= DisplayChangesFlagBits::Position;
        }

        return flags;
    }

    HMONITOR Win32Display::GetHandle() const noexcept
    {
        return this->handle;
    }

    const wchar_t* Win32Display::GetDeviceName() const noexcept
    {
        return this->device_name.data();
    }
};