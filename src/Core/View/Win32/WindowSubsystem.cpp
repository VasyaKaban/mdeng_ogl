#include "WindowSubsystem.h"
#include "Window.h"
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

        LRESULT CALLBACK WindowSubsystem::Win32ServiceWindowProc(HWND handle,
                                                                 UINT message,
                                                                 WPARAM w_param,
                                                                 LPARAM l_param)
        {
            switch(message)
            {
                case WM_CREATE:
                {
                    WindowSubsystem* win_sys = reinterpret_cast<WindowSubsystem*>(
                        reinterpret_cast<CREATESTRUCTW*>(l_param));

                    Core::System::SetLastError(ERROR_SUCCESS);
                    auto res = SetWindowLongPtrW(handle,
                                                 GWLP_USERDATA,
                                                 reinterpret_cast<LONG_PTR>(win_sys));
                    if(res == 0) //possible error
                    {
                        if(Core::System::GetLastError() != ERROR_SUCCESS) //error
                            return -1;
                    }
                }
                break;
                case WM_DISPLAYCHANGE:
                {
#error QueueEvent as std::variant and remove id -> we can get it from std::get(variant)
#error MAYBE HANDLE WM_DISPLAYCHANGE IN EACH NON-SERVICE WINDOW?? THERE IS NO STRICT ORDER OF BROADCAST MESSAGES SO IF WE HAVE 1000 WINDOWS WE WILL CREATE 1000 DISPLAY UPDATES...
                    //enumerate all HMONITOR handles
                    //find all removed and added displays
                    //update list of active displays
                    //for each added display emit DisplayAddedEvent
                    //for each removed display emit DisplayRemovedEvent
                    //for each remain displays(non-new) check position, video mode and scale
                    //on position emit DisplayMovedEvent
                    //on video mode emit DisplayVideoModeChangedEvent
                    //on scale emit DisplayScaleChangedEvent
                    //for each window check current HMONITOR handle and prev and if changed emit WindowDisplayChangedEvent

                    /*
                    Check that monitor arrived:
                    if not find HMONIUTOR in monitors:
                        Add();
                    else:
                        get szDevice
                        compare szDevice of new and old:
                        if same:
                            check inner properties and emit cghanges if needed:
                        else:
                            remove old and add new
                    */

#error TODO! -> ADD LIST OF ACTIVE DISPLAYS AND WINDOWS
#error DO NOT EXPLICITLY EMIT EVENTS IN DISPLAY FUNCTIONS(THEY WILL BE EMITTED ON THIS WM_DISPLAYCHANGE)
#error IN WINDOW'S WM_DPICHANGED ONLY SetWindowPos(or not??? -> maybe delegate it to the user??? also think about fullscreen mode)
                }
                break;
                default:
                    return DefWindowProcW(handle, message, w_param, l_param);
                    break;
            }

            return 0;
        }

        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              public_functions{},
              service_window_class_atom(0),
              service_window_handle(nullptr),
              keyboard_state(nullptr),
              window_class_atom(0)
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

            Core::ScopedCall cleanup(
                [this]()
                {
                    if(window_class_atom)
                        UnregisterClassW(MAKEINTATOM(window_class_atom), instance);

                    if(keyboard_state)
                        delete keyboard_state;

                    if(service_window_handle)
                        DestroyWindow(service_window_handle);

                    if(service_window_class_atom)
                        UnregisterClassW(MAKEINTATOM(service_window_class_atom), instance);
                });

            //register service window class
            WNDCLASSEXW service_wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                             .style = 0,
                                             .lpfnWndProc = WindowSubsystem::Win32ServiceWindowProc,
                                             .cbClsExtra = 0,
                                             .cbWndExtra = 0,
                                             .hInstance = instance,
                                             .hIcon = nullptr,
                                             .hCursor = nullptr,
                                             .hbrBackground = nullptr,
                                             .lpszMenuName = nullptr,
                                             .lpszClassName =
                                                 WindowSubsystem::WIN32_SERVICE_WINDOW_CLASS_NAME,
                                             .hIconSm = nullptr};

            if(service_window_class_atom = RegisterClassExW(&service_wnd_class);
               service_window_class_atom == 0)
                Core::System::ThrowLastError();

            //create service window
            service_window_handle =
                CreateWindowExW(0,
                                WindowSubsystem::WIN32_SERVICE_WINDOW_CLASS_NAME,
                                nullptr,
                                0,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                0,
                                0,
                                nullptr,
                                nullptr,
                                instance,
                                this);

            if(service_window_handle == nullptr)
                Core::System::ThrowLastError();

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

            if(window_class_atom = RegisterClassExW(&wnd_class); window_class_atom == 0)
                Core::System::ThrowLastError();

            cleanup.Drop();

            SUBSYSTEM = this;
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(MAKEINTATOM(window_class_atom), instance);
            delete keyboard_state;
            DestroyWindow(service_window_handle);
            UnregisterClassW(MAKEINTATOM(service_window_class_atom), instance);
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

        KeyboardAccessState WindowSubsystem::GetKeyboardAccessState()
        {
            return keyboard_state->GetKeyboardAccessState();
        }

        void WindowSubsystem::SetKeyboardAccessState(KeyboardAccessState state)
        {
            keyboard_state->SetKeyboardAccessState(state);
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