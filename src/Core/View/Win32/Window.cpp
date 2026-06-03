#include "Window.h"
#include "WindowSubsystem.h"
#include "Core/Utils/ScopedCall.hpp"
#include <windowsx.h>
#include "Core/Utils/Unicode.h"

namespace Core
{
    namespace Win32
    {
        struct WindowCreateData
        {
            Window* obj;
        };

        static MouseButtonFlags GetMouseButtonsFlags(WPARAM w_param) noexcept
        {
            MouseButtonFlags buttons = 0;
            if(w_param & MK_LBUTTON)
                buttons |= MouseButtonFlagBits::LeftButton;
            if(w_param & MK_MBUTTON)
                buttons |= MouseButtonFlagBits::MiddleButton;
            if(w_param & MK_RBUTTON)
                buttons |= MouseButtonFlagBits::RightButton;
            if(w_param & MK_XBUTTON1)
                buttons |= MouseButtonFlagBits::X1Button;
            if(w_param & MK_XBUTTON2)
                buttons |= MouseButtonFlagBits::X2Button;

            return buttons;
        }

        static WindowPosition GetRelativeCursorPosition(LPARAM l_param) noexcept
        {
            return WindowPosition{.x = GET_X_LPARAM(l_param), .y = GET_Y_LPARAM(l_param)};
        }

        static WindowPosition GetRelativeTranslatedCursorPosition(HWND handle, LPARAM l_param)
        {
            POINT point = {.x = GET_X_LPARAM(l_param), .y = GET_Y_LPARAM(l_param)};
            if(ScreenToClient(handle, &point) == 0)
                Core::System::ThrowLastError();

            return WindowPosition{.x = point.x, .y = point.y};
        }

