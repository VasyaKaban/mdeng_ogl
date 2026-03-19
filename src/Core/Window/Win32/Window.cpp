#include "Window.h"
#include "WindowSubsystem.h"

namespace Core
{
    namespace Win32
    {
        LRESULT CALLBACK Win32WindowProc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
        {
            switch(message)
            {}

            return DefWindowProcW(handle, message, w_param, l_param);
        }

        Window::Window(WindowSubsystem* _parent, const WindowInfo& info)
            : parent(_parent)
        {
            auto title = Core::System::UTF8ToWide(info.title);
            handle = CreateWindowExW(0,
                                     WIN32_WINDOW_CLASS_NAME,
                                     title.data(),
                                     WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU | WS_MINIMIZEBOX |
                                         WS_OVERLAPPED | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     info.resolution.width,
                                     info.resolution.height,
                                     nullptr,
                                     nullptr,
                                     parent->GetInstance(),
                                     nullptr); //lparam

            if(handle == nullptr)
                throw Core::System::GetLastError();
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
        {}
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

        float Window::GetScaleFactor() const; // return dpi / default_dpi;
        float Window::GetSurfaceScaleFactor() const; // return scaled_resolution / resolution;

        void Window::SetState(WindowState state);
        WindowState Window::GetState() const;

        void Window::SetMouseCursorPosition(const WindowPosition& pos);
        WindowPosition Window::GetMouseCursorPosition() const;

        WindowSurfaceInfo Window::GetWindowSurfaceInfo() const noexcept
        {
            return Render::Win32SurfaceInfo{.instance = parent->GetInstance(), .window = handle};
        }

        Core::WindowSubsystem* Window::GetParent() const noexcept
        {
            return parent;
        }

    };
};