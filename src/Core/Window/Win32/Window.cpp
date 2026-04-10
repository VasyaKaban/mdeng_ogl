#include "Window.h"
#include "WindowSubsystem.h"
#include "Core/Utils/ScopedCall.hpp"
#include <windowsx.h>

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

        static ModifierKeyFlags GetModifierFlags() noexcept
        {
            ModifierKeyFlags flags = 0;
            if(GetAsyncKeyState(VK_LSHIFT) < 0)
                flags |= ModifierKeyFlagBits::LeftShift;

            if(GetAsyncKeyState(VK_RSHIFT) < 0)
                flags |= ModifierKeyFlagBits::RightShift;

            if(GetAsyncKeyState(VK_LCONTROL) < 0)
                flags |= ModifierKeyFlagBits::LeftControl;

            if(GetAsyncKeyState(VK_RCONTROL) < 0)
                flags |= ModifierKeyFlagBits::RightControl;

            if(GetAsyncKeyState(VK_LMENU) < 0)
                flags |= ModifierKeyFlagBits::LeftAlt;

            if(GetAsyncKeyState(VK_RMENU) < 0)
                flags |= ModifierKeyFlagBits::RightAlt;

            if(GetAsyncKeyState(VK_LWIN) < 0)
                flags |= ModifierKeyFlagBits::LeftMeta;

            if(GetAsyncKeyState(VK_RWIN) < 0)
                flags |= ModifierKeyFlagBits::RightMeta;

            return flags;
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
            if(handle == nullptr) //skip NULL window
            {
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

                std::uint64_t message_time_ms = GetMessageTime();
                switch(message)
                {
                    case WM_QUIT:
                        win_sys->PushEvent(Event{
                            .data = {.window_subsystem_quit =
                                         WindowSubsystemQuitEvent{.timestamp_ms = message_time_ms}},
                            .id = ClassID<WindowSubsystemQuitEvent>::ID,
                            .window = window});
                        break;
                    case WM_CLOSE:
                        win_sys->PushEvent(
                            Event{.data = {.window_closed =
                                               WindowClosedEvent{.timestamp_ms = message_time_ms}},
                                  .id = ClassID<WindowClosedEvent>::ID,
                                  .window = window});
                        break;
                    case WM_DISPLAYCHANGE:
                        if(window->handle) //prevent WM_CREATE
                            window->UpdatePrevWindowedState();

                        window->display.reset(new Display(
                            window,
                            MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST)));

                        win_sys->PushEvent(Event{.data = {.window_display_changed =
                                                              WindowDisplayChangedEvent{
                                                                  .timestamp_ms = message_time_ms}},
                                                 .id = ClassID<WindowDisplayChangedEvent>::ID,
                                                 .window = window});
                        break;
                    case WM_DPICHANGED:
                        //if(window->handle) //prevent WM_CREATE
                        //    window->UpdatePrevWindowedState();

                        if(static_cast<WindowSubsystem*>(window->GetParent())
                               ->GetDPIAwrenessType() == PROCESS_PER_MONITOR_DPI_AWARE)
                        {
                            window->display.reset(new Display(
                                window,
                                MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST)));

                            win_sys->PushEvent(
                                Event{.data = {.window_display_changed =
                                                   WindowDisplayChangedEvent{.timestamp_ms =
                                                                                 message_time_ms}},
                                      .id = ClassID<WindowDisplayChangedEvent>::ID,
                                      .window = window});

                            RECT* rect = reinterpret_cast<RECT*>(l_param);

                            SetWindowPos(handle,
                                         NULL,
                                         rect->left,
                                         rect->top,
                                         rect->right - rect->left,
                                         rect->bottom - rect->top,
                                         SWP_NOZORDER | SWP_NOACTIVATE);

                            window->UpdatePrevWindowedState();
                        }
                        break;
                    case WM_MOVE:
                    {
                        win_sys->PushEvent(Event{
                            .data = {.window_moved =
                                         WindowMovedEvent{
                                             .timestamp_ms = message_time_ms,
                                             .position = WindowPosition{.x = LOWORD(l_param),
                                                                        .y = HIWORD(l_param)}}},
                            .id = ClassID<WindowMovedEvent>::ID,
                            .window = window});
                    }
                    break;
                    case WM_SIZE:
                    {
                        WindowResolution resolution = {.width = LOWORD(l_param),
                                                       .height = HIWORD(l_param)};

                        if(w_param == SIZE_MAXIMIZED)
                        {
                            win_sys->PushEvent(Event{
                                .data = {.window_maximized =
                                             WindowMaximizedEvent{.timestamp_ms = message_time_ms,
                                                                  .resolution = resolution,
                                                                  .scaled_resolution = resolution}},
                                .id = ClassID<WindowMaximizedEvent>::ID,
                                .window = window});
                        }
                        else if(w_param == SIZE_MINIMIZED)
                        {
                            win_sys->PushEvent(Event{
                                .data = {.window_minimized =
                                             WindowMinimizedEvent{.timestamp_ms = message_time_ms,
                                                                  .resolution = resolution,
                                                                  .scaled_resolution = resolution}},
                                .id = ClassID<WindowMinimizedEvent>::ID,
                                .window = window});
                        }
                        else
                        {
                            win_sys->PushEvent(Event{
                                .data = {.window_resized =
                                             WindowResizedEvent{.timestamp_ms = message_time_ms,
                                                                .resolution = resolution,
                                                                .scaled_resolution = resolution}},
                                .id = ClassID<WindowResizedEvent>::ID,
                                .window = window});
                        }
                    }
                    break;
                    case WM_SHOWWINDOW:
                    {
                        if(w_param == TRUE)
                        {
                            window->current_visibility = WindowVisibility::Shown;

                            win_sys->PushEvent(Event{
                                .data = {.window_shown =
                                             WindowShownEvent{.timestamp_ms = message_time_ms}},
                                .id = ClassID<WindowShownEvent>::ID,
                                .window = window});
                        }
                        else if(w_param == FALSE)
                        {
                            window->current_visibility = WindowVisibility::Hidden;

                            win_sys->PushEvent(Event{
                                .data = {.window_hidden =
                                             WindowHiddenEvent{.timestamp_ms = message_time_ms}},
                                .id = ClassID<WindowHiddenEvent>::ID,
                                .window = window});
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

                            win_sys->PushEvent(
                                Event{.data = {.window_cursor_focus_gain =
                                                   WindowCursorFocusGainEvent{
                                                       .timestamp_ms = message_time_ms,
                                                       .buttons = buttons,
                                                       .cursor_position =
                                                           GetRelativeCursorPosition(l_param)}},
                                      .id = ClassID<WindowCursorFocusGainEvent>::ID,
                                      .window = window});
                        }
                        else if(message == WM_MOUSEMOVE) //common move
                        {
                            win_sys->PushEvent(
                                Event{.data = {.mouse_cursor_move =
                                                   MouseCursorMoveEvent{
                                                       .timestamp_ms = message_time_ms,
                                                       .buttons = buttons,
                                                       .cursor_position =
                                                           GetRelativeCursorPosition(l_param)}},
                                      .id = ClassID<MouseCursorMoveEvent>::ID,
                                      .window = window});
                        }
                    }
                    break;
                    case WM_MOUSELEAVE:
                    {
                        window->mouse_focused = false;

                        win_sys->PushEvent(Event{.data = {.window_cursor_focus_leave =
                                                              WindowCursorFocusLeaveEvent{
                                                                  .timestamp_ms = message_time_ms}},
                                                 .id = ClassID<WindowCursorFocusLeaveEvent>::ID,
                                                 .window = window});
                    }
                    break;
                    case WM_SETFOCUS:
                    {
                        win_sys->PushEvent(Event{.data = {.window_keyboard_focus_gain =
                                                              WindowKeyboardFocusGainEvent{
                                                                  .timestamp_ms = message_time_ms}},
                                                 .id = ClassID<WindowKeyboardFocusGainEvent>::ID,
                                                 .window = window});
                    }
                    break;
                    case WM_KILLFOCUS:
                    {
                        win_sys->PushEvent(Event{.data = {.window_keyboard_focus_leave =
                                                              WindowKeyboardFocusLeaveEvent{
                                                                  .timestamp_ms = message_time_ms}},
                                                 .id = ClassID<WindowKeyboardFocusLeaveEvent>::ID,
                                                 .window = window});
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

                        win_sys->PushEvent(
                            Event{.data = {.mouse_button_pressed =
                                               MouseButtonPressedEvent{
                                                   .timestamp_ms = message_time_ms,
                                                   .button = button,
                                                   .clicks = clicks,
                                                   .cursor_position =
                                                       GetRelativeCursorPosition(l_param)}},
                                  .id = ClassID<MouseButtonPressedEvent>::ID,
                                  .window = window});
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
                            Event{.data = {.mouse_buttton_released =
                                               MouseButtonReleasedEvent{
                                                   .timestamp_ms = message_time_ms,
                                                   .button = button,
                                                   .cursor_position =
                                                       GetRelativeCursorPosition(l_param)}},
                                  .id = ClassID<MouseButtonReleasedEvent>::ID,
                                  .window = window});
                    }
                    break;
                    case WM_MOUSEWHEEL:
                    case WM_MOUSEHWHEEL:
                    {
                        float delta =
                            static_cast<float>(GET_WHEEL_DELTA_WPARAM(w_param)) / WHEEL_DELTA;

                        MouseButtonFlags buttons =
                            GetMouseButtonsFlags(GET_KEYSTATE_WPARAM(w_param));

                        win_sys->PushEvent(Event{
                            .data = {.mouse_wheel =
                                         MouseWheelEvent{
                                             .timestamp_ms = message_time_ms,
                                             .buttons = buttons,
                                             .cursor_position =
                                                 GetRelativeTranslatedCursorPosition(handle,
                                                                                     l_param),
                                             .x_scroll = (message == WM_MOUSEHWHEEL ? delta : 0),
                                             .y_scroll = (message == WM_MOUSEWHEEL ? delta : 0)}},
                            .id = ClassID<MouseWheelEvent>::ID,
                            .window = window});
                    }
                    break;
                    case WM_CHAR:
                    case WM_SYSCHAR:
                    {
                        if(IS_HIGH_SURROGATE(w_param)) //save surrogate
                        {
                            window->high_surrogate = w_param;
                        }
                        else
                        {
                            std::uint32_t utf32;

                            if(window->high_surrogate) //surrogate pair
                            {
                                utf32 = ((window->high_surrogate - 0xD8'00) << 10) +
                                        (w_param - 0xDC'00) + 0x1'00'00;

                                window->high_surrogate = L'\0';
                            }
                            else //single char
                            {
                                utf32 = w_param;
                            }

                            WORD key_flags = HIWORD(l_param);

                            ScanCode scancode = LOBYTE(key_flags);
                            if(key_flags & KF_EXTENDED)
                                scancode |= (0b1 << 8); //set 9-bit

                            std::uint16_t repeat_count = 1;
                            if(key_flags & KF_REPEAT)
                                repeat_count = LOWORD(l_param);

                            win_sys->PushEvent(
                                Event{.data = {.keyboard_character_pressed =
                                                   KeyboardCharacterPressedEvent{
                                                       .timestamp_ms = message_time_ms,
                                                       .scancode = scancode,
                                                       .modifiers = GetModifierFlags(),
                                                       .repeat_count = repeat_count,
                                                       .utf32_char = utf32}},
                                      .id = ClassID<KeyboardCharacterPressedEvent>::ID,
                                      .window = window});
                        }
                    }
                    break;
                    case WM_SYSKEYDOWN:
                    case WM_KEYDOWN:
                    {
                        WORD key_flags = HIWORD(l_param);

                        ScanCode scancode = LOBYTE(key_flags);
                        if(key_flags & KF_EXTENDED)
                            scancode |= (0b1 << 8); //set 9-bit

                        std::uint16_t repeat_count = 1;
                        if(key_flags & KF_REPEAT)
                            repeat_count = LOWORD(l_param);

                        win_sys->PushEvent(Event{
                            .data = {.keyboard_key_pressed =
                                         KeyboardKeyPressedEvent{.timestamp_ms = message_time_ms,
                                                                 .scancode = scancode,
                                                                 .modifiers = GetModifierFlags(),
                                                                 .repeat_count = repeat_count}},
                            .id = ClassID<KeyboardKeyPressedEvent>::ID,
                            .window = window});
                    };
                    break;
                    case WM_KEYUP:
                    case WM_SYSKEYUP:
                    {
                        WORD key_flags = HIWORD(l_param);

                        ScanCode scancode = LOBYTE(key_flags);
                        if(key_flags & KF_EXTENDED)
                            scancode |= (0b1 << 8); //set 9-bit

                        win_sys->PushEvent(Event{
                            .data = {.keyboard_key_released =
                                         KeyboardKeyReleasedEvent{.timestamp_ms = message_time_ms,
                                                                  .scancode = scancode,
                                                                  .modifiers = GetModifierFlags()}},
                            .id = ClassID<KeyboardKeyReleasedEvent>::ID,
                            .window = window});
                    };
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
              mouse_focused(false),
              high_surrogate(L'\0')
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

            auto title = Core::System::UTF8ToWide(info.title);
            HWND _handle = CreateWindowExW(ex_style,
                                           WIN32_WINDOW_CLASS_NAME,
                                           title.data(),
                                           style,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           rect.right - rect.left,
                                           rect.bottom - rect.top,
                                           nullptr,
                                           nullptr,
                                           parent->GetInstance(),
                                           &data);

            if(_handle == nullptr)
                Core::System::ThrowLastError();

            this->handle = _handle;

            Core::ScopedCall cleanup(
                [&_handle]()
                {
                    DestroyWindow(_handle);
                });

            display.reset(new Display(this, MonitorFromWindow(_handle, MONITOR_DEFAULTTONEAREST)));

            this->SetState(info.state);
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
            auto wstr = Core::System::UTF8ToWide(title);

            if(SetWindowTextW(handle, wstr.data()) == 0)
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

            std::wstring wstr(length, L'\0');
            if(GetWindowTextW(handle, wstr.data(), length + 1) == 0)
                Core::System::ThrowLastError();

            return Core::System::WideToUTF8(wstr);
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

        WindowResolution Window::GetScaledResolution() const
        {
            return GetResolution();
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

                if(SetWindowLongPtrW(handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW) == 0)
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

                if(SetWindowLongPtrW(handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW) == 0)
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

        WindowSurfaceInfo Window::GetWindowSurfaceInfo() const noexcept
        {
            return Render::Win32SurfaceInfo{.instance = parent->GetInstance(), .window = handle};
        }

        Display* Window::GetDisplay() const noexcept
        {
            return display.get();
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