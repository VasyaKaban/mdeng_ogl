#include "WindowSubsystem.h"
#include "Window.h"
#include <winuser.h>

namespace Core
{
    namespace Win32
    {
        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              SetProcessDpiAwarenessContext(nullptr),
              SetProcessDpiAwareness(nullptr),
              SetProcessDPIAware(nullptr),
              dpi_awareness(DPI_AWARENESS_CONTEXT_UNAWARE)
        {
            instance = GetModuleHandleW(nullptr);
            if(instance == nullptr)
                throw std::runtime_error("Failed to get process instance");

            auto user32_res = user32.Open("User32.dll");
            auto shcore_res = shcore.Open("Shcore.dll");

            if(!user32_res.has_value())
            {
                SetProcessDpiAwarenessContext =
                    reinterpret_cast<decltype(SetProcessDpiAwarenessContext)>(
                        user32.GetProcAddress("SetProcessDpiAwarenessContext"));

                SetProcessDPIAware = reinterpret_cast<decltype(SetProcessDPIAware)>(
                    user32.GetProcAddress("SetProcessDPIAware"));
            }

            if(!shcore_res.has_value())
            {
                SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness)>(
                    shcore.GetProcAddress("SetProcessDpiAwareness"));
            }

            if(SetProcessDpiAwarenessContext)
            {
                BOOL res =
                    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
                if(res == TRUE)
                    dpi_awareness = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
                else if(::GetLastError() == ERROR_INVALID_PARAMETER) //unsupported
                {
                    res = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
                    if(res == TRUE)
                        dpi_awareness = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
                }
            }

            if(SetProcessDpiAwareness && dpi_awareness == DPI_AWARENESS_CONTEXT_UNAWARE)
            {
                if(SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE) !=
                   S_OK)
                {
                    throw std::runtime_error("Failed to set DPI awareness");
                    dpi_awareness = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
                }
            }

            if(SetProcessDPIAware && dpi_awareness == DPI_AWARENESS_CONTEXT_UNAWARE)
            {
                if(SetProcessDPIAware() == FALSE)
                    throw std::runtime_error("Failed to set DPI awareness");

                dpi_awareness = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
            }

            WNDCLASSEXW wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                     .style = CS_DBLCLKS | CS_DROPSHADOW | CS_HREDRAW | CS_OWNDC |
                                              CS_VREDRAW,
                                     .lpfnWndProc = Win32WindowProc,
                                     .cbClsExtra = 0,
                                     .cbWndExtra = 0,
                                     .hInstance = instance,
                                     .hIcon = nullptr,
                                     .hCursor = nullptr,
                                     .hbrBackground = nullptr,
                                     .lpszMenuName = nullptr,
                                     .lpszClassName = WIN32_WINDOW_CLASS_NAME,
                                     .hIconSm = nullptr};

            if(RegisterClassExW(&wnd_class) == 0)
                throw Core::System::GetLastError();
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(WIN32_WINDOW_CLASS_NAME, instance);
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

        HINSTANCE WindowSubsystem::GetInstance() const noexcept
        {
            return instance;
        }
    };
};