#pragma once

#include <queue>
#include <ShellScalingApi.h>
#include "Core/Utils/DynamicLibrary.h"
#include "../WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        struct QueueEvent
        {
            union
            {
                WindowSubsystemQuitEvent window_subsystem_quit;
                WindowClosedEvent window_closed;
                WindowDisplayChangedEvent window_display_changed;
                WindowMovedEvent window_moved;
                WindowResizedEvent window_resized;
                WindowMinimizedEvent window_minimized;
                WindowMaximizedEvent window_maximized;
                WindowHiddenEvent window_hidden;
                WindowShownEvent window_shown;
                WindowCursorFocusGainEvent window_cursor_focus_gain;
                WindowCursorFocusLeaveEvent window_cursor_focus_leave;
                WindowKeyboardFocusGainEvent window_keyboard_focus_gain;
                WindowKeyboardFocusLeaveEvent window_keyboard_focus_leave;
                MouseButtonPressedEvent mouse_button_pressed;
                MouseButtonReleasedEvent mouse_buttton_released;
                MouseCursorMoveEvent mouse_cursor_move;
                MouseWheelEvent mouse_wheel;
            } data;

            ClassIDBase::ClassIDType id;
            Window* window;
            void (Window::*emitter)(ClassIDBase::ClassIDType id, const void* event);
        };

        class CORE_API WindowSubsystem final : public Core::WindowSubsystem,
                                               Core::NonCopyable,
                                               Core::NonMovable
        {
        public:
            WindowSubsystem();

            virtual ~WindowSubsystem() override;

            virtual void PollEvents() override;

            virtual WindowSubsystemType GetType() const noexcept override;

            virtual Core::Window* CreateWindow(const WindowInfo& info) override;

            virtual CursorState GetCursorState() const override;
            virtual void SetCursorState(CursorState state) override;

            HINSTANCE GetInstance() const noexcept;

            PROCESS_DPI_AWARENESS GetDPIAwrenessType() const noexcept;

            void PushEvent(QueueEvent&& event);
        private:
            HINSTANCE instance;

            DynamicLibrary user32;
            DynamicLibrary shcore;

            HRESULT (*SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
            PROCESS_DPI_AWARENESS dpi_awareness;

            std::queue<QueueEvent> events;
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