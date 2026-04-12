#pragma once

#include <queue>
#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"

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

        struct KeyboardLayoutDesc
        {
            HKL layout;
            std::unordered_map<ScanCode, std::string>
                scancode_to_string_mapping; //We can create table to map PS/2 Set 1 into the ours inner scancodes, but I don't want to do this...
            std::unordered_map<std::string_view /*reference to scancode_to_string_mapping*/,
                               ScanCode>
                string_to_scancode_mapping;
        };

        class CORE_API WindowSubsystem final : public Core::WindowSubsystem, Core::NonMovable
        {
            static LRESULT CALLBACK ShellProc(int code, WPARAM w_param, LPARAM l_param);
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

#pragma message("TO CORE::SYSTEM AS PART OF MAIN FUCNTION PARAMETER")
            HINSTANCE GetInstance() const noexcept;

            PROCESS_DPI_AWARENESS GetDPIAwrenessType() const noexcept;

            void PushEvent(Event&& event);
        private:
            bool UpdateKeyboardLayouts(); //returns 'false' if there is only language change
        private:
            HINSTANCE instance;

            DynamicLibrary user32;
            DynamicLibrary shcore;

            HRESULT (*SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
            PROCESS_DPI_AWARENESS dpi_awareness;
            HHOOK shell_hook;
            std::vector<KeyboardLayoutDesc> keyboard_layouts;

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
        };
    };
};