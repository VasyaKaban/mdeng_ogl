#include "WindowSubsystem.h"
#include "Window.h"
#include <winuser.h>
#include "hidusage.h"
#include "Core/Utils/ScopedCall.hpp"
#include <set>
#include <format>

namespace Core
{
    namespace Win32
    {
        struct Win32PrivateDynamicFunctions
        {
            BOOL (*SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
            DPI_AWARENESS_CONTEXT (*SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext);
            HRESULT (*SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
            BOOL (*SetProcessDPIAware)();
        };

        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              public_functions{},
              keyboard_state(nullptr)
        {
            if(SUBSYSTEM)
                throw std::runtime_error("Win32 subsystem already created");

            instance = GetModuleHandleW(nullptr);
            if(instance == nullptr)
                Core::System::ThrowLastError();

            auto user32_ex = user32.Open("User32.dll");
            auto shcore_ex = shcore.Open("Shcore.dll");

            Win32PrivateDynamicFunctions private_functions = {};
            if(!user32_ex)
            {
                private_functions.SetProcessDPIAware =
                    reinterpret_cast<decltype(private_functions.SetProcessDPIAware)>(
                        user32.GetProcAddress("SetProcessDPIAware"));

                private_functions.SetProcessDpiAwarenessContext =
                    reinterpret_cast<decltype(private_functions.SetProcessDpiAwarenessContext)>(
                        user32.GetProcAddress("SetProcessDpiAwarenessContext"));

                private_functions.SetThreadDpiAwarenessContext =
                    reinterpret_cast<decltype(private_functions.SetThreadDpiAwarenessContext)>(
                        user32.GetProcAddress("SetThreadDpiAwarenessContext"));

                public_functions.GetDisplayConfigBufferSizes =
                    reinterpret_cast<decltype(public_functions.GetDisplayConfigBufferSizes)>(
                        user32.GetProcAddress("GetDisplayConfigBufferSizes"));

                public_functions.QueryDisplayConfig =
                    reinterpret_cast<decltype(public_functions.QueryDisplayConfig)>(
                        user32.GetProcAddress("QueryDisplayConfig"));

                public_functions.DisplayConfigGetDeviceInfo =
                    reinterpret_cast<decltype(public_functions.DisplayConfigGetDeviceInfo)>(
                        user32.GetProcAddress("DisplayConfigGetDeviceInfo"));

                public_functions.EnableNonClientDpiScaling =
                    reinterpret_cast<decltype(public_functions.EnableNonClientDpiScaling)>(
                        user32.GetProcAddress("EnableNonClientDpiScaling"));
            }

            if(!shcore_ex)
            {
                private_functions.SetProcessDpiAwareness =
                    reinterpret_cast<decltype(private_functions.SetProcessDpiAwareness)>(
                        shcore.GetProcAddress("SetProcessDpiAwareness"));

                public_functions.GetDpiForMonitor =
                    reinterpret_cast<decltype(public_functions.GetDpiForMonitor)>(
                        shcore.GetProcAddress("GetDpiForMonitor"));
            }

            static bool dpi_set = false;
            if(!dpi_set)
            {
                if(private_functions.SetProcessDpiAwarenessContext ||
                   private_functions.SetThreadDpiAwarenessContext)
                {
                    DPI_AWARENESS_CONTEXT DPI_AWARENESS_CONTEXT_TYPES[] = {
                        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2,
                        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE,
                        DPI_AWARENESS_CONTEXT_SYSTEM_AWARE,
                        DPI_AWARENESS_CONTEXT_UNAWARE};

                    for(const auto& type: DPI_AWARENESS_CONTEXT_TYPES)
                    {
                        if(private_functions.SetProcessDpiAwarenessContext)
                        {
                            auto res = private_functions.SetProcessDpiAwarenessContext(
                                DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
                            if(res == TRUE)
                            {
                                DPI_AWARENESS = type;
                                dpi_set = true;
                                break;
                            }
                        }
                        else if(private_functions.SetThreadDpiAwarenessContext)
                        {
                            auto res = private_functions.SetThreadDpiAwarenessContext(type);
                            if(res != nullptr)
                            {
                                DPI_AWARENESS = type;
                                dpi_set = true;
                                break;
                            }
                        }
                    }
                }
                else if(private_functions.SetProcessDpiAwareness)
                {
                    std::pair<PROCESS_DPI_AWARENESS, DPI_AWARENESS_CONTEXT> DPI_AWARENESS_TYPES[] =
                        {{PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE,
                          DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE},
                         {PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE,
                          DPI_AWARENESS_CONTEXT_SYSTEM_AWARE},
                         {PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE,
                          DPI_AWARENESS_CONTEXT_UNAWARE}};

                    for(const auto& [type, ctx]: DPI_AWARENESS_TYPES)
                    {
                        if(auto res = private_functions.SetProcessDpiAwareness(type); res == S_OK)
                        {
                            DPI_AWARENESS = ctx;
                            dpi_set = true;
                            break;
                        }
                    }
                }
                else if(private_functions.SetProcessDPIAware)
                {
                    if(private_functions.SetProcessDPIAware() != FALSE)
                    {
                        DPI_AWARENESS = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
                        dpi_set = true;
                    }
                }
                else
                    DPI_AWARENESS = DPI_AWARENESS_CONTEXT_UNAWARE;

                dpi_set = true;
            }

            const wchar_t* window_class = nullptr;

            Core::ScopedCall cleanup(
                [this, &window_class]()
                {
                    if(window_class)
                        UnregisterClassW(window_class, instance);

                    if(keyboard_state)
                        delete keyboard_state;
                });

            keyboard_state = new KeyboardState(this);

            //register window class
            WNDCLASSEXW wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                     .style = CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW,
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
                Core::System::ThrowLastError();

            window_class = Window::WIN32_WINDOW_CLASS_NAME;

            cleanup.Drop();

            SUBSYSTEM = this;
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(Window::WIN32_WINDOW_CLASS_NAME, instance);
            delete keyboard_state;
        }

        void WindowSubsystem::PollEvents()
        {
            BOOL res = FALSE;
            MSG msg;
            while((res = PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) != 0) //not WM_QUIT
            {
                if(res > 0) //dispatch
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                else //error
                    Core::System::ThrowLastError();
            }

            while(!events.empty())
            {
                const auto& event = events.front();
                try
                {
                    event.window->EmitRaw(event.id, static_cast<const void*>(&event.data));
                    events.pop();
                }
                catch(...)
                {
                    events.pop();
                    throw;
                }
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
                Core::System::ThrowLastError();

            if(info.flags == 0) //disabled
                return CursorState::Disbaled;

            return CursorState::Enabled;
        }

        void WindowSubsystem::SetCursorState(CursorState state)
        {
            BOOL win_state = (state == CursorState::Enabled ? TRUE : FALSE);
            ShowCursor(win_state);
        }

        KeyboardKey WindowSubsystem::GetKeyByScancode(ScanCode scancode)
        {
            return keyboard_state->GetKeyByScancode(scancode);
        }

        std::optional<ScanCode> WindowSubsystem::GetScanCodeFromKey(KeyboardKey key)
        {
            return keyboard_state->GetScanCodeFromKey(key);
        }

        const Win32PublicDynamicFunctions& WindowSubsystem::GetPublicFunctions() const noexcept
        {
            return public_functions;
        }

        KeyboardState* WindowSubsystem::GetKeyboardState() const noexcept
        {
            return keyboard_state;
        }

        HINSTANCE WindowSubsystem::GetInstance() const noexcept
        {
            return instance;
        }

        DPI_AWARENESS_CONTEXT WindowSubsystem::GetDPIAwrenessType() const noexcept
        {
            return DPI_AWARENESS;
        }

        void WindowSubsystem::PushEvent(Event&& event)
        {
            events.push(std::move(event));
        }

        WindowSubsystem* WindowSubsystem::GetSubsystem() noexcept
        {
            return SUBSYSTEM;
        }

        void WindowSubsystem::operator delete(void* ptr)
        {
            ::operator delete(ptr);
            SUBSYSTEM = nullptr;
        }
    };
};