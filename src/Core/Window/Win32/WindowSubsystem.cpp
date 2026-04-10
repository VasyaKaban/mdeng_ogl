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
        }

        WindowSubsystem::~WindowSubsystem()
        {
            UnregisterClassW(Window::WIN32_WINDOW_CLASS_NAME, instance);
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

        KeyCodeDesc WindowSubsystem::GetKeyCodeDesc(RawKeyCode code) const noexcept
        {
            constexpr static char NUMBERS_STRING[] = "0123456789";
            constexpr static char LETTERS_STRING[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

            KeyCodeDesc desc = {};

            if(code >= 0x30 && code <= 0x39)
            {
                desc.utf32_char = code;
                desc.name = {NUMBERS_STRING + (code - 0x30), 1};
            }
            else if(code >= 0x41 && code <= 0x5A)
            {
                desc.utf32_char = code;
                desc.name = {LETTERS_STRING + (code - 0x41), 1};
            }

            switch(code)
            {
#error Maybe map OEM to list of strings???
                //case VK_LBUTTON: //0x01
                //    desc.utf32_char = "LBUTTON";
                //    desc.name = "LBUTTON";
                //    break;
                //case VK_RBUTTON: //0x02
                //    desc.utf32_char = "RBUTTON";
                //    desc.name = "RBUTTON";
                //    break;
                case VK_CANCEL: //0x03
                    desc.utf32_char = U'\0';
                    desc.name = "Cancel"; //XKB_KEY_Cancel
                    break;
                //case VK_MBUTTON: //0x04    /* NOT contiguous with L & RBUTTON */
                //    desc.utf32_char = "MBUTTON";
                //    desc.name = "MBUTTON";
                //    break;
                //case VK_XBUTTON1: //0x05    /* NOT contiguous with L & RBUTTON */
                //    desc.utf32_char = "XBUTTON1";
                //    desc.name = "XBUTTON1";
                //    break;
                //case VK_XBUTTON2: //0x06    /* NOT contiguous with L & RBUTTON */
                //    desc.utf32_char = "XBUTTON2";
                //    desc.name = "XBUTTON2";
                //    break;
                //Reserved -> 0x07
                case VK_BACK: //0x08
                    desc.utf32_char = U'\0';
                    desc.name = "BackSpace"; //XKB_KEY_BackSpace
                    break;
                case VK_TAB: //0x09
                    desc.utf32_char = U'\0';
                    desc.name = "Tab"; //XKB_KEY_Tab
                    break;
                //Reserved -> 0x0A-0B
                case VK_CLEAR: //0x0C
                    desc.utf32_char = U'\0';
                    desc.name = "Clear"; //XKB_KEY_Clear
                    break;
                case VK_RETURN: //0x0D
                    desc.utf32_char = U'\0';
                    desc.name = "Return"; //XKB_KEY_Return
                    break;
//Reserved ->  	0x0E-0F
#pragma message("Maybe map them to the left side(or just use non-extended names)?")
                //case VK_SHIFT: //0x10
                //    desc.utf32_char = "SHIFT";
                //    desc.name = "SHIFT";
                //    break;
                //case VK_CONTROL: //0x11
                //    desc.utf32_char = "CONTROL";
                //    desc.name = "CONTROL";
                //    break;
                //case VK_MENU: //0x12
                //    desc.utf32_char = "MENU";
                //    desc.name = "MENU";
                //    break;
                case VK_PAUSE: //0x13
                    desc.utf32_char = U'\0';
                    desc.name = "Pause"; //XKB_KEY_Pause
                    break;
                case VK_CAPITAL: //0x14
                    desc.utf32_char = U'\0';
                    desc.name = "Caps_Lock"; //XKB_KEY_Caps_Lock
                    break;
                case VK_KANA: //0x15
#pragma message("?")
                    desc.utf32_char = U'\0';
                    desc.name = "Hiragana_Katakana"; //XKB_KEY_Hiragana_Katakana
                    break;
                //case VK_HANGEUL: //0x15  /* old name - should be here for compatibility */
                //    desc.utf32_char = "HANGEUL";
                //    desc.name = "HANGEUL"; //
                //    break;
                case VK_HANGUL: //0x15
                    desc.utf32_char = U'\0';
                    desc.name = "Hangul"; //XKB_KEY_Hangul
                    break;
                case VK_IME_ON: //0x16
                    desc.utf32_char = "IME_ON";
                    desc.name = "IME_ON";
                    break;
                case VK_JUNJA: //0x17
                    desc.utf32_char = "JUNJA";
                    desc.name = "JUNJA";
                    break;
                case VK_FINAL: //0x18
                    desc.utf32_char = "FINAL";
                    desc.name = "FINAL";
                    break;
                case VK_HANJA: //0x19
                    desc.utf32_char = "HANJA";
                    desc.name = "HANJA";
                    break;
                case VK_KANJI: //0x19
                    desc.utf32_char = "KANJI";
                    desc.name = "KANJI";
                    break;
                case VK_IME_OFF: //0x1A
                    desc.utf32_char = "IME_OFF";
                    desc.name = "IME_OFF";
                    break;
                case VK_ESCAPE: //0x1B
                    desc.utf32_char = U'\0';
                    desc.name = "Escape"; //XKB_KEY_Escape
                    break;
                case VK_CONVERT: //0x1C
                    desc.utf32_char = "CONVERT";
                    desc.name = "CONVERT";
                    break;
                case VK_NONCONVERT: //0x1D
                    desc.utf32_char = "NONCONVERT";
                    desc.name = "NONCONVERT";
                    break;
                case VK_ACCEPT: //0x1E
                    desc.utf32_char = "ACCEPT";
                    desc.name = "ACCEPT";
                    break;
                case VK_MODECHANGE: //0x1F
                    desc.utf32_char = "MODECHANGE";
                    desc.name = "MODECHANGE";
                    break;
                case VK_SPACE: //0x20
                    desc.utf32_char = U'\x20';
                    desc.name = "space"; //XKB_KEY_space
                    break;
                case VK_PRIOR: //0x21
                    desc.utf32_char = U'\0';
                    desc.name = "Prior"; //XKB_KEY_Prior
                    break;
                case VK_NEXT: //0x22
                    desc.utf32_char = U'\0';
                    desc.name = "Next"; //XKB_KEY_Next
                    break;
                case VK_END: //0x23
                    desc.utf32_char = U'\0';
                    desc.name = "End"; //XKB_KEY_End
                    break;
                case VK_HOME: //0x24
                    desc.utf32_char = U'\0';
                    desc.name = "Home"; //XKB_KEY_Home
                    break;
                case VK_LEFT: //0x25
                    desc.utf32_char = U'\0';
                    desc.name = "Left"; //XKB_KEY_Left
                    break;
                case VK_UP: //0x26
                    desc.utf32_char = U'\0';
                    desc.name = "Up"; //XKB_KEY_Up
                    break;
                case VK_RIGHT: //0x27
                    desc.utf32_char = U'\0';
                    desc.name = "Right"; //XKB_KEY_Right
                    break;
                case VK_DOWN: //0x28
                    desc.utf32_char = U'\0';
                    desc.name = "Down"; //XKB_KEY_Down
                    break;
                case VK_SELECT: //0x29
                    desc.utf32_char = U'\0';
                    desc.name = "Select"; //XKB_KEY_Select
                    break;
                case VK_PRINT: //0x2A
                    desc.utf32_char = "PRINT";
                    desc.name = "PRINT";
                    break;
                case VK_EXECUTE: //0x2B
                    desc.utf32_char = U'\0';
                    desc.name = "Execute"; //XKB_KEY_Execute
                    break;
                case VK_SNAPSHOT: //0x2C
                    desc.utf32_char = U'\0';
                    desc.name = "Print"; //XKB_KEY_Print
                    break;
                case VK_INSERT: //0x2D
                    desc.utf32_char = U'\0';
                    desc.name = "Insert"; //XKB_KEY_Insert
                    break;
                case VK_DELETE: //0x2E
                    desc.utf32_char = U'\0';
                    desc.name = "Delete"; //XKB_KEY_Delete
                    break;
                case VK_HELP: //0x2F
                    desc.utf32_char = U'\0';
                    desc.name = "Help"; //XKB_KEY_Help
                    break;
                    //0-9 -> 0x30-0x39
                    //Reserved -> 0x3A-40
                    //A-Z -> 0x41-0x5A
                case VK_LWIN: //0x5B
                    desc.utf32_char = U'\0';
                    desc.name = "Meta_L"; //XKB_KEY_Meta_L
                    break;
                case VK_RWIN: //0x5C
                    desc.utf32_char = U'\0';
                    desc.name = "Meta_R"; //XKB_KEY_Meta_R
                    break;
                case VK_APPS: //0x5D
                    desc.utf32_char = U'\0';
                    desc.name = "Menu"; //XKB_KEY_Menu
                    break;
                //Reserved -> 0x5E
                case VK_SLEEP: //0x5F
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Sleep"; //XKB_KEY_XF86Sleep
                    break;
                case VK_NUMPAD0: //0x60
                    desc.utf32_char = U'\x30';
                    desc.name = "KP_0"; //XKB_KEY_KP_0
                    break;
                case VK_NUMPAD1: //0x61
                    desc.utf32_char = U'\x31';
                    desc.name = "KP_1"; //XKB_KEY_KP_1
                    break;
                case VK_NUMPAD2: //0x62
                    desc.utf32_char = U'\x32';
                    desc.name = "KP_2"; //XKB_KEY_KP_2
                    break;
                case VK_NUMPAD3: //0x63
                    desc.utf32_char = U'\x33';
                    desc.name = "KP_3"; //XKB_KEY_KP_3
                    break;
                case VK_NUMPAD4: //0x64
                    desc.utf32_char = U'\x34';
                    desc.name = "KP_4"; //XKB_KEY_KP_4
                    break;
                case VK_NUMPAD5: //0x65
                    desc.utf32_char = U'\x35';
                    desc.name = "KP_5"; //XKB_KEY_KP_5
                    break;
                case VK_NUMPAD6: //0x66
                    desc.utf32_char = U'\x36';
                    desc.name = "KP_6"; //XKB_KEY_KP_6
                    break;
                case VK_NUMPAD7: //0x67
                    desc.utf32_char = U'\x37';
                    desc.name = "KP_7"; //XKB_KEY_KP_7
                    break;
                case VK_NUMPAD8: //0x68
                    desc.utf32_char = U'\x38';
                    desc.name = "KP_8"; //XKB_KEY_KP_8
                    break;
                case VK_NUMPAD9: //0x69
                    desc.utf32_char = U'\x39';
                    desc.name = "KP_9"; //XKB_KEY_KP_9
                    break;
                case VK_MULTIPLY: //0x6A
                    desc.utf32_char = U'\xD7';
                    desc.name = "multiply"; //XKB_KEY_multiply
                    break;
                case VK_ADD: //0x6B
                    desc.utf32_char = U'\x2B';
                    desc.name = "plus"; //XKB_KEY_plus
                    break;
                case VK_SEPARATOR: //0x6C
                    desc.utf32_char = U'\x2C';
                    desc.name = "comma"; //XKB_KEY_comma
                    break;
                case VK_SUBTRACT: //0x6D
                    desc.utf32_char = U'\x2D';
                    desc.name = "minus"; //XKB_KEY_minus
                    break;
                case VK_DECIMAL: //0x6E
                    desc.utf32_char = U'\x2E';
                    desc.name = "period"; //XKB_KEY_period
                    break;
                case VK_DIVIDE: //0x6F
                    desc.utf32_char = U'\x2F';
                    desc.name = "slash"; //XKB_KEY_slash
                    break;
                case VK_F1: //0x70
                    desc.utf32_char = U'\0';
                    desc.name = "F1"; //XKB_KEY_F1
                    break;
                case VK_F2: //0x71
                    desc.utf32_char = U'\0';
                    desc.name = "F2"; //XKB_KEY_F2
                    break;
                case VK_F3: //0x72
                    desc.utf32_char = U'\0';
                    desc.name = "F3"; //XKB_KEY_F3
                    break;
                case VK_F4: //0x73
                    desc.utf32_char = U'\0';
                    desc.name = "F4"; //XKB_KEY_F4
                    break;
                case VK_F5: //0x74
                    desc.utf32_char = U'\0';
                    desc.name = "F5"; //XKB_KEY_F5
                    break;
                case VK_F6: //0x75
                    desc.utf32_char = U'\0';
                    desc.name = "F6"; //XKB_KEY_F6
                    break;
                case VK_F7: //0x76
                    desc.utf32_char = U'\0';
                    desc.name = "F7"; //XKB_KEY_F7
                    break;
                case VK_F8: //0x77
                    desc.utf32_char = U'\0';
                    desc.name = "F8"; //XKB_KEY_F8
                    break;
                case VK_F9: //0x78
                    desc.utf32_char = U'\0';
                    desc.name = "F9"; //XKB_KEY_F9
                    break;
                case VK_F10: //0x79
                    desc.utf32_char = U'\0';
                    desc.name = "F10"; //XKB_KEY_F10
                    break;
                case VK_F11: //0x7A
                    desc.utf32_char = U'\0';
                    desc.name = "F11"; //XKB_KEY_F11
                    break;
                case VK_F12: //0x7B
                    desc.utf32_char = U'\0';
                    desc.name = "F12"; //XKB_KEY_F12
                    break;
                case VK_F13: //0x7C
                    desc.utf32_char = U'\0';
                    desc.name = "F13"; //XKB_KEY_F13
                    break;
                case VK_F14: //0x7D
                    desc.utf32_char = U'\0';
                    desc.name = "F14"; //XKB_KEY_F14
                    break;
                case VK_F15: //0x7E
                    desc.utf32_char = U'\0';
                    desc.name = "F15"; //XKB_KEY_F15
                    break;
                case VK_F16: //0x7F
                    desc.utf32_char = U'\0';
                    desc.name = "F16"; //XKB_KEY_F16
                    break;
                case VK_F17: //0x80
                    desc.utf32_char = U'\0';
                    desc.name = "F17"; //XKB_KEY_F17
                    break;
                case VK_F18: //0x81
                    desc.utf32_char = U'\0';
                    desc.name = "F18"; //XKB_KEY_F18
                    break;
                case VK_F19: //0x82
                    desc.utf32_char = U'\0';
                    desc.name = "F19"; //XKB_KEY_F19
                    break;
                case VK_F20: //0x83
                    desc.utf32_char = U'\0';
                    desc.name = "F20"; //XKB_KEY_F20
                    break;
                case VK_F21: //0x84
                    desc.utf32_char = U'\0';
                    desc.name = "F21"; //XKB_KEY_F21
                    break;
                case VK_F22: //0x85
                    desc.utf32_char = U'\0';
                    desc.name = "F22"; //XKB_KEY_F22
                    break;
                case VK_F23: //0x86
                    desc.utf32_char = U'\0';
                    desc.name = "F23"; //XKB_KEY_F23
                    break;
                case VK_F24: //0x87
                    desc.utf32_char = U'\0';
                    desc.name = "F24"; //XKB_KEY_F24
                    break;
#pragma message("Gamepad???")
                /*case VK_NAVIGATION_VIEW: //0x88
                    desc.utf32_char = "NAVIGATION_VIEW";
                    desc.name = "NAVIGATION_VIEW";
                    break;
                case VK_NAVIGATION_MENU: //0x89
                    desc.utf32_char = "NAVIGATION_MENU";
                    desc.name = "NAVIGATION_MENU";
                    break;
                case VK_NAVIGATION_UP: //0x8A
                    desc.utf32_char = "NAVIGATION_UP";
                    desc.name = "NAVIGATION_UP";
                    break;
                case VK_NAVIGATION_DOWN: //0x8B
                    desc.utf32_char = "NAVIGATION_DOWN";
                    desc.name = "NAVIGATION_DOWN";
                    break;
                case VK_NAVIGATION_LEFT: //0x8C
                    desc.utf32_char = "NAVIGATION_LEFT";
                    desc.name = "NAVIGATION_LEFT";
                    break;
                case VK_NAVIGATION_RIGHT: //0x8D
                    desc.utf32_char = "NAVIGATION_RIGHT";
                    desc.name = "NAVIGATION_RIGHT";
                    break;
                case VK_NAVIGATION_ACCEPT: //0x8E
                    desc.utf32_char = "NAVIGATION_ACCEPT";
                    desc.name = "NAVIGATION_ACCEPT";
                    break;
                case VK_NAVIGATION_CANCEL: //0x8F
                    desc.utf32_char = "NAVIGATION_CANCEL";
                    desc.name = "NAVIGATION_CANCEL";
                    break;*/
                case VK_NUMLOCK: //0x90
                    desc.utf32_char = U'\0';
                    desc.name = "Num_Lock"; //XKB_KEY_Num_Lock
                    break;
                case VK_SCROLL: //0x91
                    desc.utf32_char = U'\0';
                    desc.name = "Scroll_Lock"; //XKB_KEY_Scroll_Lock
                    break;
                case VK_OEM_NEC_EQUAL: //0x92
                    desc.utf32_char = U'\x3D';
                    desc.name = "KP_Equal"; //XKB_KEY_KP_Equal
                    break;
                /*case VK_OEM_FJ_JISHO: //0x92
                    desc.utf32_char = "OEM_FJ_JISHO";
                    desc.name = "OEM_FJ_JISHO";
                    break;
                case VK_OEM_FJ_MASSHOU: //0x93
                    desc.utf32_char = "OEM_FJ_MASSHOU";
                    desc.name = "OEM_FJ_MASSHOU";
                    break;
                case VK_OEM_FJ_TOUROKU: //0x94
                    desc.utf32_char = "OEM_FJ_TOUROKU";
                    desc.name = "OEM_FJ_TOUROKU";
                    break;
                case VK_OEM_FJ_LOYA: //0x95
                    desc.utf32_char = "OEM_FJ_LOYA";
                    desc.name = "OEM_FJ_LOYA";
                    break;
                case VK_OEM_FJ_ROYA: //0x96
                    desc.utf32_char = "OEM_FJ_ROYA";
                    desc.name = "OEM_FJ_ROYA";
                    break;*/
                case VK_LSHIFT: //0xA0
                    desc.utf32_char = U'\0';
                    desc.name = "Shift_L"; //XKB_KEY_Shift_L
                    break;
                case VK_RSHIFT: //0xA1
                    desc.utf32_char = U'\0';
                    desc.name = "Shift_R"; //XKB_KEY_Shift_R
                    break;
                case VK_LCONTROL: //0xA2
                    desc.utf32_char = U'\0';
                    desc.name = "Control_L"; //XKB_KEY_Control_L
                    break;
                case VK_RCONTROL: //0xA3
                    desc.utf32_char = U'\0';
                    desc.name = "Control_R"; //XKB_KEY_Control_R
                    break;
                case VK_LMENU: //0xA4
                    desc.utf32_char = U'\0';
                    desc.name = "Alt_L"; //XKB_KEY_Alt_L
                    break;
                case VK_RMENU: //0xA5
                    desc.utf32_char = U'\0';
                    desc.name = "Alt_R"; //XKB_KEY_Alt_R
                    break;
                case VK_BROWSER_BACK: //0xA6
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Back"; //XKB_KEY_XF86Back
                    break;
                case VK_BROWSER_FORWARD: //0xA7
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Forward"; //XKB_KEY_XF86Forward
                    break;
                case VK_BROWSER_REFRESH: //0xA8
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Refresh"; //XKB_KEY_XF86Refresh
                    break;
                case VK_BROWSER_STOP: //0xA9
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Stop"; //XKB_KEY_XF86Stop
                    break;
                case VK_BROWSER_SEARCH: //0xAA
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Search"; //XKB_KEY_XF86Search
                    break;
                case VK_BROWSER_FAVORITES: //0xAB
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Favorites"; //XKB_KEY_XF86Favorites
                    break;
                case VK_BROWSER_HOME: //0xAC
                    desc.utf32_char = U'\0';
                    desc.name = "XF86HomePage"; //XKB_KEY_XF86HomePage
                    break;
                case VK_VOLUME_MUTE: //0xAD
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioMute"; //XKB_KEY_XF86AudioMute
                    break;
                case VK_VOLUME_DOWN: //0xAE
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioLowerVolume"; //XKB_KEY_XF86AudioLowerVolume
                    break;
                case VK_VOLUME_UP: //0xAF
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioRaiseVolume"; //XKB_KEY_XF86AudioRaiseVolume
                    break;
                case VK_MEDIA_NEXT_TRACK: //0xB0
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioNext"; //XKB_KEY_XF86AudioNext
                    break;
                case VK_MEDIA_PREV_TRACK: //0xB1
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioPrev"; //XKB_KEY_XF86AudioPrev
                    break;
                case VK_MEDIA_STOP: //0xB2
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioStop"; //XKB_KEY_XF86AudioStop
                    break;
                case VK_MEDIA_PLAY_PAUSE: //0xB3
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioPause"; //XKB_KEY_XF86AudioPause
                    break;
                case VK_LAUNCH_MAIL: //0xB4
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Mail"; //XKB_KEY_XF86Mail
                    break;
                case VK_LAUNCH_MEDIA_SELECT: //0xB5
                    desc.utf32_char = U'\0';
                    desc.name = "XF86AudioMedia"; //XKB_KEY_XF86AudioMedia
                    break;
                case VK_LAUNCH_APP1: //0xB6
                    desc.utf32_char = U'\0';
                    desc.name = "XF86Launch1"; //XKB_KEY_XF86Launch1
                    break;
                case VK_LAUNCH_APP2: //0xB7
                    desc.utf32_char = U'\0';
                    ;
                    desc.name = "LAUNCH_APP2"; //XKB_KEY_XF86Launch2
                    break;
                case VK_OEM_1: //0xBA
                    desc.utf32_char = U'\x3A';
                    desc.name = "colon"; //XKB_KEY_colon
                    break;
                case VK_OEM_PLUS: //0xBB
                    desc.utf32_char = U'\x3D';
                    desc.name = "equal"; //XKB_KEY_equal
                    break;
                case VK_OEM_COMMA: //0xBC
                    desc.utf32_char = U'\x3C';
                    desc.name = "less"; //XKB_KEY_less
                    break;
                case VK_OEM_MINUS: //0xBD
                    desc.utf32_char = U'\x5F';
                    desc.name = "underscore"; //XKB_KEY_underscore
                    break;
                case VK_OEM_PERIOD: //0xBE
                    desc.utf32_char = U'\x3E';
                    desc.name = "greater"; //XKB_KEY_greater
                    break;
                case VK_OEM_2: //0xBF
                    desc.utf32_char = U'\x3F';
                    desc.name = "question"; //XKB_KEY_question
                    break;
                case VK_OEM_3: //0xC0
                    desc.utf32_char = U'\x60';
                    desc.name = "grave"; //XKB_KEY_grave
                    break;
#pragma message("Gamepad???")
                /*case VK_GAMEPAD_A: //0xC3
                    desc.utf32_char = "GAMEPAD_A";
                    desc.name = "GAMEPAD_A";
                    break;
                case VK_GAMEPAD_B: //0xC4
                    desc.utf32_char = "GAMEPAD_B";
                    desc.name = "GAMEPAD_B";
                    break;
                case VK_GAMEPAD_X: //0xC5
                    desc.utf32_char = "GAMEPAD_X";
                    desc.name = "GAMEPAD_X";
                    break;
                case VK_GAMEPAD_Y: //0xC6
                    desc.utf32_char = "GAMEPAD_Y";
                    desc.name = "GAMEPAD_Y";
                    break;
                case VK_GAMEPAD_RIGHT_SHOULDER: //0xC7
                    desc.utf32_char = "GAMEPAD_RIGHT_SHOULDER";
                    desc.name = "GAMEPAD_RIGHT_SHOULDER";
                    break;
                case VK_GAMEPAD_LEFT_SHOULDER: //0xC8
                    desc.utf32_char = "GAMEPAD_LEFT_SHOULDER";
                    desc.name = "GAMEPAD_LEFT_SHOULDER";
                    break;
                case VK_GAMEPAD_LEFT_TRIGGER: //0xC9
                    desc.utf32_char = "GAMEPAD_LEFT_TRIGGER";
                    desc.name = "GAMEPAD_LEFT_TRIGGER";
                    break;
                case VK_GAMEPAD_RIGHT_TRIGGER: //0xCA
                    desc.utf32_char = "GAMEPAD_RIGHT_TRIGGER";
                    desc.name = "GAMEPAD_RIGHT_TRIGGER";
                    break;
                case VK_GAMEPAD_DPAD_UP: //0xCB
                    desc.utf32_char = "GAMEPAD_DPAD_UP";
                    desc.name = "GAMEPAD_DPAD_UP";
                    break;
                case VK_GAMEPAD_DPAD_DOWN: //0xCC
                    desc.utf32_char = "GAMEPAD_DPAD_DOWN";
                    desc.name = "GAMEPAD_DPAD_DOWN";
                    break;
                case VK_GAMEPAD_DPAD_LEFT: //0xCD
                    desc.utf32_char = "GAMEPAD_DPAD_LEFT";
                    desc.name = "GAMEPAD_DPAD_LEFT";
                    break;
                case VK_GAMEPAD_DPAD_RIGHT: //0xCE
                    desc.utf32_char = "GAMEPAD_DPAD_RIGHT";
                    desc.name = "GAMEPAD_DPAD_RIGHT";
                    break;
                case VK_GAMEPAD_MENU: //0xCF
                    desc.utf32_char = "GAMEPAD_MENU";
                    desc.name = "GAMEPAD_MENU";
                    break;
                case VK_GAMEPAD_VIEW: //0xD0
                    desc.utf32_char = "GAMEPAD_VIEW";
                    desc.name = "GAMEPAD_VIEW";
                    break;
                case VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON: //0xD1
                    desc.utf32_char = "GAMEPAD_LEFT_THUMBSTICK_BUTTON";
                    desc.name = "GAMEPAD_LEFT_THUMBSTICK_BUTTON";
                    break;
                case VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON: //0xD2
                    desc.utf32_char = "GAMEPAD_RIGHT_THUMBSTICK_BUTTON";
                    desc.name = "GAMEPAD_RIGHT_THUMBSTICK_BUTTON";
                    break;
                case VK_GAMEPAD_LEFT_THUMBSTICK_UP: //0xD3
                    desc.utf32_char = "GAMEPAD_LEFT_THUMBSTICK_UP";
                    desc.name = "GAMEPAD_LEFT_THUMBSTICK_UP";
                    break;
                case VK_GAMEPAD_LEFT_THUMBSTICK_DOWN: //0xD4
                    desc.utf32_char = "GAMEPAD_LEFT_THUMBSTICK_DOWN";
                    desc.name = "GAMEPAD_LEFT_THUMBSTICK_DOWN";
                    break;
                case VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT: //0xD5
                    desc.utf32_char = "GAMEPAD_LEFT_THUMBSTICK_RIGHT";
                    desc.name = "GAMEPAD_LEFT_THUMBSTICK_RIGHT";
                    break;
                case VK_GAMEPAD_LEFT_THUMBSTICK_LEFT: //0xD6
                    desc.utf32_char = "GAMEPAD_LEFT_THUMBSTICK_LEFT";
                    desc.name = "GAMEPAD_LEFT_THUMBSTICK_LEFT";
                    break;
                case VK_GAMEPAD_RIGHT_THUMBSTICK_UP: //0xD7
                    desc.utf32_char = "GAMEPAD_RIGHT_THUMBSTICK_UP";
                    desc.name = "GAMEPAD_RIGHT_THUMBSTICK_UP";
                    break;
                case VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN: //0xD8
                    desc.utf32_char = "GAMEPAD_RIGHT_THUMBSTICK_DOWN";
                    desc.name = "GAMEPAD_RIGHT_THUMBSTICK_DOWN";
                    break;
                case VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT: //0xD9
                    desc.utf32_char = "GAMEPAD_RIGHT_THUMBSTICK_RIGHT";
                    desc.name = "GAMEPAD_RIGHT_THUMBSTICK_RIGHT";
                    break;
                case VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT: //0xDA
                    desc.utf32_char = "GAMEPAD_RIGHT_THUMBSTICK_LEFT";
                    desc.name = "GAMEPAD_RIGHT_THUMBSTICK_LEFT";
                    break;*/
                case VK_OEM_4: //0xDB
                    desc.utf32_char = U'\x7B';
                    desc.name = "braceleft"; //XKB_KEY_braceleft
                    break;
                case VK_OEM_5: //0xDC
                    desc.utf32_char = U'\x5C';
                    desc.name = "backslash"; //XKB_KEY_backslash
                    break;
                case VK_OEM_6: //0xDD
                    desc.utf32_char = U'\x7D';
                    desc.name = "braceright"; //XKB_KEY_braceright
                    break;
                case VK_OEM_7: //0xDE
                    desc.utf32_char = U'\x27';
                    desc.name = "apostrophe"; //XKB_KEY_apostrophe
                    break;
                case VK_OEM_8: //0xDF
                    desc.utf32_char = U'\x21';
                    desc.name = "exclam"; //XKB_KEY_exclam
                    break;
                case VK_OEM_AX: //0xE1
                    desc.utf32_char = "OEM_AX";
                    desc.name = "OEM_AX";
                    break;
                case VK_OEM_102: //0xE2
                    desc.utf32_char = U'\x7C';
                    desc.name = "bar"; //XKB_KEY_bar
                    break;
                case VK_ICO_HELP: //0xE3
                    desc.utf32_char = "ICO_HELP";
                    desc.name = "ICO_HELP";
                    break;
                case VK_ICO_00: //0xE4
                    desc.utf32_char = "ICO_00";
                    desc.name = "ICO_00";
                    break;
                case VK_PROCESSKEY: //0xE5
                    desc.utf32_char = "PROCESSKEY";
                    desc.name = "PROCESSKEY";
                    break;
                case VK_ICO_CLEAR: //0xE6
                    desc.utf32_char = "ICO_CLEAR";
                    desc.name = "ICO_CLEAR";
                    break;
                case VK_PACKET: //0xE7
                    desc.utf32_char = "PACKET";
                    desc.name = "PACKET";
                    break;
                case VK_OEM_RESET: //0xE9
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Reset"; //XKB_KEY_3270_Reset
                    break;
                case VK_OEM_JUMP: //0xEA
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Jump"; //XKB_KEY_3270_Jump
                    break;
                case VK_OEM_PA1: //0xEB
                    desc.utf32_char = U'\0';
                    desc.name = "3270_PA1"; //XKB_KEY_3270_PA1
                    break;
                case VK_OEM_PA2: //0xEC
                    desc.utf32_char = U'\0';
                    desc.name = "3270_PA2"; //XKB_KEY_3270_PA2
                    break;
                case VK_OEM_PA3: //0xED
                    desc.utf32_char = U'\0';
                    desc.name = "3270_PA3"; //XKB_KEY_3270_PA3
                    break;
                case VK_OEM_WSCTRL: //0xEE
                    desc.utf32_char = "OEM_WSCTRL";
                    desc.name = "OEM_WSCTRL";
                    break;
                case VK_OEM_CUSEL: //0xEF
                    desc.utf32_char = U'\0';
                    desc.name = "3270_CursorSelect"; //XKB_KEY_3270_CursorSelect
                    break;
                case VK_OEM_ATTN: //0xF0
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Attn"; //XKB_KEY_3270_Attn
                    break;
                case VK_OEM_FINISH: //0xF1
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Quit"; //XKB_KEY_3270_Quit
                    break;
                case VK_OEM_COPY: //0xF2
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Copy"; //XKB_KEY_3270_Copy
                    break;
                case VK_OEM_AUTO: //0xF3
                    desc.utf32_char = "OEM_AUTO";
                    desc.name = "OEM_AUTO";
                    break;
                case VK_OEM_ENLW: //0xF4
                    desc.utf32_char = "OEM_ENLW";
                    desc.name = "OEM_ENLW";
                    break;
                case VK_OEM_BACKTAB: //0xF5
                    desc.utf32_char = U'\0';
                    desc.name = "3270_BackTab"; //XKB_KEY_3270_BackTab
                    break;
                case VK_ATTN: //0xF6
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Attn"; //XKB_KEY_3270_Attn
                    break;
                case VK_CRSEL: //0xF7
                    desc.utf32_char = U'\0';
                    desc.name = "3270_CursorSelect"; //XKB_KEY_3270_CursorSelect
                    break;
                case VK_EXSEL: //0xF8
                    desc.utf32_char = U'\0';
                    desc.name = "3270_ExSelect"; //XKB_KEY_3270_ExSelect
                    break;
                case VK_EREOF: //0xF9
                    desc.utf32_char = U'\0';
                    desc.name = "3270_EraseEOF"; //XKB_KEY_3270_EraseEOF
                    break;
                case VK_PLAY: //0xFA
                    desc.utf32_char = U'\0';
                    desc.name = "3270_Play"; //XKB_KEY_3270_Play
                    break;
                case VK_ZOOM: //0xFB
                    desc.utf32_char = "ZOOM";
                    desc.name = "ZOOM";
                    break;
                //case VK_NONAME: //0xFC -> Reserved
                //    desc.utf32_char = "NONAME";
                //    desc.name = "NONAME";
                //    break;
                case VK_PA1: //0xFD
                    desc.utf32_char = U'\0';
                    desc.name = "3270_PA1"; //XKB_KEY_3270_PA1
                    break;
                case VK_OEM_CLEAR: //0xFE
                    desc.utf32_char = "OEM_CLEAR";
                    desc.name = "OEM_CLEAR";
                    break;
            }

            return desc;
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
    };
};