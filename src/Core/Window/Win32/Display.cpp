#include "Display.h"
#include "Window.h"
#include "WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        struct DevModeKey
        {
            std::uint32_t width;
            std::uint32_t height;
            std::uint32_t refresh_rate;
            std::uint32_t bits_per_pixel;

            constexpr auto operator<=>(const DevModeKey&) const noexcept = default;
        };

        Display::Display(Window* _parent, HMONITOR _handle)
            : parent(_parent),
              handle(_handle)
        {
            MONITORINFOEXW info = {MONITORINFO{.cbSize = sizeof(MONITORINFOEXW)}};
            if(GetMonitorInfoW(handle, &info) == 0)
                Core::System::ThrowLastError();

            std::copy(info.szDevice, info.szDevice + CCHDEVICENAME, this->device_name.data());

            WindowSubsystem* win_sys = static_cast<WindowSubsystem*>(parent->GetParent());
            if(win_sys->QueryDisplayConfig)
            {
                UINT32 flags = QDC_ONLY_ACTIVE_PATHS;
                LONG result = ERROR_SUCCESS;
                std::vector<DISPLAYCONFIG_PATH_INFO> active_path_infos;
                std::vector<DISPLAYCONFIG_MODE_INFO> active_mode_infos;
                do
                {
                    UINT32 active_path_count;
                    UINT32 active_mode_count;
                    result = win_sys->GetDisplayConfigBufferSizes(flags,
                                                                  &active_path_count,
                                                                  &active_mode_count);

                    if(result != ERROR_SUCCESS)
                        throw Win32Exception(result);

                    active_path_infos.resize(active_path_count);
                    active_mode_infos.resize(active_mode_count);

                    result = win_sys->QueryDisplayConfig(flags,
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

                    result = win_sys->DisplayConfigGetDeviceInfo(&target_device_name_info.header);
                    if(result != ERROR_SUCCESS)
                        throw Win32Exception(result);

                    DISPLAYCONFIG_SOURCE_DEVICE_NAME source_device_name_info = {
                        DISPLAYCONFIG_DEVICE_INFO_HEADER{
                            .type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                            .size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME),
                            .adapterId = path.sourceInfo.adapterId,
                            .id = path.sourceInfo.id}};

                    result = win_sys->DisplayConfigGetDeviceInfo(&source_device_name_info.header);
                    if(result != ERROR_SUCCESS)
                        throw Win32Exception(result);

                    if(std::wcscmp(source_device_name_info.viewGdiDeviceName, device_name.data()) ==
                       0)
                    {
                        device_description = Core::System::WideToUTF8(
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
                    device_description = Core::System::WideToUTF8(
                        {display_device.DeviceString, std::wcslen(display_device.DeviceString)});
                }
            }

            std::map<DevModeKey, DEVMODEW> dev_modes_map;
            DEVMODEW dev_mode = {.dmSize = sizeof(DEVMODEW), .dmDriverExtra = 0};
            DWORD dev_mode_count = 0;
            while(EnumDisplaySettingsExW(device_name.data(), dev_mode_count, &dev_mode, 0) != 0)
            {
                dev_mode_count++;

                if(!((dev_mode.dmFields & DM_PELSWIDTH) && (dev_mode.dmFields & DM_PELSHEIGHT) &&
                     (dev_mode.dmFields & DM_DISPLAYFREQUENCY) &&
                     (dev_mode.dmFields & DM_BITSPERPEL)))
                    continue;

                auto [it, inserted] =
                    dev_modes_map.insert({DevModeKey{.width = dev_mode.dmPelsWidth,
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

            dev_modes.reserve(dev_modes_map.size());
            for(const auto& [_, dev_mode]: dev_modes_map)
                dev_modes.push_back(dev_mode);
        }

        Display::~Display()
        {}

        std::string Display::GetName() const
        {
            return device_description;
        }

        std::vector<VideoMode> Display::GetVideoModes() const
        {
            std::vector<VideoMode> video_modes;
            video_modes.reserve(dev_modes.size());

            for(std::size_t i = 0; i < dev_modes.size(); i++)
            {
                const DEVMODEW& dev_mode = dev_modes[i];
                video_modes.push_back(VideoMode{.width = dev_mode.dmPelsWidth,
                                                .height = dev_mode.dmPelsHeight,
                                                .refresh_rate = dev_mode.dmDisplayFrequency,
                                                .bits_per_pixel = dev_mode.dmBitsPerPel});
            }

            return video_modes;
        }

        VideoMode Display::GetCurrentVideoMode() const
        {
            DEVMODEW dev_mode = {.dmSize = sizeof(DEVMODEW), .dmDriverExtra = 0};
            if(EnumDisplaySettingsExW(device_name.data(), ENUM_CURRENT_SETTINGS, &dev_mode, 0) == 0)
                Core::System::ThrowLastError();

            return VideoMode{.width = dev_mode.dmPelsWidth,
                             .height = dev_mode.dmPelsHeight,
                             .refresh_rate = dev_mode.dmDisplayFrequency,
                             .bits_per_pixel = dev_mode.dmBitsPerPel};
        }

        float Display::GetScaleFactor() const
        {
            float dpi = USER_DEFAULT_SCREEN_DPI;

            auto subsystem = static_cast<Core::Win32::WindowSubsystem*>(parent->GetParent());
            auto awareness = subsystem->GetDPIAwrenessType();
            if(awareness == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ||
               awareness == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
            {
                UINT dpi_x;
                UINT dpi_y;
                auto res = subsystem->GetDpiForMonitor(handle,
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

            return dpi / USER_DEFAULT_SCREEN_DPI;
        }

        float Display::GetDisplayScaleFactor() const
        {
            return 1.0f;
        }

        void Display::SetVideoMode(std::uint32_t index)
        {
            auto res = ChangeDisplaySettingsExW(device_name.data(),
                                                &dev_modes[index],
                                                nullptr,
                                                CDS_FULLSCREEN,
                                                nullptr);

            if(res != DISP_CHANGE_SUCCESSFUL)
                throw Core::DisplayException(res);
        }

        WindowPosition Display::GetPosition() const
        {
            MONITORINFOEXW info = {MONITORINFO{.cbSize = sizeof(MONITORINFOEXW)}};
            if(GetMonitorInfoW(handle, &info) == 0)
                Core::System::ThrowLastError();

            return WindowPosition{.x = info.rcMonitor.left, .y = info.rcMonitor.top};
        }

        Core::Window* Display::GetParent() const noexcept
        {
            return parent;
        }
    };
};