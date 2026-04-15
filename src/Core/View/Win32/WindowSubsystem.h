#pragma once

#include <queue>
#include <set>
#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"
#include "Core/View/WindowEvents.h"

namespace Core
{
    namespace Win32
    {
        class Window;

        struct Event
        {
            QueueEvent data;
            ClassIDBase::ClassIDType id;
            Window* window;
        };

        struct KeyboardState
        {
            HKL current_layout;
            std::set<HKL> layouts;
            std::unordered_map<ScanCode, std::string> scancode_to_string_mapping;
            std::unordered_map<std::string_view /*reference to scancode_to_string_mapping*/,
                               ScanCode>
                string_to_scancode_mapping;
        };

        class CORE_API WindowSubsystem final : public Core::WindowSubsystem, Core::NonMovable
        {
            static LRESULT CALLBACK ShellProc(int code, WPARAM w_param, LPARAM l_param);
            static LRESULT CALLBACK RawInputWindowProc(HWND handle,
                                                       UINT message,
                                                       WPARAM w_param,
                                                       LPARAM l_param);

            enum class PrecededScanCode
            {
                None = 0,
                E02A = 0xE0'2A,
                E11D = 0xE1'1D
            };
        public:
            WindowSubsystem();

            virtual ~WindowSubsystem() override;

            virtual void PollEvents() override;

            virtual WindowSubsystemType GetType() const noexcept override;

            virtual Core::Window* CreateWindow(const WindowInfo& info) override;

            virtual CursorState GetCursorState() const override;
            virtual void SetCursorState(CursorState state) override;

            virtual std::string GetKeyNameByScancode(ScanCode scancode) override;
            virtual std::optional<ScanCode> GetScanCodeFromKeyName(std::string_view name) override;

            HINSTANCE GetInstance() const noexcept;

            DPI_AWARENESS_CONTEXT GetDPIAwrenessType() const noexcept;

            void PushEvent(Event&& event);

            KeyboardState* GetKeyboardState() const noexcept;

            static WindowSubsystem* GetSubsystem() noexcept;
        private:
            bool UpdateKeyboardLayouts(); //returns 'false' if there is only language change
        private:
            HINSTANCE instance;

            DynamicLibrary user32;
            DynamicLibrary shcore;

            BOOL (*SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
            DPI_AWARENESS_CONTEXT (*SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext);
            HRESULT (*SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
            DPI_AWARENESS_CONTEXT dpi_awareness;

            HWND raw_input_hwnd;
            BYTE raw_input_keyboard_state[256];
            PrecededScanCode preceded_scancode;

            HHOOK shell_hook;
            KeyboardState keyboard_state;

            std::queue<Event> events;
        public:
            //let's make it public
            //Windows 8.1+
            const HRESULT (*GetDpiForMonitor)(HMONITOR hmonitor,
                                              MONITOR_DPI_TYPE dpiType,
                                              UINT* dpiX,
                                              UINT* dpiY);

            //Window 7+
            LONG (*GetDisplayConfigBufferSizes)(
                UINT32 flags,
                UINT32* numPathArrayElements,
                UINT32* numModeInfoArrayElements); //also in Vista but do not care

            LONG (*QueryDisplayConfig)(UINT32 flags,
                                       UINT32* numPathArrayElements,
                                       DISPLAYCONFIG_PATH_INFO* pathArray,
                                       UINT32* numModeInfoArrayElements,
                                       DISPLAYCONFIG_MODE_INFO* modeInfoArray,
                                       DISPLAYCONFIG_TOPOLOGY_ID* currentTopologyId);

            LONG (*DisplayConfigGetDeviceInfo)(DISPLAYCONFIG_DEVICE_INFO_HEADER* requestPacket);

            BOOL (*EnableNonClientDpiScaling)(HWND hwnd);
        };
    };
};