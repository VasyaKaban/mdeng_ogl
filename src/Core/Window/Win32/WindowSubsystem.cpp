#include "WindowSubsystem.h"
#include "Window.h"
#include <winuser.h>
#include "Core/Utils/ScopedCall.hpp"
#include <set>

namespace Core
{
    namespace Win32
    {
        static WindowSubsystem* HOOK_WIN_SYS = nullptr;

        LRESULT CALLBACK WindowSubsystem::ShellProc(int code, WPARAM w_param, LPARAM l_param)
        {
            if(code == HSHELL_LANGUAGE)
            {
                //#error SHOULD WE EVEN CREATE AN EVENT???
                //#error THERE IS NO WAY THAT SOMEONE WILL CHANGE LAYOUT IN RUNTIME. SO OK IF USER DELETS ENGLISH LAYOUT THEN HE WILL RECEIVE FOR EXAMPEL CYRILLIC SYMBOLS BUT IT STILL OK. MAYBE CREATR TWO EVENTS: ActiveLayoutChanged and LayoutListChanged
                //#error ON ActiveLayoutChanged -> WE SHOULD DO NOTHING. ON LayoutListChanged WE SHOULD RECALCULATE ALL VALUES FOR KEYS
                //#error MAYBE DELETE ActiveLayoutChanged AND LEAVE LayoutListChanged ONLY???
                //#error ONLY RECREATE MAPPINGS WHEN FOR NEW LAYOUTS. CHECK HKL -> THEIR FIRST 8B'ITS IS A ID. + READ NAME FROM REGISTRY + SHLoad... + SEND EVENT ONLY WHEN CURRENT LAYOUT IS NOT
                //#error GetKeyNameTextW FOR UNICODE AND NON-CHAR KEYS???
                if(HOOK_WIN_SYS->UpdateKeyboardLayouts())
                {
                    HOOK_WIN_SYS->PushEvent(
                        Event{.data = {.window_sybsystem_keyboard_layout_changed =
                                           WindowSybsystemKeyboardLayoutChangedEvent{
                                               .timestamp_ms = GetEventTimestamp()}},
                              .id = Core::ClassID<WindowSybsystemKeyboardLayoutChangedEvent>::ID,
                              .window = nullptr});
                };
            }

            return CallNextHookEx(nullptr, code, w_param, l_param);
        }

        WindowSubsystem::WindowSubsystem()
            : instance(nullptr),
              SetProcessDpiAwareness(nullptr),
              SetProcessDPIAware(nullptr),
              dpi_awareness(PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE),
              GetDpiForMonitor(nullptr),
              GetDisplayConfigBufferSizes(nullptr),
              QueryDisplayConfig(nullptr),
              DisplayConfigGetDeviceInfo(nullptr)
        {
            instance = GetModuleHandleW(nullptr);
            if(instance == nullptr)
                Core::System::ThrowLastError();

            auto user32_ex = user32.Open("User32.dll");
            auto shcore_ex = shcore.Open("Shcore.dll");

            if(!user32_ex)
            {
                SetProcessDPIAware = reinterpret_cast<decltype(SetProcessDPIAware)>(
                    user32.GetProcAddress("SetProcessDPIAware"));

                GetDisplayConfigBufferSizes =
                    reinterpret_cast<decltype(GetDisplayConfigBufferSizes)>(
                        user32.GetProcAddress("GetDisplayConfigBufferSizes"));

                QueryDisplayConfig = reinterpret_cast<decltype(QueryDisplayConfig)>(
                    user32.GetProcAddress("QueryDisplayConfig"));

                DisplayConfigGetDeviceInfo = reinterpret_cast<decltype(DisplayConfigGetDeviceInfo)>(
                    user32.GetProcAddress("DisplayConfigGetDeviceInfo"));
            }

            if(!shcore_ex)
            {
                SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness)>(
                    shcore.GetProcAddress("SetProcessDpiAwareness"));

                GetDpiForMonitor = reinterpret_cast<decltype(GetDpiForMonitor)>(
                    shcore.GetProcAddress("GetDpiForMonitor"));
            }

