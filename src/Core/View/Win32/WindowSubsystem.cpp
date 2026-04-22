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

        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              public_functions{},
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

            if(window_class_atom = RegisterClassExW(&wnd_class); window_class_atom == 0)
                Core::System::ThrowLastError();

            HandleDisplayChange(true); //add existing displays

            cleanup.Drop();

            SUBSYSTEM = this;
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(MAKEINTATOM(window_class_atom), instance);
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
                    std::visit(
                        [this, &handle = event.handle]<typename T>(const T& data)
                        {
                            if constexpr(std::same_as<DisplayAddedEvent, T>)
                            {
                                this->Emit(data);
                            }
                            else if constexpr(std::same_as<DisplayRemovedEvent, T> ||
                                              std::same_as<DisplayMovedEvent, T> ||
                                              std::same_as<DisplayVideoModeChangedEvent, T> ||
                                              std::same_as<DisplayScaleChangedEvent, T>)
                            {
                                std::get<std::shared_ptr<Display>>(handle)->Emit(data);
                            }
                            else
                            {
                                std::get<Window*>(handle)->Emit(data);
                            }
                        },
                        event.data);
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

        std::vector<std::shared_ptr<Core::Display>> WindowSubsystem::GetDisplays()
        {
            std::vector<std::shared_ptr<Core::Display>> out;
            out.reserve(displays.size());

            for(const auto& [_, node]: displays)
                out.push_back(node.display);

            return out;
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

        void WindowSubsystem::HandleDisplayChange(bool initial)
        {
            //reset active states for current displays
            for(auto& [_, node]: displays)
                node.active = false;

#pragma message("Maybe check szDevice???")
            /*
            std::unordered_map<HMONITOR, DisplayNode(Display, wchar_t[32] name)> old + new;


            collect new displays
            for each old display:
                if handle found:
                    if same names:
                        check updates
                    else:
                        erase old
                else:
                    erase old
            
            */

            //enumerate all HMONITOR handles
            EnumDisplayMonitors(
                nullptr,
                nullptr,
                [](HMONITOR handle, HDC dc, LPRECT rect, LPARAM data) -> BOOL
                {
                    WindowSubsystem* win_sys = reinterpret_cast<WindowSubsystem*>(data);
                    auto [it, inserted] = win_sys->displays.insert(
                        std::pair{handle, DisplayNode{.display = {}, .active = true}});
                    if(!inserted) //already exists -> mark as active
                        it->second.active = true;

                    return TRUE;
                },
                reinterpret_cast<LPARAM>(this));

            //firstly add displays
            //secondly update display properties
            for(auto& [handle, node]: displays)
            {
                if(!node.active)
                    continue;

                if(node.display.get() == nullptr) //new display
                {
                    node.display.reset(new Display(this, handle));
                    if(initial)
                    {
                        events.push(
                            Event{.data = DisplayAddedEvent{.timestamp_ms = GetEventTimestamp(),
                                                            .display = node.display},
                                  .handle = nullptr});
                    }
                }
                else //remain display -> check changed properties
                {
                    //on position emit DisplayMovedEvent
                    //on video mode emit DisplayVideoModeChangedEvent
                    //on scale emit DisplayScaleChangedEvent

                    auto flags = node.display->Update();
                    if(flags & DisplayChangesFlagBits::Position)
                    {
                        events.push(Event{
                            .data = DisplayMovedEvent{.timestamp_ms = GetEventTimestamp(),
                                                      .position = node.display->GetPosition()},
                            .handle = node.display});
                    }
                    else if(flags & DisplayChangesFlagBits::VideoMode)
                    {
                        events.push(
                            Event{.data =
                                      DisplayVideoModeChangedEvent{
                                          .timestamp_ms = GetEventTimestamp(),
                                          .video_mode = node.display->GetCurrentVideoMode()},
                                  .handle = node.display});
                    }
                    else if(flags & DisplayChangesFlagBits::ScaleFactor)
                    {
                        events.push(Event{
                            .data = DisplayScaleChangedEvent{.timestamp_ms = GetEventTimestamp(),
                                                             .scale_factor =
                                                                 node.display->GetScaleFactor()},
                            .handle = node.display});
                    }
                }
            }

            //finally remove displays so we can update window's display in handler
            for(auto it = displays.begin(); it != displays.end(); it++)
            {
                const auto& [_, node] = *it;

                if(node.active)
                    continue;

                events.push(Event{.data = DisplayRemovedEvent{.timestamp_ms = GetEventTimestamp()},
                                  .handle = node.display});

                displays.erase(it);
            }
        }

        std::shared_ptr<Display>
        WindowSubsystem::GetDisplayByMonitorHandle(HMONITOR handle) const noexcept
        {
            return displays.find(handle)->second.display;
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