        LRESULT CALLBACK Window::Win32WindowProc(HWND handle,
                                                 UINT message,
                                                 WPARAM w_param,
                                                 LPARAM l_param)
        {
            if(message == WM_NCCREATE)
            {
                WindowCreateData* data = static_cast<WindowCreateData*>(
                    reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                if(data->obj->parent->GetDPIAwrenessType() ==
                       DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE &&
                   data->obj->parent->GetPublicFunctions().EnableNonClientDpiScaling)
                {
                    if(data->obj->parent->GetPublicFunctions().EnableNonClientDpiScaling(handle) ==
                       0)
                        return -1;
                }

                return DefWindowProcW(handle, message, w_param, l_param);
            }
            else if(message == WM_CREATE) //handle create message
            {
                WindowCreateData* data = static_cast<WindowCreateData*>(
                    reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                Core::System::SetLastError(ERROR_SUCCESS);
                auto res =
                    SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data->obj));
                if(res == 0) //possible error
                {
                    if(Core::System::GetLastError() != ERROR_SUCCESS) //error
                        return -1;
                }
            }
            else //handle other messages
            {
                Window* window =
                    reinterpret_cast<Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

                if(!window) //not our window
                    return DefWindowProcW(handle, message, w_param, l_param);

                WindowSubsystem* win_sys = static_cast<WindowSubsystem*>(window->GetParent());

                switch(message)
                {
                    case WM_CLOSE:

                        win_sys->PushEvent(
                            Event{.data = WindowClosedEvent{.timestamp_ms = GetEventTimestamp()},
                                  .handle = window});
                        break;
                    case WM_DISPLAYCHANGE:
                    {
                        if(window->handle) //prevent WM_CREATE
                            window->UpdatePrevWindowedState();

                        win_sys->HandleDisplayChange(false);

                        HMONITOR new_monitor_handle =
                            MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
                        if(window->display.get() != nullptr &&
                           new_monitor_handle !=
                               window->display.get()->GetHandle()) //update display
                        {
                            window->display =
                                win_sys->GetDisplayByMonitorHandle(new_monitor_handle);

                            win_sys->PushEvent(
                                Event{.data = WindowDisplayChangedEvent{.timestamp_ms =
                                                                            GetEventTimestamp(),
                                                                        .display = window->display},
                                      .handle = window});
                        }
                    }
                    break;
                    case WM_DPICHANGED:
                    {
                        HMONITOR new_monitor_handle =
                            MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
                        if(new_monitor_handle !=
                           window->display.get()->GetHandle()) //update display
                        {
                            window->display =
                                win_sys->GetDisplayByMonitorHandle(new_monitor_handle);

                            win_sys->PushEvent(
                                Event{.data = WindowDisplayChangedEvent{.timestamp_ms =
                                                                            GetEventTimestamp(),
                                                                        .display = window->display},
                                      .handle = window});
                        }

                        auto flags = window->display->Update();
                        if(flags & DisplayChangesFlagBits::Position)
                        {
                            win_sys->PushEvent(
                                Event{.data = DisplayMovedEvent{.timestamp_ms = GetEventTimestamp(),
                                                                .position =
                                                                    window->display->GetPosition()},
                                      .handle = window->display});
                        }
                        else if(flags & DisplayChangesFlagBits::VideoMode)
                        {
                            win_sys->PushEvent(
                                Event{.data =
                                          DisplayVideoModeChangedEvent{
                                              .timestamp_ms = GetEventTimestamp(),
                                              .video_mode = window->display->GetCurrentVideoMode()},
                                      .handle = window->display});
                        }
                        else if(flags & DisplayChangesFlagBits::ScaleFactor)
                        {
                            win_sys->PushEvent(
                                Event{.data =
                                          DisplayScaleChangedEvent{
                                              .timestamp_ms = GetEventTimestamp(),
                                              .scale_factor = window->display->GetScaleFactor()},
                                      .handle = window->display});
                        }

                        /*auto awareness_type = static_cast<WindowSubsystem*>(window->GetParent())
                                                  ->GetDPIAwrenessType();

                        if(awareness_type == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ||
                           awareness_type == DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
                        {
                            window->display.reset(new Display(
                                window,
                                MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST)));

                            win_sys->PushEvent(
                                Event{.data = WindowDisplayChangedEvent{.timestamp_ms =
                                                                            GetEventTimestamp()},
                                      .handle = window});

                            RECT* rect = reinterpret_cast<RECT*>(l_param);

                            SetWindowPos(handle,
                                         NULL,
                                         rect->left,
                                         rect->top,
                                         rect->right - rect->left,
                                         rect->bottom - rect->top,
                                         SWP_NOZORDER | SWP_NOACTIVATE);

                            window->UpdatePrevWindowedState();
                        }*/
                    }
                    break;
                    case WM_MOVE:
                    {
                        win_sys->PushEvent(Event{
                            .data =
                                WindowMovedEvent{.timestamp_ms = GetEventTimestamp(),
                                                 .position = WindowPosition{.x = LOWORD(l_param),
                                                                            .y = HIWORD(l_param)}},
                            .handle = window});
                    }
                    break;
                    case WM_SIZE:
                    {
                        WindowResolution resolution = {.width = LOWORD(l_param),
                                                       .height = HIWORD(l_param)};

                        if(w_param == SIZE_MAXIMIZED)
                        {
                            win_sys->PushEvent(Event{
                                .data = WindowMaximizedEvent{.timestamp_ms = GetEventTimestamp(),
                                                             .resolution = resolution},
                                .handle = window});
                        }
                        else if(w_param == SIZE_MINIMIZED)
                        {
                            win_sys->PushEvent(Event{
                                .data = WindowMinimizedEvent{.timestamp_ms = GetEventTimestamp(),
                                                             .resolution = resolution},
                                .handle = window});
                        }
                        else
                        {
                            win_sys->PushEvent(Event{
                                .data = WindowResizedEvent{.timestamp_ms = GetEventTimestamp(),
                                                           .resolution = resolution},
                                .handle = window});
                        }
                    }
                    break;
                    case WM_SHOWWINDOW:
                    {
                        if(w_param == TRUE)
                        {
                            window->current_visibility = WindowVisibility::Shown;

                            win_sys->PushEvent(
                                Event{.data = WindowShownEvent{.timestamp_ms = GetEventTimestamp()},
                                      .handle = window});
                        }
                        else if(w_param == FALSE)
                        {
                            window->current_visibility = WindowVisibility::Hidden;

                            win_sys->PushEvent(Event{
                                .data = WindowHiddenEvent{.timestamp_ms = GetEventTimestamp()},
                                .handle = window});
                        }
                    }
                    break;
                    case WM_MOUSEMOVE:
                    {
                        MouseButtonFlags buttons =
                            GetMouseButtonsFlags(GET_KEYSTATE_WPARAM(w_param));

                        if(window->mouse_focused == false /*&& w_param != 0*/) //enter
                        {
                            TRACKMOUSEEVENT track_info = {.cbSize = sizeof(TRACKMOUSEEVENT),
                                                          .dwFlags = TME_LEAVE,
                                                          .hwndTrack = handle,
                                                          .dwHoverTime = 0};
                            if(TrackMouseEvent(&track_info) == 0)
                                Core::System::ThrowLastError();

                            window->mouse_focused = true;

                            win_sys->PushEvent(Event{
                                .data =
                                    WindowCursorFocusGainEvent{
                                        .timestamp_ms = GetEventTimestamp(),
                                        .buttons = buttons,
                                        .cursor_position = GetRelativeCursorPosition(l_param)},
                                .handle = window});
                        }
                        else if(message == WM_MOUSEMOVE) //common move
                        {
                            win_sys->PushEvent(Event{
                                .data =
                                    MouseCursorMoveEvent{.timestamp_ms = GetEventTimestamp(),
                                                         .buttons = buttons,
                                                         .cursor_position =
                                                             GetRelativeCursorPosition(l_param)},
                                .handle = window});
                        }
                    }
                    break;
                    case WM_MOUSELEAVE:
                    {
                        window->mouse_focused = false;

                        win_sys->PushEvent(
                            Event{.data = WindowCursorFocusLeaveEvent{.timestamp_ms =
                                                                          GetEventTimestamp()},
                                  .handle = window});
                    }
                    break;
                    case WM_SETFOCUS:
                    {
                        win_sys->PushEvent(
                            Event{.data = WindowKeyboardFocusGainEvent{.timestamp_ms =
                                                                           GetEventTimestamp()},
                                  .handle = window});
                    }
                    break;
                    case WM_KILLFOCUS:
                    {
                        win_sys->PushEvent(
                            Event{.data = WindowKeyboardFocusLeaveEvent{.timestamp_ms =
                                                                            GetEventTimestamp()},
                                  .handle = window});
                    }
                    break;
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONDBLCLK:
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONDBLCLK:
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONDBLCLK:
                    case WM_XBUTTONDOWN:
                    case WM_XBUTTONDBLCLK:
                    {
                        MouseButtonFlagBits button;
                        std::uint32_t clicks = 1;
                        switch(message)
                        {
                            case WM_LBUTTONDBLCLK:
                                clicks = 2;
                            case WM_LBUTTONDOWN:
                                button = MouseButtonFlagBits::LeftButton;
                                break;
                            case WM_MBUTTONDBLCLK:
                                clicks = 2;
                            case WM_MBUTTONDOWN:
                                button = MouseButtonFlagBits::MiddleButton;
                                break;
                            case WM_RBUTTONDBLCLK:
                                clicks = 2;
                            case WM_RBUTTONDOWN:
                                button = MouseButtonFlagBits::RightButton;
                                break;
                            case WM_XBUTTONDBLCLK:
                                clicks = 2;
                            case WM_XBUTTONDOWN:
                                if(GET_XBUTTON_WPARAM(w_param) == XBUTTON1)
                                    button = MouseButtonFlagBits::X1Button;
                                else
                                    button = MouseButtonFlagBits::X2Button;
                                break;
                        }

                        win_sys->PushEvent(Event{
                            .data = MouseButtonPressedEvent{.timestamp_ms = GetEventTimestamp(),
                                                            .button = button,
                                                            .clicks = clicks,
                                                            .cursor_position =
                                                                GetRelativeCursorPosition(l_param)},
                            .handle = window});
                    }
                    break;
                    case WM_LBUTTONUP:
                    case WM_MBUTTONUP:
                    case WM_RBUTTONUP:
                    case WM_XBUTTONUP:
                    {
                        MouseButtonFlagBits button;
                        switch(message)
                        {
                            case WM_LBUTTONUP:
                                button = MouseButtonFlagBits::LeftButton;
                                break;
                            case WM_MBUTTONUP:
                                button = MouseButtonFlagBits::MiddleButton;
                                break;
                            case WM_RBUTTONUP:
                                button = MouseButtonFlagBits::RightButton;
                                break;
                            case WM_XBUTTONUP:
                                if(GET_XBUTTON_WPARAM(w_param) == XBUTTON1)
                                    button = MouseButtonFlagBits::X1Button;
                                else
                                    button = MouseButtonFlagBits::X2Button;
                                break;
                        }

                        win_sys->PushEvent(
                            Event{.data =
                                      MouseButtonReleasedEvent{
                                          .timestamp_ms = GetEventTimestamp(),
                                          .button = button,
                                          .cursor_position = GetRelativeCursorPosition(l_param)},
                                  .handle = window});
                    }
                    break;
                    case WM_MOUSEWHEEL:
                    case WM_MOUSEHWHEEL:
                    {
                        float delta =
                            static_cast<float>(GET_WHEEL_DELTA_WPARAM(w_param)) / WHEEL_DELTA;

                        MouseButtonFlags buttons =
                            GetMouseButtonsFlags(GET_KEYSTATE_WPARAM(w_param));

                        win_sys->PushEvent(
                            Event{.data =
                                      MouseWheelEvent{
                                          .timestamp_ms = GetEventTimestamp(),
                                          .buttons = buttons,
                                          .cursor_position =
                                              GetRelativeTranslatedCursorPosition(handle, l_param),
                                          .x_scroll = (message == WM_MOUSEHWHEEL ? delta : 0),
                                          .y_scroll = (message == WM_MOUSEWHEEL ? delta : 0)},
                                  .handle = window});
                    }
                    case WM_ACTIVATEAPP:
                    {
                        if(w_param == TRUE) //we should reset keyboard state
                            window->parent->GetKeyboardState()->Reset();
                    }
                    break;
                    case WM_INPUTLANGCHANGE: //yes. we pass it to DefWindowProc
                    {
                        window->parent->GetKeyboardState()->UpdateCurrentLayout(
                            reinterpret_cast<HKL>(l_param));

                        return DefWindowProcW(handle, message, w_param, l_param);
                    }
                    break;
                    case WM_WINDOWPOSCHANGED:
                    {
                        HMONITOR new_monitor_handle =
                            MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
                        if(window->display.get() != nullptr &&
                           new_monitor_handle !=
                               window->display.get()->GetHandle()) //update display
                        {
                            window->display =
                                win_sys->GetDisplayByMonitorHandle(new_monitor_handle);

                            win_sys->PushEvent(
                                Event{.data = WindowDisplayChangedEvent{.timestamp_ms =
                                                                            GetEventTimestamp(),
                                                                        .display = window->display},
                                      .handle = window});
                        }

                        return DefWindowProcW(handle, message, w_param, l_param);
                    }
                    break;
                    case WM_DROPFILES:
                    {
                        HDROP drop = reinterpret_cast<HDROP>(w_param);

                        std::wstring native_file_buffer(MAX_PATH, L'\0');

                        POINT point = {};
                        DragQueryPoint(drop, &point);

                        auto file_count = DragQueryFileW(drop, 0xFF'FF'FF'FF, nullptr, 0);
                        std::vector<std::filesystem::path> files;
                        files.reserve(file_count);
                        for(std::size_t i = 0; i < file_count; i++)
                        {
                            auto size = DragQueryFileW(drop, i, nullptr, 0);
                            if(size > native_file_buffer.size())
                                native_file_buffer.resize(size);

                            DragQueryFileW(drop, i, native_file_buffer.data(), size);
                            files.push_back(std::filesystem::path(
                                std::wstring_view{native_file_buffer.data(), size}));
                        }

                        DragFinish(drop);

                        win_sys->PushEvent(Event{
                            .data =
                                DragAndDropEvent{.timestamp_ms = GetEventTimestamp(),
                                                 .files = std::move(files),
                                                 .mouse_cursor_position =
                                                     WindowPosition{.x = point.x, .y = point.y}},
                            .handle = window});
                    }
                    break;
                    default:
                        return DefWindowProcW(handle, message, w_param, l_param);
                        break;
                }
            }

            return 0;
        }

        Window::Window(WindowSubsystem* _parent, const WindowInfo& info)
            : parent(_parent),
              handle(nullptr),
              current_state(info.state),
              current_visibility(WindowVisibility::Hidden),
              drag_and_drop_enabled(false),
              mouse_focused(false)
        {
            WindowCreateData data = {.obj = this};

            DWORD ex_style = 0;
            DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

            RECT rect = {.left = 0,
                         .top = 0,
                         .right = static_cast<LONG>(info.resolution.width),
                         .bottom = static_cast<LONG>(info.resolution.height)};
            if(AdjustWindowRectEx(&rect, style & ~WS_OVERLAPPED, style & WS_SYSMENU, ex_style) == 0)
                Core::System::ThrowLastError();

            std::u16string title(Core::UTF8ToUTF16(info.title));
            handle = CreateWindowExW(ex_style,
                                     WIN32_WINDOW_CLASS_NAME,
                                     reinterpret_cast<const wchar_t*>(title.data()),
                                     style,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     rect.right - rect.left,
                                     rect.bottom - rect.top,
                                     nullptr,
                                     nullptr,
                                     parent->GetInstance(),
                                     &data);

            if(handle == nullptr)
                Core::System::ThrowLastError();

            Core::ScopedCall cleanup(
                [this]()
                {
                    DestroyWindow(handle);
                });

            display = parent->GetDisplayByMonitorHandle(
                MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST));

            parent->PushEvent(
                Event{.data = WindowDisplayChangedEvent{.timestamp_ms = GetEventTimestamp(),
                                                        .display = display},
                      .handle = this});

            SetState(info.state);
            if(info.state == WindowState::Windowed)
                UpdatePrevWindowedState();

            cleanup.Drop();
        }

