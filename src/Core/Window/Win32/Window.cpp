#include "Window.h"
#include "WindowSubsystem.h"
#include "Core/Utils/ScopedCall.hpp"

#error SEND EVENTS WHEN WE CHANGE SIZES, STATES, ETC...(ALSO FOR MOUSE, WINDOW, DISPLAY, ...)

namespace Core
{
    namespace Win32
    {
        struct WindowCreateData
        {
            Window* obj;
        };

        thread_local std::runtime_error WND_PROC_LAST_ERROR("");

        LRESULT CALLBACK Win32WindowProc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
        {
            switch(message)
            {
                case WM_CREATE:
                {
                    WindowCreateData* data = static_cast<WindowCreateData*>(
                        reinterpret_cast<CREATESTRUCTW*>(l_param)->lpCreateParams);

                    SetLastError(0);
                    auto res = SetWindowLongPtrW(handle,
                                                 GWLP_USERDATA,
                                                 reinterpret_cast<LONG_PTR>(data->obj));
                    if(res == 0) //possible error
                    {
                        if(::GetLastError() != 0) //error
                            return -1;
                    }
                }
                break;
            }

            return DefWindowProcW(handle, message, w_param, l_param);
        }

        Window::Window(WindowSubsystem* _parent, const WindowInfo& info)
            : parent(_parent)
        {
            WindowCreateData data = {.obj = this};

            auto title = Core::System::UTF8ToWide(info.title);
            HWND _handle = CreateWindowExW(0,
                                           WIN32_WINDOW_CLASS_NAME,
                                           title.data(),
                                           WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU |
                                               WS_MINIMIZEBOX | WS_OVERLAPPED | WS_VISIBLE,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           info.resolution.width,
                                           info.resolution.height,
                                           nullptr,
                                           nullptr,
                                           parent->GetInstance(),
                                           &data);

            if(_handle == nullptr)
                throw Core::System::GetLastError();

            Core::ScopedCall cleanup(
                [&_handle]()
                {
                    DestroyWindow(_handle);
                });

            display.reset(new Display(this, MonitorFromWindow(_handle, MONITOR_DEFAULTTONEAREST)));

            this->SetState(info.state);

            cleanup.Drop();
            this->handle = _handle;
        }

        Window::~Window()
        {
            DestroyWindow(handle);
        }

        void Window::SetTitle(std::string_view title)
        {
            auto wstr = Core::System::UTF8ToWide(title);

            if(SetWindowTextW(handle, wstr.data()) == 0)
                throw Core::System::GetLastError();
        }

        std::string Window::GetTitle() const
        {
            ::SetLastError(0); //clear last error
            int length = GetWindowTextLengthW(handle);
            if(length == 0)
            {
                auto last_error = ::GetLastError();
                if(last_error == 0) //empty title
                    return "";

                throw Core::System::GetLastError();
            }

            std::wstring wstr(length + 1, L'\0');
            if(GetWindowTextW(handle, wstr.data(), length) == 0)
                throw Core::System::GetLastError();

            return Core::System::WideToUTF8(wstr);
        }

        void Window::Resize(const WindowResolution& resolution)
        {
#error TODO!!!
        }

        WindowResolution Window::GetResolution() const
        {
            RECT rect;
            if(GetClientRect(handle, &rect) == 0)
                throw Core::System::GetLastError();

            return WindowResolution{.width = static_cast<std::uint32_t>(rect.right),
                                    .height = static_cast<std::uint32_t>(rect.bottom)};
        }

        WindowResolution Window::GetScaledResolution() const
        {
            return GetResolution();
        }

        void Window::SetState(WindowState state)
        {
#error TODO!!!
        }

        WindowState Window::GetState() const
        {
#error TODO!!!
        }

        void Window::SetMouseCursorPosition(const WindowPosition& pos)
        {
            POINT point = {.x = pos.x, .y = pos.y};
            if(ClientToScreen(handle, &point) == 0)
                throw Core::System::GetLastError();

            if(SetCursorPos(point.x, point.y) == 0)
                throw Core::System::GetLastError();
        }

        WindowPosition Window::GetMouseCursorPosition() const
        {
            POINT point = {};
            if(GetCursorPos(&point) == 0)
                throw Core::System::GetLastError();

            if(ScreenToClient(handle, &point) == 0)
                throw Core::System::GetLastError();

            return WindowPosition{.x = point.x, .y = point.y};
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
    };
};