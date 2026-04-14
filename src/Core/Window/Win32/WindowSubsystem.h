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
            Enter = 0x1C,
            Escape = 0x01,
            BackSpace = 0x0E,
            Tab = 0x0F,
            Space = 0x39,
            CapsLock = 0x3A,
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
            PrintScreen = 0xE0'37,
            ScrollLock = 0x46,
            Break = 0xE0'46, //also E0 C6
            Pause = 0xE1'1D'45, //also E1 9D C5
            Insert = 0xE0'52,
            Home = 0xE0'47,
            PageUp = 0xE0'49,
            Delete = 0xE0'53,
            End = 0xE0'4F,
            PageDown = 0xE0'51,
            RightArrow = 0xE0'4D,
            LeftArrow = 0xE0'4B,
            DownArrow = 0xE0'50,
            UpArrow = 0xE0'48,
            NumLock = 0x45,
            KeypadDivide = 0xE0'35,
            KeypadMultiply = 0x37,
            KeypadSubtract = 0x4A,
            KeypadAdd = 0x4E,
            KeypadEnter = 0xE0'1C,
            Keypad1 = 0x4F,
            Keypad2 = 0x50,
            Keypad3 = 0x51,
            Keypad4 = 0x4B,
            Keypad5 = 0x4C,
            Keypad6 = 0x4D,
            Keypad7 = 0x47,
            Keypad8 = 0x48,
            Keypad9 = 0x49,
            Keypad0 = 0x52,
            KeypadPeriod = 0x53,
            Menu = 0xE0'5D,
            KeyboardPower = 0xE0'5E,
            KeypadEqual = 0x59,
            F13 = 0x64,
            F14 = 0x65,
            F15 = 0x66,
            F16 = 0x67,
            F17 = 0x68,
            F18 = 0x69,
            F19 = 0x6A,
            F20 = 0x6B,
            F21 = 0x6C,
            F22 = 0x6D,
            F23 = 0x6E,
            F24 = 0x76,
            BrazilKeypad = 0x7E,
            Ro = 0x73,
            Kana = 0x70,
            Yen = 0x7D,
            Henkan = 0x79,
            Muhenkan = 0x7B,
            PC9800KeypadPeriod = 0x5C,
            Hanguel = 0xF2,
            Hanja = 0xF1,
            Katakana = 0x78,
            Hiragana = 0x77,
            ZenkakuHankaku = 0x76,
            LeftControl = 0x1D,
            LeftShift = 0x2A,
            LeftAlt = 0x38,
            LeftGUI = 0xE0'5B,
            RightControl = 0xE0'1D,
            RightShift = 0x36,
            RightAlt = 0xE0'38,
            RightGUI = 0xE0'5C,
            ScanNextTrack = 0xE0'19,
            ScanPreviousTrack = 0xE0'10,
            Stop = 0xE0'24,
            PlayPause = 0xE0'22,
            Mute = 0xE0'20,
            VolumeUp = 0xE0'30,
            VolumeDown = 0xE0'2E,
            MediaSelect = 0xE0'6D,
            Mail = 0xE0'6C,
            Calculator = 0xE0'21,
            MyComputer = 0xE0'6B,
            WWWSearch = 0xE0'65,
            WWWHome = 0xE0'32,
            WWWBack = 0xE0'6A,
            WWWForward = 0xE0'69,
            WWWStop = 0xE0'68,
            WWWRefresh = 0xE0'67,
            WWWFavorites = 0xE0'66
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