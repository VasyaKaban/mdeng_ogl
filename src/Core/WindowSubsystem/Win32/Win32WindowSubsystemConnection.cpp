#include "Win32WindowSubsystemConnection.h"
#include "Win32Window.h"
#include "Win32Display.h"
#include "Core/Utils/ScopedCall.hpp"
#include "Win32KeyboardState.h"
#include "Win32Clipboard.h"

namespace Core
{
    Win32WindowSubsystemConnection::Win32WindowSubsystemConnection(
        const WindowSubsystemConnectionInfo& info)
        : instance(nullptr),
          user32({}),
          shcore({}),
          dwmapi({}),
          keyboard_state(),
          clipboard(),
          window_class_atom(0),
          displays(),
          events()
    {
        if(SUBSYSTEM)
            throw std::runtime_error("Win32 subsystem already created");

        instance = GetModuleHandleW(nullptr);
        if(instance == nullptr)
            Core::System::ThrowLastError();

        auto user32_ex = user32.lib.Open(user32.LIBRARY_NAME);
        auto shcore_ex = shcore.lib.Open(shcore.LIBRARY_NAME);
        auto dwmapi_ex = dwmapi.lib.Open(dwmapi.LIBRARY_NAME);

#define POPULATE_LOADER(LOADER, SYMBOL_NAME) \
    LOADER.SYMBOL_NAME = \
        reinterpret_cast<decltype(LOADER.SYMBOL_NAME)>(LOADER.lib.GetProcAddress(#SYMBOL_NAME));

        if(!user32_ex)
        {
            POPULATE_LOADER(user32, GetDisplayConfigBufferSizes)
            POPULATE_LOADER(user32, QueryDisplayConfig)
            POPULATE_LOADER(user32, DisplayConfigGetDeviceInfo)
            POPULATE_LOADER(user32, EnableNonClientDpiScaling)
            POPULATE_LOADER(user32, SetProcessDpiAwarenessContext)
            POPULATE_LOADER(user32, SetThreadDpiAwarenessContext)
            POPULATE_LOADER(user32, SetProcessDPIAware)
        }

        if(!shcore_ex)
        {
            POPULATE_LOADER(shcore, GetDpiForMonitor)
            POPULATE_LOADER(shcore, SetProcessDpiAwareness)
        }

        if(!dwmapi_ex)
        {
            POPULATE_LOADER(dwmapi, DwmEnableBlurBehindWindow)
        }

#undef POPULATE_LOADER

        static bool dpi_set = false;
        if(!dpi_set)
        {
            DPI_AWARENESS = DPI_AWARENESS_CONTEXT_UNAWARE;

            if(user32.SetProcessDpiAwarenessContext || user32.SetThreadDpiAwarenessContext)
            {
                DPI_AWARENESS_CONTEXT DPI_AWARENESS_CONTEXT_TYPES[] = {
                    DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2,
                    DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE,
                    DPI_AWARENESS_CONTEXT_SYSTEM_AWARE,
                    DPI_AWARENESS_CONTEXT_UNAWARE};

                for(const auto& type: DPI_AWARENESS_CONTEXT_TYPES)
                {
                    if(user32.SetProcessDpiAwarenessContext)
                    {
                        auto res = user32.SetProcessDpiAwarenessContext(
                            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
                        if(res == TRUE)
                        {
                            DPI_AWARENESS = type;
                            dpi_set = true;
                            break;
                        }
                    }
                    else if(user32.SetThreadDpiAwarenessContext)
                    {
                        auto res = user32.SetThreadDpiAwarenessContext(type);
                        if(res != nullptr)
                        {
                            DPI_AWARENESS = type;
                            dpi_set = true;
                            break;
                        }
                    }
                }
            }
            else if(shcore.SetProcessDpiAwareness)
            {
                std::pair<PROCESS_DPI_AWARENESS, DPI_AWARENESS_CONTEXT> DPI_AWARENESS_TYPES[] = {
                    {PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE,
                     DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE},
                    {PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE,
                     DPI_AWARENESS_CONTEXT_SYSTEM_AWARE},
                    {PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE, DPI_AWARENESS_CONTEXT_UNAWARE}};

                for(const auto& [type, ctx]: DPI_AWARENESS_TYPES)
                {
                    if(auto res = shcore.SetProcessDpiAwareness(type); res == S_OK)
                    {
                        DPI_AWARENESS = ctx;
                        dpi_set = true;
                        break;
                    }
                }
            }
            else if(user32.SetProcessDPIAware)
            {
                if(user32.SetProcessDPIAware() != FALSE)
                {
                    DPI_AWARENESS = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
                    dpi_set = true;
                }
            }

            dpi_set = true;
        }

        Core::ScopedCall cleanup(
            [this]()
            {
                if(window_class_atom)
                    UnregisterClassW(MAKEINTATOM(window_class_atom), instance);
            });

        keyboard_state.reset(new Win32KeyboardState(this, info.keyboard_access_state));

        //register window class
        WNDCLASSEXW wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                 .style = CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW,
                                 .lpfnWndProc = Win32Window::Win32WindowProc,
                                 .cbClsExtra = 0,
                                 .cbWndExtra = 0,
                                 .hInstance = instance,
                                 .hIcon = nullptr,
                                 .hCursor = nullptr,
                                 .hbrBackground = nullptr,
                                 .lpszMenuName = nullptr,
                                 .lpszClassName = Win32Window::WIN32_WINDOW_CLASS_NAME,
                                 .hIconSm = nullptr};

        if(window_class_atom = RegisterClassExW(&wnd_class); window_class_atom == 0)
            Core::System::ThrowLastError();

        HandleDisplayChange(true); //add existing displays
        SetCursorState(info.cursor_state);

        cleanup.Drop();

        SUBSYSTEM = this;
    }

    Win32WindowSubsystemConnection::~Win32WindowSubsystemConnection()
    {
        UnregisterClassW(MAKEINTATOM(window_class_atom), instance);
    }

    void Win32WindowSubsystemConnection::PollEvents()
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
                    [this, &event]<typename T>(const T& data)
                    {
                        if(event.window)
                        {
                            event.window->Emit(data);
                        }
                        else if(event.display)
                        {
                            event.display->Emit(data);
                        }
                        else
                        {
                            this->Emit(data);
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

    WindowSubsystemConnectionType Win32WindowSubsystemConnection::GetType() const noexcept
    {
        return WindowSubsystemConnectionType::Win32;
    }

    Core::Window* Win32WindowSubsystemConnection::CreateWindow(const WindowInfo& info)
    {
        return new Win32Window(this, info);
    }

    CursorState Win32WindowSubsystemConnection::GetCursorState() const
    {
        CURSORINFO info = {.cbSize = sizeof(CURSORINFO)};
        if(GetCursorInfo(&info) == 0)
            Core::System::ThrowLastError();

        if(info.flags == 0) //disabled
            return CursorState::Disbaled;

        return CursorState::Enabled;
    }

    void Win32WindowSubsystemConnection::SetCursorState(CursorState state)
    {
        BOOL win_state = (state == CursorState::Enabled ? TRUE : FALSE);
        ShowCursor(win_state);
    }

    KeyboardKey Win32WindowSubsystemConnection::GetKeyByScancode(ScanCode scancode)
    {
        return keyboard_state->GetKeyByScancode(scancode);
    }

    std::optional<ScanCode> Win32WindowSubsystemConnection::GetScanCodeFromKey(KeyboardKey key)
    {
        return keyboard_state->GetScanCodeFromKey(key);
    }

    KeyboardAccessState Win32WindowSubsystemConnection::GetKeyboardAccessState()
    {
        return keyboard_state->GetKeyboardAccessState();
    }

    void Win32WindowSubsystemConnection::SetKeyboardAccessState(KeyboardAccessState state)
    {
        keyboard_state->SetKeyboardAccessState(state);
    }

    void
    Win32WindowSubsystemConnection::GetClipboard(const std::function<ClipboardCallback>& callback)
    {
        Win32Clipboard* clipboard_ptr =
            std::construct_at(reinterpret_cast<Win32Clipboard*>(clipboard), this);

        std::exception_ptr ex_ptr;
        try
        {
            callback(clipboard_ptr);
        }
        catch(...)
        {
            ex_ptr = std::current_exception();
        }

        std::destroy_at(clipboard_ptr);

        if(ex_ptr)
            std::rethrow_exception(ex_ptr);
    }

    std::vector<RCPointer<Display>> Win32WindowSubsystemConnection::GetDisplays()
    {
        std::vector<RCPointer<Display>> out;
        out.reserve(displays.size());

        for(const auto& [_, node]: displays)
            out.push_back(node.display);

        return out;
    }

    const User32Loader& Win32WindowSubsystemConnection::GetUser32Loader() const noexcept
    {
        return user32;
    }

    const SHCoreLoader& Win32WindowSubsystemConnection::GetSHCoreLoader() const noexcept
    {
        return shcore;
    }

    const DWMAPILoader& Win32WindowSubsystemConnection::GetDWMAPILoader() const noexcept
    {
        return dwmapi;
    }

    Win32KeyboardState* Win32WindowSubsystemConnection::GetKeyboardState() const noexcept
    {
        return keyboard_state.get();
    }

    HINSTANCE Win32WindowSubsystemConnection::GetInstance() const noexcept
    {
        return instance;
    }

    DPI_AWARENESS_CONTEXT Win32WindowSubsystemConnection::GetDPIAwrenessType() const noexcept
    {
        return DPI_AWARENESS;
    }

    void Win32WindowSubsystemConnection::PushEvent(Event&& event)
    {
        events.push(std::move(event));
    }

    void Win32WindowSubsystemConnection::HandleDisplayChange(bool initial)
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
                Win32WindowSubsystemConnection* win_conn =
                    reinterpret_cast<Win32WindowSubsystemConnection*>(data);
                auto [it, inserted] = win_conn->displays.insert(
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

            if(!node.display) //new display
            {
                node.display.Reset(new Win32Display(this, handle);
                if(!initial)
                {
                    events.push(Event{.data = DisplayAddedEvent{.timestamp_ms = GetEventTimestamp(),
                                                                .display = node.display},
                                      .window = nullptr,
                                      .display = nullptr});
                }
            }
            else //remain display -> check changed properties
            {
                //on position emit DisplayMovedEvent
                //on video mode emit DisplayVideoModeChangedEvent
                //on scale emit DisplayScaleChangedEvent

                auto flags = node.display.Get()->Update();
                if(flags & DisplayChangesFlagBits::Position)
                {
                    events.push(
                        Event{.data = DisplayMovedEvent{.timestamp_ms = GetEventTimestamp(),
                                                        .position = node.display->GetPosition()},
                              .handle = node.display});
                }
                else if(flags & DisplayChangesFlagBits::VideoMode)
                {
                    events.push(Event{.data =
                                          DisplayVideoModeChangedEvent{
                                              .timestamp_ms = GetEventTimestamp(),
                                              .video_mode = node.display->GetCurrentVideoMode()},
                                      .handle = node.display});
                }
                else if(flags & DisplayChangesFlagBits::ScaleFactor)
                {
                    events.push(
                        Event{.data = DisplayScaleChangedEvent{.timestamp_ms = GetEventTimestamp(),
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

    RCPointer<Display>
    Win32WindowSubsystemConnection::GetDisplayByMonitorHandle(HMONITOR handle) const noexcept
    {
        return displays.find(handle)->second.display;
    }

    Win32WindowSubsystemConnection* Win32WindowSubsystemConnection::GetConnection() noexcept
    {
        return SUBSYSTEM;
    }

    void Win32WindowSubsystemConnection::operator delete(void* ptr)
    {
        ::operator delete(ptr);
        SUBSYSTEM = nullptr;
    }
};