#pragma once

#include <queue>
#include <set>
#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        class Window;

        enum class StableScanCode : ScanCode
        {
            BackSpace = 0x0E,
            Tab = 0x0F,
            CapsLock = 0x3A,
            Enter = 0x1C,
            LeftShift = 0x2A,
            ABTN_C1 = 0x73,
            RightShift = 0x36,
            LeftControl = 0x1D,
            LeftAlt = 0x38,
            Space = 0x39,
            RightAlt = 0xE0'38,
            RightControl = 0xE0'1D,
            Insert = 0xE0'52,
            Delete = 0xE0'53,
            LeftArrow = 0xE0'4B,
            Home = 0xE0'47,
            End = 0xE0'4F,
            UpArrow = 0xE0'48,
            DownArrow = 0xE0'50,
            PageUp = 0xE0'49,
            PageDown = 0xE0'51,
            RightArrow = 0xE0'4D,
            NumLock = 0x45,
            Num7 = 0x47,
            Num4 = 0x4B,
            Num1 = 0x4F,
            NumDiv = 0xE0'35,
            Num8 = 0x48,
            Num5 = 0x4C,
            Num2 = 0x50,
            Num0 = 0x52,
            NumMul = 0x37,
            Num9 = 0x49,
            Num6 = 0x4D,
            Num3 = 0x51,
            NumPeriod = 0x53,
            NumMin = 0x4A,
            NumAdd = 0x4E,
            ABTN_C2 = 0x7E,
            NumEnter = 0xE0'1C,
            Esc = 0x01,
            F1 = 0x3B,
            F2 = 0x3C,
            F3 = 0x3D,
            F4 = 0x3E,
            F5 = 0x3F,
            F6 = 0x40,
            F7 = 0x41,
            F8 = 0x42,
            F9 = 0x43,
            F10 = 0x44,
            F11 = 0x57,
            F12 = 0x58,
            PrintScreen = 0xE0'2A'E0'37,
            ScrollLock = 0x46,
            Pause = 0xE1'1D'45,
            LeftMeta = 0xE0'5B,
            RightMeta = 0xE0'5C,
            Menu = 0xE0'5D,
            Power = 0xE0'5E,
            Sleep = 0xE0'5F,
            Wake = 0xE0'63,
            Kana = 0x70,
            SBCSCHAR = 0x77, //???
            Convert = 0x79,
            NonConvert = 0x7B
#pragma message("ADD PTHER KEYS FROM USB HID TABLE")
        };

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