            if(SetProcessDpiAwareness)
            {
                if(auto res =
                       SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE);
                   res != S_OK)
                {
                    throw Core::Win32Exception(HRESULT_CODE(res));
                }

                dpi_awareness = PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE;
            }
            else if(SetProcessDPIAware)
            {
                if(SetProcessDPIAware() ==
                   FALSE) //there is noinfo about GetLastError so we throw common exception
                    throw std::runtime_error(
                        "Failed to set DPI awareness. Function: SetProcessDPIAware");

                dpi_awareness = PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE;
            }

            this->UpdateKeyboardLayouts();

            shell_hook = SetWindowsHookExW(WH_SHELL,
                                           WindowSubsystem::ShellProc,
                                           nullptr,
                                           Core::System::GetMainThreadID());

            if(shell_hook == nullptr)
                Core::System::ThrowLastError();

            HOOK_WIN_SYS = this;

            Core::ScopedCall cleanup(
                [this]()
                {
                    UnhookWindowsHookEx(shell_hook);
                });

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
                Core::System::ThrowLastError();

            cleanup.Drop();
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(Window::WIN32_WINDOW_CLASS_NAME, instance);
            UnhookWindowsHookEx(shell_hook);
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
                    if(event.window)
                        event.window->EmitRaw(event.id, static_cast<const void*>(&event.data));
                    else
                        this->EmitRaw(event.id, static_cast<const void*>(&event.data));

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

        std::string WindowSubsystem::GetKeyNameByScancode(ScanCode scancode)
        {
            std::string res;

            for(const auto& desc: keyboard_layouts)
            {
                auto it = desc.scancode_to_string_mapping.find(scancode);
                if(it != desc.scancode_to_string_mapping.end())
                {
                    res = it->second;
                    break;
                }
            }

            return res;
        }

        std::optional<ScanCode> WindowSubsystem::GetScanCodeFromKeyName(std::string_view name)
        {
            std::optional<ScanCode> res;

            for(const auto& desc: keyboard_layouts)
            {
                auto it = desc.string_to_scancode_mapping.find(name);
                if(it != desc.string_to_scancode_mapping.end())
                {
                    res = it->second;
                    break;
                }
            }

            return res;
        }

        HINSTANCE WindowSubsystem::GetInstance() const noexcept
        {
            return instance;
        }

        PROCESS_DPI_AWARENESS WindowSubsystem::GetDPIAwrenessType() const noexcept
        {
            return dpi_awareness;
        }

        void WindowSubsystem::PushEvent(Event&& event)
        {
            events.push(std::move(event));
        }