        Window::~Window()
        {
            DestroyWindow(handle);
        }

        void Window::SetTitle(std::string_view title)
        {
            std::u16string wtitle(Core::UTF8ToUTF16(title));
            if(SetWindowTextW(handle, reinterpret_cast<const wchar_t*>(wtitle.data())) == 0)
                Core::System::ThrowLastError();
        }

        std::string Window::GetTitle() const
        {
            Core::System::SetLastError(ERROR_SUCCESS); //clear last error
            int length = GetWindowTextLengthW(handle);
            if(length == 0)
            {
                auto last_error = Core::System::GetLastError();
                if(last_error == ERROR_SUCCESS) //empty title
                    return "";

                Core::System::ThrowLastError();
            }

            std::u16string wstr(length, L'\0');
            if(GetWindowTextW(handle, reinterpret_cast<wchar_t*>(wstr.data()), length + 1) == 0)
                Core::System::ThrowLastError();

            return Core::UTF16ToUTF8(wstr);
        }

        void Window::Resize(const WindowResolution& resolution)
        {
            if(current_state == WindowState::FullScreen)
                return;

            RECT rect = {.left = 0,
                         .top = 0,
                         .right = static_cast<LONG>(resolution.width),
                         .bottom = static_cast<LONG>(resolution.height)};

            Core::System::SetLastError(ERROR_SUCCESS);
            auto style = GetWindowLongPtrW(handle, GWL_STYLE);
            if(style == 0)
            {
                if(Core::System::GetLastError() != ERROR_SUCCESS)
                    Core::System::ThrowLastError();
            }

            auto ex_style = GetWindowLongPtrW(handle, GWL_EXSTYLE);
            if(ex_style == 0)
            {
                if(Core::System::GetLastError() != ERROR_SUCCESS)
                    Core::System::ThrowLastError();
            }

            if(AdjustWindowRectEx(&rect, style & ~WS_OVERLAPPED, style & WS_SYSMENU, ex_style) == 0)
                Core::System::ThrowLastError();

            //ignore X and Y
            if(SetWindowPos(handle,
                            nullptr,
                            0,
                            0,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOZORDER |
                                SWP_NOACTIVATE) == 0)
            {
                Core::System::ThrowLastError();
            }

            UpdatePrevWindowedState();
        }

