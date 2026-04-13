#include "WindowSubsystem.h"
#include "Window.h"
#include <winuser.h>
#include "hidusage.h"
#include "Core/Utils/ScopedCall.hpp"
#include <set>

namespace Core
{
    namespace Win32
    {
        constexpr static wchar_t WIN32_RAW_INPUT_WINDOW_CLASS_NAME[] =
            L"WIN32_RAW_INPUT_WINDOW_CLASS_NAME";

        static WindowSubsystem* HOOK_WIN_SYS = nullptr;

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

        LRESULT CALLBACK WindowSubsystem::RawInputWindowProc(HWND handle,
                                                             UINT message,
                                                             WPARAM w_param,
                                                             LPARAM l_param)
        {
            switch(message)
            {
                case WM_CREATE:
                {
                    HOOK_WIN_SYS->preceded_scancode = PrecededScanCode::None;
                    if(GetKeyboardState(HOOK_WIN_SYS->raw_input_keyboard_state) == 0)
                        return -1;

                    RAWINPUTDEVICE raw_keyboard_desc = {.usUsagePage = HID_USAGE_PAGE_GENERIC,
                                                        .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                                                        .dwFlags = RIDEV_NOLEGACY |
                                                                   RIDEV_INPUTSINK |
                                                                   RIDEV_NOHOTKEYS | RIDEV_APPKEYS,
                                                        .hwndTarget = handle};
                    if(RegisterRawInputDevices(&raw_keyboard_desc, 1, sizeof(RAWINPUTDEVICE)) !=
                       TRUE)
                        return -1;
                }
                break;
                case WM_INPUT:
                {
                    RAWINPUT raw_input;
                    UINT raw_input_size = sizeof(raw_input);
                    if(GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param),
                                       RID_INPUT,
                                       &raw_input,
                                       &raw_input_size,
                                       sizeof(raw_input.header)) != (UINT)-1)
                    {
                        if(raw_input.header.dwType == RIM_TYPEKEYBOARD)
                        {
                            const RAWKEYBOARD& data = raw_input.data.keyboard;

#pragma message("UPDATE REPEAT COUNT!")
#pragma message("UPDATE KEYBOARD STATE!")
#pragma message("ToUnicodeEx!!!")

                            //ignore out of bounds
                            //if(data.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE ||
                            //   data.VKey >= UCHAR_MAX)
                            //    return 0;

                            //get scancode properties
                            bool is_pressed = !(data.Flags & RI_KEY_BREAK);
                            ScanCode scancode = data.MakeCode & 0x7F;
                            if(data.Flags & RI_KEY_E0)
                                scancode |= 0xE0'00;
                            else if(data.Flags & RI_KEY_E1)
                                scancode |= 0xE1'00;

#pragma message("CHECK Numeric /!")
                            //PS/2 Set 1 check shit...
                            /*
                            1. Insert, Delete, LeftArrow, Home, End, UpArrow, DownArrow, PageUp, PageDown, RightArrow
                            have format: any precede follow scancodes: 0xE0'2A, 0xE0'AA, 0xE0'B6, 0xE0'36
                            + make code. -> Just ignore precede or followe scancodes
                            
                            2. Numeric /:
                                Make: E0'35
                                (LShift) + Make: (E0'AA E0'35) E0'35
                                (RShift) + Make: (E0'B6 E0'35) E0'35
                                (RShift + RShift) + Make: (E0'AA E0'B6 ) E0'35

                            3. Key 124(PrintScreen on US): 
                                Make: E0'2A E0'37
                                (LCtrl or RCtrl + LShift or RShift) + Make: (E0 37) E0'2A E0'37
                                (LAlt or Ralt) + Make: (54) E0'2A E0'37:
                                -> 

                                if(scancode == 0x54 || (scancode == 0xE0'37 && preceded_e02a == false))
                                    return 0;

                                if(scancode == 0xE0'2A)
                                {
                                    preceded_e02a = true;
                                    return 0;
                                }
                                else if(scancode == 0xE0'37)//our scancode
                                {
                                    ...
                                }
                                
                                preceded_e02a = false;

                            4. Ley 126(Pause/Break on US):
                                Make: E1'1D 45 [(E1'9D C5) -> we can ignore this?]
                                LCtrl or RCtrl + Make: (E0'46 E0'C6) E1'1D 45 [(E1'9D C5) -> we can ignore this?]
                            */

                            //skip PrintScreen prefixes
                            //if(scancode == 0x54 ||
                            //   (scancode == 0xE0'37 &&
                            //    HOOK_WIN_SYS->preceded_scancode != PrecededScanCode::E02A))
                            //    return 0;

                            //set preceded 0xE0'2A
                            if(scancode == 0xE0'2A)
                            {
                                HOOK_WIN_SYS->preceded_scancode = PrecededScanCode::E02A;
                                return 0;
                            }
                            else if(scancode == 0xE1'1D)
                            {
                                HOOK_WIN_SYS->preceded_scancode = PrecededScanCode::E11D;
                                return 0;
                            }

                            if(scancode == 0xE0'37 && HOOK_WIN_SYS->preceded_scancode ==
                                                          PrecededScanCode::E02A) //PrintScreen
                            {
                                scancode |= static_cast<ScanCode>(HOOK_WIN_SYS->preceded_scancode)
                                            << 16;
                            }
                            else if(scancode == 0x45 && HOOK_WIN_SYS->preceded_scancode ==
                                                            PrecededScanCode::E11D) //Pause/Break
                            {
                                scancode |= static_cast<ScanCode>(HOOK_WIN_SYS->preceded_scancode)
                                            << 8;
                            }

                            HWND current_focus = GetFocus();
                            if(current_focus)
                            {
                                Window* window = reinterpret_cast<Window*>(
                                    GetWindowLongPtrW(current_focus, GWLP_USERDATA));
                                if(window)
                                {
                                    if(is_pressed)
                                    {
#pragma message("UPDATE REPEAT COUNT!")
                                        HOOK_WIN_SYS->PushEvent(Event{
                                            .data = {.keyboard_key_pressed =
                                                         KeyboardKeyPressedEvent{
                                                             .timestamp_ms = GetEventTimestamp(),
                                                             .scancode = scancode,
                                                             .raw_key = data.VKey,
                                                             .modifiers = GetModifierFlags(),
                                                             .repeat_count = 0}},
                                            .id = ClassID<KeyboardKeyPressedEvent>::ID,
                                            .window = window});
                                    }
                                    else
                                    {
                                        HOOK_WIN_SYS->PushEvent(Event{
                                            .data = {.keyboard_key_released =
                                                         KeyboardKeyReleasedEvent{
                                                             .timestamp_ms = GetEventTimestamp(),
                                                             .scancode = scancode,
                                                             .raw_key = data.VKey,
                                                             .modifiers = GetModifierFlags()}},
                                            .id = ClassID<KeyboardKeyReleasedEvent>::ID,
                                            .window = window});
                                    }
                                }
                            }

                            HOOK_WIN_SYS->preceded_scancode = PrecededScanCode::None;

                            //skip preceded and follow scancodes for Note 1. and 2.
                            //constexpr static ScanCode PRECEDE_FOLLOW_SCANCODES[] = {0xE0'AA,
                            //                                                        0xE0'B6,
                            //                                                        0xE0'36};
                            //if(auto it = std::find(std::begin(PRECEDE_FOLLOW_SCANCODES),
                            //                       std::end(PRECEDE_FOLLOW_SCANCODES),
                            //                       scancode);
                            //   it == std::end(PRECEDE_FOLLOW_SCANCODES))
                            //{
                            //    return 0; //skip these scancodes
                            //}

                            //Update keyboard state

                            //Send key message to the active window

                            //Send char message to the active window

                            return 0;
                        }
                    }
                }
                break;
            }

            return DefWindowProcW(handle, message, w_param, l_param);
        }