        bool WindowSubsystem::UpdateKeyboardLayouts()
        {
            constexpr static std::size_t SCANCODES_COUNT =
                256; //approximate count. There is still arounf ~126 keys on keyboard, so we have a reserve

            wchar_t wstr[65] = {};

            auto size = GetKeyboardLayoutList(0, nullptr);
            std::vector<HKL> layouts(size);
            GetKeyboardLayoutList(size, layouts.data());

            std::set<HKL> unique_layouts(layouts.begin(), layouts.end());

            //check that it's only language change
            std::size_t found_count = 0;
            for(const auto& desc: keyboard_layouts)
            {
                if(unique_layouts.find(desc.layout) != unique_layouts.end())
                    found_count++;
            }

            if(found_count == unique_layouts.size()) //it's only language change
                return false;

            for(const auto& desc: keyboard_layouts)
                unique_layouts.insert(desc.layout);

            std::vector<KeyboardLayoutDesc> new_keyboard_layouts;
            new_keyboard_layouts.reserve(unique_layouts.size());
            for(const auto& l: layouts)
            {
                auto it = unique_layouts.find(l);
                if(it != unique_layouts.end())
                {
                    new_keyboard_layouts.push_back(
                        KeyboardLayoutDesc{.layout = l,
                                           .scancode_to_string_mapping = {},
                                           .string_to_scancode_mapping = {}});

                    new_keyboard_layouts.back().scancode_to_string_mapping.reserve(SCANCODES_COUNT);
                    new_keyboard_layouts.back().string_to_scancode_mapping.reserve(SCANCODES_COUNT);

                    unique_layouts.erase(it);
                }
            }

            for(const auto& desc: keyboard_layouts)
            {
                auto it = unique_layouts.find(desc.layout);
                if(it != unique_layouts.end())
                {
                    new_keyboard_layouts.push_back(
                        KeyboardLayoutDesc{.layout = desc.layout,
                                           .scancode_to_string_mapping = {},
                                           .string_to_scancode_mapping = {}});

                    new_keyboard_layouts.back().scancode_to_string_mapping.reserve(SCANCODES_COUNT);
                    new_keyboard_layouts.back().string_to_scancode_mapping.reserve(SCANCODES_COUNT);

                    unique_layouts.erase(it);
                }
            }

            constexpr static std::pair<int, int> SCANCODE_RANGES[] = {{0x0, 0xFF},
                                                                      {0xE0'00, 0xE0'FF},
                                                                      {0xE1'00, 0xE1'FF}};

            constexpr static std::pair<int, std::string_view> NON_OEM_VIRTUAL_KEYS[] = {
                {VK_CANCEL, "Cancel"},
                {VK_BACK, "BackSpace"},
                {VK_TAB, "Tab"},
                {VK_CLEAR, "Clear"},
                {VK_RETURN, "Return"},
                {VK_PAUSE, "Pause"},
                {VK_CAPITAL, "CapsLock"},
                {VK_ESCAPE, "Esc"},
                {VK_SPACE, "space"},
                {VK_PRIOR, "PageUp"},
                {VK_NEXT, "PageDown"},
                {VK_END, "End"},
                {VK_HOME, "Home"},
                {VK_LEFT, "Left"},
                {VK_UP, "Up"},
                {VK_RIGHT, "Right"},
                {VK_DOWN, "Down"},
                {VK_SELECT, "Select"},
                {VK_PRINT, "Print"},
                {VK_EXECUTE, "Execute"},
                {VK_SNAPSHOT, "PrintScreen"},
                {VK_INSERT, "Insert"},
                {VK_DELETE, "Delete"},
                {VK_HELP, "Help"},
                {1, "1"},
                {2, "2"},
                {3, "3"},
                {4, "4"},
                {5, "5"},
                {6, "6"},
                {7, "7"},
                {8, "8"},
                {9, "9"},
                {'A', "A"},
                {'B', "B"},
                {'C', "C"},
                {'D', "D"},
                {'E', "E"},
                {'F', "F"},
                {'G', "G"},
                {'H', "H"},
                {'I', "I"},
                {'J', "J"},
                {'K', "K"},
                {'L', "L"},
                {'M', "M"},
                {'N', "N"},
                {'O', "O"},
                {'P', "P"},
                {'Q', "Q"},
                {'R', "R"},
                {'S', "S"},
                {'T', "T"},
                {'U', "U"},
                {'V', "V"},
                {'W', "W"},
                {'X', "X"},
                {'Y', "Y"},
                {'Z', "Z"},
                {VK_LWIN, "LeftMeta"},
                {VK_RWIN, "RightMeta"},
                {VK_APPS, "Menu"},
                {VK_SLEEP, "Sleep"},
                {VK_NUMPAD0, "KP_0"},
                {VK_NUMPAD1, "KP_1"},
                {VK_NUMPAD2, "KP_2"},
                {VK_NUMPAD3, "KP_3"},
                {VK_NUMPAD4, "KP_4"},
                {VK_NUMPAD5, "KP_5"},
                {VK_NUMPAD6, "KP_6"},
                {VK_NUMPAD7, "KP_7"},
                {VK_NUMPAD8, "KP_8"},
                {VK_NUMPAD9, "KP_9"},
                {VK_MULTIPLY, "KP_MULTIPLY"},
                {VK_ADD, "KP_ADD"},
                {VK_SUBTRACT, "KP_SUBTRACT"},
                {VK_DECIMAL, "KP_DECIMAL"},
                {VK_DIVIDE, "KP_DIVIDE"},
                {VK_F1, "F1"},
                {VK_F2, "F2"},
                {VK_F3, "F3"},
                {VK_F4, "F4"},
                {VK_F5, "F5"},
                {VK_F6, "F6"},
                {VK_F7, "F7"},
                {VK_F8, "F8"},
                {VK_F9, "F9"},
                {VK_F10, "F10"},
                {VK_F11, "F11"},
                {VK_F12, "F12"},
                {VK_F13, "F13"},
                {VK_F14, "F14"},
                {VK_F15, "F15"},
                {VK_F16, "F16"},
                {VK_F17, "F17"},
                {VK_F18, "F18"},
                {VK_F19, "F19"},
                {VK_F20, "F20"},
                {VK_F21, "F21"},
                {VK_F22, "F22"},
                {VK_F23, "F23"},
                {VK_F24, "F24"},
                {VK_NUMLOCK, "NumLock"},
                {VK_SCROLL, "ScrollLock"},
                {VK_LSHIFT, "LeftShift"},
                {VK_RSHIFT, "RightShift"},
                {VK_LCONTROL, "LeftControl"},
                {VK_RCONTROL, "RightControl"},
                {VK_LMENU, "LeftAlt"},
                {VK_RMENU, "RightAlt"},
                {VK_NUMLOCK, "NumLock"},
                {VK_NUMLOCK, "NumLock"},
                {VK_NUMLOCK, "NumLock"},
                {VK_NUMLOCK, "NumLock"},
                {VK_NUMLOCK, "NumLock"},
                {VK_NUMLOCK, "NumLock"},
#pragma messge("TODO -> BROWSER AND OTHER...")
            };

            auto prev_keyboard_layout = GetKeyboardLayout(Core::System::GetMainThreadID());
            for(auto& desc: new_keyboard_layouts)
            {
                ActivateKeyboardLayout(desc.layout, 0);

                auto vk = VK_INSERT;
                auto sc = MapVirtualKeyExW(VK_INSERT, MAPVK_VK_TO_VSC, desc.layout);

                BYTE state[256];
                GetKeyboardState(state);
                auto r = ToUnicodeEx(vk, sc, state, wstr, std::size(wstr) - 1, 0, desc.layout);

                ToUnicodeEx(vk, sc, state, wstr, std::size(wstr) - 1, 0, desc.layout);

                for(const auto& non_oem_rng: NON_OEM_VIRTUAL_KEYS)
                {
                    auto scancode =
                        MapVirtualKeyExW(non_oem_rng.first, MAPVK_VK_TO_VSC, desc.layout);
                    if(scancode != 0)
                    {
                        auto [it, _] = desc.scancode_to_string_mapping.insert(
                            std::pair{scancode, non_oem_rng.second});

                        desc.string_to_scancode_mapping.insert({it->second, scancode});
                    }
                }

                for(const auto& scancode_rng: SCANCODE_RANGES)
                {
                    for(int scancode = scancode_rng.first; scancode < scancode_rng.second;
                        scancode++)
                    {
                        bool is_extended =
                            ((scancode & 0xFF'00) == 0xE0'00) || ((scancode & 0xFF'00) == 0xE1'00);

                        LPARAM l_param = 0;
                        l_param |= (scancode & 0xff) << 16;
                        if(is_extended)
                            l_param |= 0b1 << 23;

                        auto vk = MapVirtualKeyExW(scancode, MAPVK_VSC_TO_VK_EX, desc.layout);
                        if(vk != 0)
                        {
                            wchar_t character = MapVirtualKeyExW(vk, MAPVK_VK_TO_CHAR, desc.layout);
                            if(character != 0)
                            {
                                auto [it, _] = desc.scancode_to_string_mapping.insert(
                                    std::pair{scancode, Core::System::WideToUTF8({&character, 1})});

                                desc.string_to_scancode_mapping.insert({it->second, scancode});
                            }
                        }
                    }
                }
            }

            ActivateKeyboardLayout(prev_keyboard_layout, 0);

            this->keyboard_layouts = std::move(new_keyboard_layouts);

            return true;
        }
    };
};