        WindowResolution Window::GetResolution() const
        {
            RECT rect;
            if(GetClientRect(handle, &rect) == 0)
                Core::System::ThrowLastError();

            return WindowResolution{.width = static_cast<std::uint32_t>(rect.right),
                                    .height = static_cast<std::uint32_t>(rect.bottom)};
        }

        void Window::SetState(WindowState state)
        {
            if(current_state == state)
                return;

            if(state == WindowState::FullScreen)
            {
                UpdatePrevWindowedState();

                //get monitor rect(rcMonitor)
                auto video_mode = display->GetCurrentVideoMode();
                auto display_positon = display->GetPosition();

                //hide title bar and other gui
                Core::System::SetLastError(ERROR_SUCCESS);
                auto style = GetWindowLongPtrW(handle, GWL_STYLE);
                if(style == 0)
                {
                    if(Core::System::GetLastError() != ERROR_SUCCESS)
                        Core::System::ThrowLastError();
                }

                if(SetWindowLongPtrW(handle,
                                     GWL_STYLE,
                                     (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP) == 0)
                {
                    if(Core::System::GetLastError() != ERROR_SUCCESS)
                        Core::System::ThrowLastError();
                }

                //apply rect + make topmost
                if(SetWindowPos(handle,
                                HWND_TOPMOST,
                                display_positon.x,
                                display_positon.y,
                                video_mode.width,
                                video_mode.height,
                                SWP_NOREDRAW) == 0)
                    Core::System::ThrowLastError();
            }
            else
            {
                //restore style
                Core::System::SetLastError(ERROR_SUCCESS);
                auto style = GetWindowLongPtrW(handle, GWL_STYLE);
                if(style == 0)
                {
                    if(Core::System::GetLastError() != ERROR_SUCCESS)
                        Core::System::ThrowLastError();
                }

                if(SetWindowLongPtrW(handle,
                                     GWL_STYLE,
                                     (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW) == 0)
                {
                    if(Core::System::GetLastError() != ERROR_SUCCESS)
                        Core::System::ThrowLastError();
                }

                //restore to prev windowed position and resolution
                if(SetWindowPos(handle,
                                HWND_NOTOPMOST,
                                prev_windowed_rect.left,
                                prev_windowed_rect.top,
                                prev_windowed_rect.right - prev_windowed_rect.left,
                                prev_windowed_rect.bottom - prev_windowed_rect.top,
                                SWP_NOREDRAW | SWP_NOACTIVATE) == 0)
                    Core::System::ThrowLastError();
            }

            current_state = state;
        }

        WindowState Window::GetState() const
        {
            return current_state;
        }

        void Window::SetMouseCursorPosition(const WindowPosition& pos)
        {
            POINT point = {.x = pos.x, .y = pos.y};
            if(ClientToScreen(handle, &point) == 0)
                Core::System::ThrowLastError();

            if(SetCursorPos(point.x, point.y) == 0)
                Core::System::ThrowLastError();
        }

        WindowPosition Window::GetMouseCursorPosition() const
        {
            POINT point = {};
            if(GetCursorPos(&point) == 0)
                Core::System::ThrowLastError();

            if(ScreenToClient(handle, &point) == 0)
                Core::System::ThrowLastError();

            return WindowPosition{.x = point.x, .y = point.y};
        }

        void Window::SetVisibility(WindowVisibility visibility)
        {
            /*Controls how the window is to be shown.This parameter is ignored the first time an
                application calls ShowWindow,
                if the program that launched the application provides a STARTUPINFO
                    structure.Otherwise,
                the first time ShowWindow is called,
                the value should be the value obtained by the WinMain function in its nCmdShow
                    parameter.In subsequent calls,
                this parameter can be one of the following values.*/

            static bool is_first_call = true;
            if(is_first_call)
            {
                ShowWindow(handle, Core::System::GetCmdShow());
                is_first_call = false;
            }

            int cmd = (visibility == WindowVisibility::Hidden ? SW_HIDE : SW_SHOW);
            ShowWindow(handle, cmd);
        }

        WindowVisibility Window::GetVisibility() const
        {
            return current_visibility;
        }

        void Window::SetDragAndDropState(bool enabled)
        {
            if(enabled == drag_and_drop_enabled)
                return;

            DragAcceptFiles(handle, (enabled ? TRUE : FALSE));

            drag_and_drop_enabled = enabled;
        }

        bool Window::GetDragAndDropState() const
        {
            return drag_and_drop_enabled;
        }

        WindowSurfaceInfo Window::GetWindowSurfaceInfo() const noexcept
        {
            return Render::Win32SurfaceInfo{.instance = parent->GetInstance(), .window = handle};
        }

        std::shared_ptr<Core::Display> Window::GetDisplay() const noexcept
        {
            return display;
        }

        Core::WindowSubsystem* Window::GetParent() const noexcept
        {
            return parent;
        }

        void Window::UpdatePrevWindowedState()
        {
            if(current_state == WindowState::Windowed)
            {
                if(GetWindowRect(handle, &prev_windowed_rect) == 0)
                    Core::System::ThrowLastError();
            }
            else //use current monitor rcWork
            {
                MONITORINFOEXW info = {MONITORINFO{.cbSize = sizeof(MONITORINFOEXW)}};
                if(GetMonitorInfoW(MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST), &info) == 0)
                    Core::System::ThrowLastError();

                prev_windowed_rect = info.rcWork;
            }
        }
    };
};

/*
Init -> set prev as current
Display change -> set prev as current
DPI change -> set prev as current
Move -> set prev as current
Resize -> set prev as current
Go fullscreen -> save window
*/

//#error WM_DPICHANGED -> WE SHOULD SAVE PREV WINDOWED STATE AFTER SetWindowPos!!! -> (set flag and check it and then unset it)