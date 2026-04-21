#pragma once

#include <queue>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"
#include "Core/View/WindowEvents.h"
#include "KeyboardState.h"

namespace Core
{
    namespace Win32
    {
        class Window;

        struct Event
        {
            QueueEvent data;
            ClassIDBase::ClassIDType id;
            void* window; //Display, Window or nullptr(WindowSubsystem)
        };

        struct Win32PublicDynamicFunctions
        {
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

        class CORE_API WindowSubsystem final : public Core::WindowSubsystem, Core::NonMovable
        {
        private:
            constexpr static wchar_t WIN32_SERVICE_WINDOW_CLASS_NAME[] =
                L"WIN32_SERVICE_WINDOW_CLASS";

            static LRESULT CALLBACK Win32ServiceWindowProc(HWND handle,
                                                           UINT message,
                                                           WPARAM w_param,
                                                           LPARAM l_param);
        public:
            WindowSubsystem();

            virtual ~WindowSubsystem() override;

            virtual void PollEvents() override;

            virtual WindowSubsystemType GetType() const noexcept override;

            virtual Core::Window* CreateWindow(const WindowInfo& info) override;

            virtual CursorState GetCursorState() const override;
            virtual void SetCursorState(CursorState state) override;

            virtual KeyboardKey GetKeyByScancode(ScanCode scancode) override;
            virtual std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key) override;

            virtual KeyboardAccessState GetKeyboardAccessState() override;
            virtual void SetKeyboardAccessState(KeyboardAccessState state) override;

            const Win32PublicDynamicFunctions& GetPublicFunctions() const noexcept;
            KeyboardState* GetKeyboardState() const noexcept;

            HINSTANCE GetInstance() const noexcept;

            DPI_AWARENESS_CONTEXT GetDPIAwrenessType() const noexcept;

            void PushEvent(Event&& event);

            static WindowSubsystem* GetSubsystem() noexcept;

            void operator delete(void* ptr);
        private:
            static inline DPI_AWARENESS_CONTEXT DPI_AWARENESS = DPI_AWARENESS_CONTEXT_UNAWARE;
            static inline WindowSubsystem* SUBSYSTEM = nullptr;

            HINSTANCE instance;
            DynamicLibrary user32;
            DynamicLibrary shcore;
            Win32PublicDynamicFunctions public_functions;

            ATOM service_window_class_atom;
            HWND service_window_handle;

            KeyboardState* keyboard_state;

            ATOM window_class_atom;

            std::queue<Event> events;
        };
    };
};