        LRESULT CALLBACK WindowSubsystem::ShellProc(int code, WPARAM w_param, LPARAM l_param)
        {
            if(code == HSHELL_LANGUAGE)
            {
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

            const wchar_t* raw_input_window_class = nullptr;
            HWND _raw_input_hwnd = nullptr;
            HHOOK _shell_hook = nullptr;
            const wchar_t* window_class = nullptr;

            Core::ScopedCall cleanup(
                [instance = GetModuleHandleW(nullptr),
                 &raw_input_window_class,
                 &_raw_input_hwnd,
                 &_shell_hook,
                 &window_class]()
                {
                    if(raw_input_window_class)
                        UnregisterClassW(raw_input_window_class, instance);

                    if(_raw_input_hwnd)
                        DestroyWindow(_raw_input_hwnd);

                    if(_shell_hook)
                        UnhookWindowsHookEx(_shell_hook);

                    if(window_class)
                        UnregisterClassW(window_class, instance);
                });

            HOOK_WIN_SYS = this;

            //register raw input window class
            WNDCLASSEXW raw_input_wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                               .style = 0,
                                               .lpfnWndProc = WindowSubsystem::RawInputWindowProc,
                                               .cbClsExtra = 0,
                                               .cbWndExtra = 0,
                                               .hInstance = instance,
                                               .hIcon = nullptr,
                                               .hCursor = nullptr,
                                               .hbrBackground = nullptr,
                                               .lpszMenuName = nullptr,
                                               .lpszClassName = WIN32_RAW_INPUT_WINDOW_CLASS_NAME,
                                               .hIconSm = nullptr};

            if(RegisterClassExW(&raw_input_wnd_class) == 0)
                Core::System::ThrowLastError();

            raw_input_window_class = WIN32_RAW_INPUT_WINDOW_CLASS_NAME;

            //create raw input window
            _raw_input_hwnd = CreateWindowExW(0,
                                              WIN32_RAW_INPUT_WINDOW_CLASS_NAME,
                                              nullptr,
                                              0,
                                              CW_USEDEFAULT,
                                              CW_USEDEFAULT,
                                              0,
                                              0,
                                              nullptr,
                                              nullptr,
                                              instance,
                                              nullptr);

            if(_raw_input_hwnd == nullptr)
                Core::System::ThrowLastError();

            //create shell hook
            this->UpdateKeyboardLayouts();

            _shell_hook = SetWindowsHookExW(WH_SHELL,
                                            WindowSubsystem::ShellProc,
                                            nullptr,
                                            Core::System::GetMainThreadID());

            if(_shell_hook == nullptr)
                Core::System::ThrowLastError();

            //register window class
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

            window_class = Window::WIN32_WINDOW_CLASS_NAME;

            cleanup.Drop();

            this->raw_input_hwnd = _raw_input_hwnd;
            this->shell_hook = _shell_hook;
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(Window::WIN32_WINDOW_CLASS_NAME, instance);
            UnhookWindowsHookEx(shell_hook);
            DestroyWindow(raw_input_hwnd);
            UnregisterClassW(WIN32_RAW_INPUT_WINDOW_CLASS_NAME, instance);
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

            constexpr static std::pair<StableScanCode, std::string_view> STABLE_SCANCODE_PAIRS[] = {
                std::pair{StableScanCode::BackSpace, "BackSpace"},
                std::pair{StableScanCode::Tab, "Tab"},
                std::pair{StableScanCode::CapsLock, "CapsLock"},
                std::pair{StableScanCode::Enter, "Enter"},
                std::pair{StableScanCode::LeftShift, "LeftShift"},
                std::pair{StableScanCode::ABTN_C1, "ABTN_C1"},
                std::pair{StableScanCode::RightShift, "RightShift"},
                std::pair{StableScanCode::LeftControl, "LeftControl"},
                std::pair{StableScanCode::LeftAlt, "LeftAlt"},
                std::pair{StableScanCode::Space, "Space"},
                std::pair{StableScanCode::RightAlt, "RightAlt"},
                std::pair{StableScanCode::RightControl, "RightControl"},
                std::pair{StableScanCode::Insert, "Insert"},
                std::pair{StableScanCode::Delete, "Delete"},
                std::pair{StableScanCode::LeftArrow, "LeftArrow"},
                std::pair{StableScanCode::Home, "Home"},
                std::pair{StableScanCode::End, "End"},
                std::pair{StableScanCode::UpArrow, "UpArrow"},
                std::pair{StableScanCode::DownArrow, "DownArrow"},
                std::pair{StableScanCode::PageUp, "PageUp"},
                std::pair{StableScanCode::PageDown, "PageDown"},
                std::pair{StableScanCode::RightArrow, "RightArrow"},
                std::pair{StableScanCode::NumLock, "NumLock"},
                std::pair{StableScanCode::Num7, "Num7"},
                std::pair{StableScanCode::Num4, "Num4"},
                std::pair{StableScanCode::Num1, "Num1"},
                std::pair{StableScanCode::NumDiv, "NumDiv"},
                std::pair{StableScanCode::Num8, "Num8"},
                std::pair{StableScanCode::Num5, "Num5"},
                std::pair{StableScanCode::Num2, "Num2"},
                std::pair{StableScanCode::Num0, "Num0"},
                std::pair{StableScanCode::NumMul, "NumMul"},
                std::pair{StableScanCode::Num9, "Num9"},
                std::pair{StableScanCode::Num6, "Num6"},
                std::pair{StableScanCode::Num3, "Num3"},
                std::pair{StableScanCode::NumPeriod, "NumPeriod"},
                std::pair{StableScanCode::NumMin, "NumMin"},
                std::pair{StableScanCode::NumAdd, "NumAdd"},
                std::pair{StableScanCode::ABTN_C2, "ABTN_C2"},
                std::pair{StableScanCode::NumEnter, "NumEnter"},
                std::pair{StableScanCode::Esc, "Esc"},
                std::pair{StableScanCode::F1, "F1"},
                std::pair{StableScanCode::F2, "F2"},
                std::pair{StableScanCode::F3, "F3"},
                std::pair{StableScanCode::F4, "F4"},
                std::pair{StableScanCode::F5, "F5"},
                std::pair{StableScanCode::F6, "F6"},
                std::pair{StableScanCode::F7, "F7"},
                std::pair{StableScanCode::F8, "F8"},
                std::pair{StableScanCode::F9, "F9"},
                std::pair{StableScanCode::F10, "F10"},
                std::pair{StableScanCode::F11, "F11"},
                std::pair{StableScanCode::F12, "F12"},
                std::pair{StableScanCode::PrintScreen, "PrintScreen"},
                std::pair{StableScanCode::ScrollLock, "ScrollLock"},
                std::pair{StableScanCode::Pause, "Pause"},
                std::pair{StableScanCode::LeftMeta, "LeftMeta"},
                std::pair{StableScanCode::RightMeta, "RightMeta"},
                std::pair{StableScanCode::Menu, "Menu"},
                std::pair{StableScanCode::Power, "Power"},
                std::pair{StableScanCode::Sleep, "Sleep"},
                std::pair{StableScanCode::Wake, "Wake"},
                std::pair{StableScanCode::Kana, "Kana"},
                std::pair{StableScanCode::SBCSCHAR, "SBCSCHAR"},
                std::pair{StableScanCode::Convert, "Convert"},
                std::pair{StableScanCode::NonConvert, "NonConvert"},
            };

            auto prev_keyboard_layout = GetKeyboardLayout(Core::System::GetMainThreadID());
            for(auto& desc: new_keyboard_layouts)
            {
                ActivateKeyboardLayout(desc.layout, 0);

                for(const auto& [sc, name]: STABLE_SCANCODE_PAIRS)
                {
                    ScanCode scancode = static_cast<ScanCode>(sc);
                    auto [it, _] =
                        desc.scancode_to_string_mapping.insert(std::pair{scancode, name});
                    desc.string_to_scancode_mapping.insert({it->second, scancode});
                }

                for(int vk = 1; vk < 256; vk++)
                {
                    auto scancode = MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC, desc.layout);
                    if(scancode != 0)
                    {
                        wchar_t character = MapVirtualKeyExW(vk, MAPVK_VK_TO_CHAR, desc.layout);
                        if(character != 0)
                        {
                            auto name = Core::System::WideToUTF8({&character, 1});

                            auto [it, inserted] =
                                desc.scancode_to_string_mapping.insert(std::pair{scancode, name});

                            if(inserted)
                                desc.string_to_scancode_mapping.insert({it->second, scancode});
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