#include "WindowSubsystem.h"
#include "Window.h"
#include <winuser.h>

namespace Core
{
    namespace Win32
    {
        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              SetProcessDpiAwareness(nullptr),
              SetProcessDPIAware(nullptr),
              dpi_awareness(PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE),
              GetDpiForMonitor(nullptr)
        {
            instance = GetModuleHandleW(nullptr);
            if(instance == nullptr)
                throw std::runtime_error("Failed to get process instance");

            auto user32_res = user32.Open("User32.dll");
            auto shcore_res = shcore.Open("Shcore.dll");

            if(!user32_res.has_value())
            {
                SetProcessDPIAware = reinterpret_cast<decltype(SetProcessDPIAware)>(
                    user32.GetProcAddress("SetProcessDPIAware"));
            }

            if(!shcore_res.has_value())
            {
                SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness)>(
                    shcore.GetProcAddress("SetProcessDpiAwareness"));

                GetDpiForMonitor = reinterpret_cast<decltype(GetDpiForMonitor)>(
                    shcore.GetProcAddress("GetDpiForMonitor"));
            }

            if(SetProcessDpiAwareness)
            {
                if(SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE) !=
                   S_OK)
                {
                    throw std::runtime_error("Failed to set DPI awareness");
                }

                dpi_awareness = PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE;
            }
            else if(SetProcessDPIAware)
            {
                if(SetProcessDPIAware() == FALSE)
                    throw std::runtime_error("Failed to set DPI awareness");

                dpi_awareness = PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE;
            }

            WNDCLASSEXW wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                     .style = CS_DBLCLKS /*| CS_DROPSHADOW*/ | CS_HREDRAW |
                                              CS_OWNDC | CS_VREDRAW,
                                     .lpfnWndProc = Window::Win32WindowProc,
                                     .cbClsExtra = 0,
                                     .cbWndExtra = 0,
                                     .hInstance = instance,
                                     .hIcon = nullptr,
                                     .hCursor = nullptr,
                                     .hbrBackground = nullptr,
                                     .lpszMenuName = nullptr,
                                     .lpszClassName = Window::WIN32_WINDOW_CLASS_NAME,
                                     .hIconSm = nullptr};

            if(RegisterClassExW(&wnd_class) == 0)
                throw Core::System::GetLastError();
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(Window::WIN32_WINDOW_CLASS_NAME, instance);
        }

        void WindowSubsystem::PollEvents()
        {
            while(true)
            {
                MSG msg;
                auto res = GetMessageW(&msg, nullptr, 0, 0);
                if(res >= 0) //dispatch
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                else //error
                    throw Core::System::GetLastError();
            }
        }

        WindowSubsystemType WindowSubsystem::GetType() const noexcept
        {
            return WindowSubsystemType::Win32;
        }

        Core::Window* WindowSubsystem::CreateWindow(const WindowInfo& info)
        {
            return new Window(this, info);
        }

        CursorState WindowSubsystem::GetCursorState() const
        {
            CURSORINFO info = {.cbSize = sizeof(CURSORINFO)};
            if(GetCursorInfo(&info) == 0)
                throw Core::System::GetLastError();

            if(info.flags == 0) //disabled
                return CursorState::Disbaled;

            return CursorState::Enabled;
        }

        void WindowSubsystem::SetCursorState(CursorState state)
        {
            BOOL win_state = (state == CursorState::Enabled ? TRUE : FALSE);
            ShowCursor(win_state);
        }

        HINSTANCE WindowSubsystem::GetInstance() const noexcept
        {
            return instance;
        }

        PROCESS_DPI_AWARENESS WindowSubsystem::GetDPIAwrenessType() const noexcept
        {
            return dpi_awareness;
        }
    };
};