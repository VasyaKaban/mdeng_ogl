#include "KeyboardState.h"
#include "Core/Utils/ScopedCall.hpp"
#include "WindowSubsystem.h"
#include "hidusage.h"

namespace Core
{
    namespace Win32
    {
        static void RegisterRawKeyboardInput(HWND handle, KeyboardAccessState state)
        {
            const RAWINPUTDEVICE raw_keyboard_desc = {
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                .dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK |
                           (state == KeyboardAccessState::Exclusive ? DWORD(RIDEV_NOHOTKEYS) : 0),
                .hwndTarget = handle};
            if(RegisterRawInputDevices(&raw_keyboard_desc, 1, sizeof(RAWINPUTDEVICE)) != TRUE)
                Core::System::ThrowLastError();
        }

        static void UnregisterRawKeyboardInput()
        {
            const RAWINPUTDEVICE raw_keyboard_desc = {.usUsagePage = HID_USAGE_PAGE_GENERIC,
                                                      .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                                                      .dwFlags = RIDEV_REMOVE,
                                                      .hwndTarget = nullptr};
            RegisterRawInputDevices(&raw_keyboard_desc, 1, sizeof(RAWINPUTDEVICE));
        }

        const static std::unordered_map<ScanCode, KeyboardKey> STABLE_SCANCODES = {
            std::pair{0x1C, SpecialKey::Enter},
            std::pair{0x01, SpecialKey::Escape},
            std::pair{0x0E, SpecialKey::BackSpace},
            std::pair{0x0F, SpecialKey::Tab},
            std::pair{0x39, SpecialKey::Space},
            std::pair{0x3A, SpecialKey::CapsLock},
            std::pair{0x3B, SpecialKey::F1},
            std::pair{0x3C, SpecialKey::F2},
            std::pair{0x3D, SpecialKey::F3},
            std::pair{0x3E, SpecialKey::F4},
            std::pair{0x3F, SpecialKey::F5},
            std::pair{0x40, SpecialKey::F6},
            std::pair{0x41, SpecialKey::F7},
            std::pair{0x42, SpecialKey::F8},
            std::pair{0x43, SpecialKey::F9},
            std::pair{0x44, SpecialKey::F10},
            std::pair{0x57, SpecialKey::F11},
            std::pair{0x58, SpecialKey::F12},
            std::pair{0xE0'37, SpecialKey::PrintScreen},
            std::pair{0x54, SpecialKey::SysRq},
            std::pair{0x46, SpecialKey::ScrollLock},
            std::pair{0xE0'46, SpecialKey::Break},
            std::pair{0xE1'1D'45, SpecialKey::Pause},
            std::pair{0xE0'52, SpecialKey::Insert},
            std::pair{0xE0'47, SpecialKey::Home},
            std::pair{0xE0'49, SpecialKey::PageUp},
            std::pair{0xE0'53, SpecialKey::Delete},
            std::pair{0xE0'4F, SpecialKey::End},
            std::pair{0xE0'51, SpecialKey::PageDown},
            std::pair{0xE0'4D, SpecialKey::RightArrow},
            std::pair{0xE0'4B, SpecialKey::LeftArrow},
            std::pair{0xE0'50, SpecialKey::DownArrow},
            std::pair{0xE0'48, SpecialKey::UpArrow},
            std::pair{0x45, SpecialKey::NumLock},
            std::pair{0xE0'35, SpecialKey::KeypadDivide},
            std::pair{0x37, SpecialKey::KeypadMultiply},
            std::pair{0x4A, SpecialKey::KeypadSubtract},
            std::pair{0x4E, SpecialKey::KeypadAdd},
            std::pair{0xE0'1C, SpecialKey::KeypadEnter},
            std::pair{0x4F, SpecialKey::Keypad1},
            std::pair{0x50, SpecialKey::Keypad2},
            std::pair{0x51, SpecialKey::Keypad3},
            std::pair{0x4B, SpecialKey::Keypad4},
            std::pair{0x4C, SpecialKey::Keypad5},
            std::pair{0x4D, SpecialKey::Keypad6},
            std::pair{0x47, SpecialKey::Keypad7},
            std::pair{0x48, SpecialKey::Keypad8},
            std::pair{0x49, SpecialKey::Keypad9},
            std::pair{0x52, SpecialKey::Keypad0},
            std::pair{0x53, SpecialKey::KeypadPeriod},
            std::pair{0xE0'5D, SpecialKey::Menu},
            std::pair{0xE0'5E, SpecialKey::KeyboardPower},
            std::pair{0x59, SpecialKey::KeypadEqual},
            std::pair{0x64, SpecialKey::F13},
            std::pair{0x65, SpecialKey::F14},
            std::pair{0x66, SpecialKey::F15},
            std::pair{0x67, SpecialKey::F16},
            std::pair{0x68, SpecialKey::F17},
            std::pair{0x69, SpecialKey::F18},
            std::pair{0x6A, SpecialKey::F19},
            std::pair{0x6B, SpecialKey::F20},
            std::pair{0x6C, SpecialKey::F21},
            std::pair{0x6D, SpecialKey::F22},
            std::pair{0x6E, SpecialKey::F23},
            std::pair{0x76, SpecialKey::F24},
            std::pair{0x7E, SpecialKey::BrazilKeypad},
            std::pair{0x73, SpecialKey::Ro},
            std::pair{0x70, SpecialKey::Kana},
            std::pair{0x7D, SpecialKey::Yen},
            std::pair{0x79, SpecialKey::Henkan},
            std::pair{0x7B, SpecialKey::Muhenkan},
            std::pair{0x5C, SpecialKey::PC9800KeypadPeriod},
            std::pair{0xF2, SpecialKey::Hanguel},
            std::pair{0xF1, SpecialKey::Hanja},
            std::pair{0x78, SpecialKey::Katakana},
            std::pair{0x77, SpecialKey::Hiragana},
            std::pair{0x76, SpecialKey::ZenkakuHankaku},
            std::pair{0x1D, SpecialKey::LeftControl},
            std::pair{0x2A, SpecialKey::LeftShift},
            std::pair{0x38, SpecialKey::LeftAlt},
            std::pair{0xE0'5B, SpecialKey::LeftGUI},
            std::pair{0xE0'1D, SpecialKey::RightControl},
            std::pair{0x36, SpecialKey::RightShift},
            std::pair{0xE0'38, SpecialKey::RightAlt},
            std::pair{0xE0'5C, SpecialKey::RightGUI},
            std::pair{0xE0'19, SpecialKey::ScanNextTrack},
            std::pair{0xE0'10, SpecialKey::ScanPreviousTrack},
            std::pair{0xE0'24, SpecialKey::Stop},
            std::pair{0xE0'22, SpecialKey::PlayPause},
            std::pair{0xE0'20, SpecialKey::Mute},
            std::pair{0xE0'30, SpecialKey::VolumeUp},
            std::pair{0xE0'2E, SpecialKey::VolumeDown},
            std::pair{0xE0'6D, SpecialKey::MediaSelect},
            std::pair{0xE0'6C, SpecialKey::Mail},
            std::pair{0xE0'21, SpecialKey::Calculator},
            std::pair{0xE0'6B, SpecialKey::MyComputer},
            std::pair{0xE0'65, SpecialKey::WWWSearch},
            std::pair{0xE0'32, SpecialKey::WWWHome},
            std::pair{0xE0'6A, SpecialKey::WWWBack},
            std::pair{0xE0'69, SpecialKey::WWWForward},
            std::pair{0xE0'68, SpecialKey::WWWStop},
            std::pair{0xE0'67, SpecialKey::WWWRefresh},
            std::pair{0xE0'66, SpecialKey::WWWFavorites},
        };

        constexpr static std::size_t SCANCODES_COUNT =
            256; //approximate count. There is still around ~126 keys on keyboard, so we have a reserve

        constexpr static int VIRTUAL_KEY_PRESSED_BIT = 0b1000'0000;
        constexpr static int VIRTUAL_KEY_TOGGLED_BIT = 0b0000'0001;

        struct LeftRightVirtualKey
        {
            std::uint8_t common;
            std::uint8_t current;
            std::uint8_t contra;
        };

        static std::optional<LeftRightVirtualKey> GetLeftRightVirtualKey(int vk,
                                                                         int scancode) noexcept
        {
            std::optional<LeftRightVirtualKey> out;
            std::uint8_t current = MapVirtualKeyExW(scancode, MAPVK_VSC_TO_VK_EX, nullptr);
            switch(vk)
            {
                case VK_SHIFT:
                {
                    out = LeftRightVirtualKey{.common = VK_SHIFT,
                                              .current = current,
                                              .contra = static_cast<std::uint8_t>(
                                                  (current == VK_LSHIFT ? VK_RSHIFT : VK_LSHIFT))};
                }
                break;
                case VK_MENU:
                    out = LeftRightVirtualKey{.common = VK_MENU,
                                              .current = current,
                                              .contra = static_cast<std::uint8_t>(
                                                  (current == VK_LMENU ? VK_RMENU : VK_LMENU))};
                    break;
                case VK_CONTROL:
                    out = LeftRightVirtualKey{
                        .common = VK_CONTROL,
                        .current = current,
                        .contra = static_cast<std::uint8_t>(
                            (current == VK_LCONTROL ? VK_RCONTROL : VK_LCONTROL))};
                    break;
            }

            return out;
        }

        LRESULT CALLBACK KeyboardState::RawInputWindowProc(HWND handle,
                                                           UINT message,
                                                           WPARAM w_param,
                                                           LPARAM l_param)
        {
            switch(message)
            {
                case WM_CREATE:
                {
                    KeyboardState* state =
                        reinterpret_cast<KeyboardState*>(reinterpret_cast<CREATESTRUCTW*>(l_param));

                    Core::System::SetLastError(ERROR_SUCCESS);
                    auto res =
                        SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
                    if(res == 0) //possible error
                    {
                        if(Core::System::GetLastError() != ERROR_SUCCESS) //error
                            return -1;
                    }
                }
                break;
                case WM_DESTROY:
                {
                    //unregister raw input
                    UnregisterRawKeyboardInput();
                }
                break;
                case WM_INPUT:
                {
                    /*
                    PS/2 Set 1 Make codes:
                    ((), E0'2A, E0'AA, E0'B6, E0'AA E0'B6) + Insert, Delete, LeftArrow, Home, End, UpArrow, DownArrow, PageUp, PageDown, RightArrow
                    ((), E0'AA, E0'B6, E0'AA E0'B6) + Numeric/
                    PrintScreen = E0'2A E0'37, E0'37, 54
                    Pause = E1'1D 45, E0 46

                    scancode stream -> only for Pause/NumLock 
                    */
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
                            RAWKEYBOARD& data = raw_input.data.keyboard;

                            //ignore out of bounds
                            if(data.MakeCode > KEYBOARD_OVERRUN_MAKE_CODE || data.VKey > UCHAR_MAX)
                                return 0;

                            KeyboardState* state = reinterpret_cast<KeyboardState*>(
                                GetWindowLongPtrW(handle, GWLP_USERDATA));

                            //get scancode properties
                            bool is_pressed = !(data.Flags & RI_KEY_BREAK);
                            ScanCode scancode = data.MakeCode & 0b0111'1111;
                            if(data.Flags & RI_KEY_E0)
                                scancode |= 0xE0'00;
                            else if(data.Flags & RI_KEY_E1)
                                scancode |= 0xE1'00;

                            std::uint16_t prev_scancode = state->prev_scancode;
                            state->prev_scancode = scancode;

                            if(scancode == 0xE0'2A || scancode == 0xE0'AA || scancode == 0xE0'B6 ||
                               scancode == 0xE1'1D) //skip prefix scancodes
                                return 0;

                            //Pause = E1'1D 45
                            if(prev_scancode == 0xE1'1D && scancode == 0x45) //check Pause
                                scancode = 0xE1'1D'45;

                            //select right or left version
                            auto lrvk = GetLeftRightVirtualKey(data.VKey, data.MakeCode);
                            if(lrvk)
                            {
                                data.VKey = lrvk->current;
                            }

                            //translate scancode
                            KeyboardKey key = SpecialKey::Unknown;
                            if(auto it = STABLE_SCANCODES.find(scancode);
                               it != STABLE_SCANCODES.end())
                            {
                                key = it->second;
                            }
                            else
                            {
                                key = state->GetKeyByScancode(scancode);
                            }

                            auto [scancode_it, _] = state->scancode_to_key_mapping.insert(
                                std::pair{scancode, SpecialKey::Unknown});

                            //Update keyboard state
                            //Update pressed/released state

                            if(is_pressed)
                            {
                                state->vk_keyboard_state[data.VKey] |= VIRTUAL_KEY_PRESSED_BIT;
                                if(lrvk)
                                {
                                    state->vk_keyboard_state[lrvk->common] |=
                                        VIRTUAL_KEY_PRESSED_BIT;
                                }
                            }
                            else
                            {
                                state->vk_keyboard_state[data.VKey] &= ~VIRTUAL_KEY_PRESSED_BIT;
                                if(lrvk)
                                {
                                    if(!(state->vk_keyboard_state[lrvk->contra] &
                                         VIRTUAL_KEY_PRESSED_BIT))

                                        state->vk_keyboard_state[lrvk->common] &=
                                            ~VIRTUAL_KEY_PRESSED_BIT;
                                }
                            }

                            //Update toggle state
                            constexpr static ScanCode TOGGABLE_SCANCODES[] = {
                                0x45, //Num Lock
                                0x3A, //Caps Lock
                                0x46 //Scroll Lock
                            };

                            if(auto it = std::ranges::find(TOGGABLE_SCANCODES, scancode);
                               it != std::end(TOGGABLE_SCANCODES))
                            {
                                if(is_pressed) //toggled again
                                {
                                    if(!(state->vk_keyboard_state[data.VKey] &
                                         VIRTUAL_KEY_TOGGLED_BIT)) //toggle
                                    {
                                        state->vk_keyboard_state[data.VKey] |=
                                            VIRTUAL_KEY_TOGGLED_BIT;
                                    }
                                    else //untoggle
                                    {
                                        state->vk_keyboard_state[data.VKey] &=
                                            ~VIRTUAL_KEY_TOGGLED_BIT;
                                    }
                                }
                            }

                            HWND current_focus = GetFocus();
                            Window* window = nullptr;
                            if(current_focus)
                                window = reinterpret_cast<Window*>(
                                    GetWindowLongPtrW(current_focus, GWLP_USERDATA));

                            if(window)
                            {
                                if(is_pressed)
                                {
                                    state->parent->PushEvent(Event{
                                        .data = {.keyboard_key_pressed =
                                                     KeyboardKeyPressedEvent{
                                                         .timestamp_ms = GetEventTimestamp(),
                                                         .scancode = scancode,
                                                         .key = key,
                                                         .modifiers = state->GetModifierFlags()}},
                                        .id = ClassID<KeyboardKeyPressedEvent>::ID,
                                        .window = window});
                                }
                                else
                                {
                                    state->parent->PushEvent(Event{
                                        .data = {.keyboard_key_released =
                                                     KeyboardKeyReleasedEvent{
                                                         .timestamp_ms = GetEventTimestamp(),
                                                         .scancode = scancode,
                                                         .key = key,
                                                         .modifiers = state->GetModifierFlags()}},
                                        .id = ClassID<KeyboardKeyReleasedEvent>::ID,
                                        .window = window});
                                }
                            }

                            if(is_pressed)
                            {
                                wchar_t wbuffer[2] = {};

                                int res = ToUnicodeEx(data.VKey,
                                                      scancode,
                                                      state->vk_keyboard_state.data(),
                                                      wbuffer,
                                                      std::size(wbuffer),
                                                      0,
                                                      state->current_layout);

                                if(res > 0) //neither dead key nor invalid combination
                                {
                                    char32_t utf32;
                                    if(res == 2) //surrogate pairs
                                    {
                                        utf32 = ((wbuffer[0] - 0xD8'00) << 10) +
                                                (wbuffer[1] - 0xDC'00) + 0x1'00'00;
                                    }
                                    else
                                    {
                                        utf32 = wbuffer[0];
                                    }

                                    if(window)
                                    {
                                        state->parent->PushEvent(Event{
                                            .data = {.keyboard_character_pressed =
                                                         KeyboardCharacterPressedEvent{
                                                             .timestamp_ms = GetEventTimestamp(),
                                                             .modifiers = state->GetModifierFlags(),
                                                             .utf32_char = utf32}},
                                            .id = ClassID<KeyboardCharacterPressedEvent>::ID,
                                            .window = window});
                                    }
                                }
                            }

                            return 0;
                        }
                    }
                }
                break;
                default:
                    return DefWindowProcW(handle, message, w_param, l_param);
                    break;
            }

            return 0;
        }

        KeyboardState::KeyboardState(WindowSubsystem* _parent)
            : parent(_parent),
              access_state(KeyboardAccessState::Shared),
              current_layout(GetKeyboardLayout(Core::System::GetMainThreadID())),
              prev_scancode(0)
        {
            const wchar_t* window_class = nullptr;
            HINSTANCE instance = parent->GetInstance();
            Core::ScopedCall cleanup(
                [&window_class, instance, this]()
                {
                    if(service_window_handle)
                        DestroyWindow(service_window_handle);

                    if(window_class)
                        UnregisterClassW(window_class, instance);
                });

            //register raw input window class
            WNDCLASSEXW wnd_class = {.cbSize = sizeof(WNDCLASSEXW),
                                     .style = 0,
                                     .lpfnWndProc = KeyboardState::RawInputWindowProc,
                                     .cbClsExtra = 0,
                                     .cbWndExtra = 0,
                                     .hInstance = instance,
                                     .hIcon = nullptr,
                                     .hCursor = nullptr,
                                     .hbrBackground = nullptr,
                                     .lpszMenuName = nullptr,
                                     .lpszClassName = KeyboardState::CLASS_NAME,
                                     .hIconSm = nullptr};

            if(RegisterClassExW(&wnd_class) == 0)
                Core::System::ThrowLastError();

            window_class = KeyboardState::CLASS_NAME;

            //create raw input window
            service_window_handle = CreateWindowExW(0,
                                                    KeyboardState::CLASS_NAME,
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

            RegisterRawKeyboardInput(service_window_handle, KeyboardAccessState::Shared);
            Reset();

            Core::System::SetLastError(ERROR_SUCCESS);
            if(SetWindowLongPtrW(service_window_handle,
                                 GWLP_USERDATA,
                                 reinterpret_cast<LONG_PTR>(this)) == 0)
            {
                if(Core::System::GetLastError() != ERROR_SUCCESS)
                    Core::System::ThrowLastError();
            }

            //update keyboard state
            scancode_to_key_mapping.reserve(SCANCODES_COUNT);
            key_to_scancode_mapping.reserve(SCANCODES_COUNT);
            for(const auto& [sc, key]: STABLE_SCANCODES)
            {
                scancode_to_key_mapping.insert(std::pair{sc, key});
                key_to_scancode_mapping.insert(std::pair{key, sc});
            }

            auto size = GetKeyboardLayoutList(0, nullptr);
            if(size == 0)
                Core::System::ThrowLastError();

            std::vector<HKL> layouts(size);
            GetKeyboardLayoutList(size, layouts.data());

            std::ranges::sort(layouts);

            auto prev_keyboard_layout = GetKeyboardLayout(Core::System::GetMainThreadID());
            for(HKL l: layouts)
            {
                ActivateKeyboardLayout(l, 0);
                for(int vk = 1; vk < 256; vk++)
                {
                    auto scancode = MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC, l);
                    if(scancode != 0)
                    {
                        auto [it, inserted] = scancode_to_key_mapping.insert(
                            std::pair{scancode, SpecialKey::Unknown});
                        if(!inserted)
                        {
                            if(it->second != SpecialKey::Unknown)
                                continue;
                        }

                        char16_t character = MapVirtualKeyExW(vk, MAPVK_VK_TO_CHAR, l);
                        if(character != 0)
                        {
                            char16_t utf16_seq[2] = {character, 0};
                            auto utf32 = Core::System::UTF16ToUTF32(utf16_seq);
                            if(!utf32.has_value())
                                continue;

                            if(*utf32 <= 0xFF)
                            {
                                if(Core::System::IsUnicodeC0ControlCodeOrSpace(*utf32))
                                    continue;
                            }

                            it->second = *utf32;

                            auto [s_it, s_inserted] = key_to_scancode_mapping.insert(std::pair{
                                *utf32,
                                scancode}); //there can be duplication of name(very very rare event but let's handle it)

                            if(!s_inserted)
                                scancode_to_key_mapping.erase(it); //just remove it
                        }
                    }
                }
            }

            ActivateKeyboardLayout(prev_keyboard_layout, 0);

            cleanup.Drop();
        }

        KeyboardState::~KeyboardState()
        {
            if(service_window_handle)
            {
                DestroyWindow(service_window_handle);
                UnregisterClassW(KeyboardState::CLASS_NAME, parent->GetInstance());
            }
        }

        KeyboardState::KeyboardState(KeyboardState&& state) noexcept
            : parent(state.parent),
              service_window_handle(std::exchange(state.service_window_handle, nullptr)),
              vk_keyboard_state(state.vk_keyboard_state),
              scancode_to_key_mapping(std::move(state.scancode_to_key_mapping)),
              key_to_scancode_mapping(std::move(state.key_to_scancode_mapping)),
              current_layout(state.current_layout),
              prev_scancode(state.prev_scancode)
        {}

        KeyboardState& KeyboardState::operator=(KeyboardState&& state) noexcept
        {
            this->~KeyboardState();

            parent = state.parent;
            service_window_handle = std::exchange(state.service_window_handle, nullptr);
            vk_keyboard_state = state.vk_keyboard_state;
            scancode_to_key_mapping = std::move(state.scancode_to_key_mapping);
            key_to_scancode_mapping = std::move(state.key_to_scancode_mapping);
            current_layout = state.current_layout;
            prev_scancode = state.prev_scancode;

            return *this;
        }

        KeyboardKey KeyboardState::GetKeyByScancode(ScanCode scancode)
        {
            auto it = scancode_to_key_mapping.find(scancode);
            if(it != scancode_to_key_mapping.end())
                return it->second;

            return SpecialKey::Unknown;
        }

        std::optional<ScanCode> KeyboardState::GetScanCodeFromKey(KeyboardKey key)
        {
            auto it = key_to_scancode_mapping.find(key);
            if(it != key_to_scancode_mapping.end())
                return it->second;

            return std::nullopt;
        }

        KeyboardAccessState KeyboardState::GetKeyboardAccessState() const noexcept
        {
            return access_state;
        }

        void KeyboardState::SetKeyboardAccessState(KeyboardAccessState state)
        {
            if(access_state == state)
                return;

            RegisterRawKeyboardInput(service_window_handle, state);
            Reset();

            access_state = state;
        }

        void KeyboardState::UpdateCurrentLayout(HKL layout) noexcept
        {
            current_layout = layout;
        }

        void KeyboardState::Reset()
        {
            if(GetKeyboardState(vk_keyboard_state.data()) == 0)
                Core::System::ThrowLastError();
        }

        ModifierKeyFlags KeyboardState::GetModifierFlags() const noexcept
        {
            constexpr static std::pair<int, ModifierKeyFlagBits> VK_MODS[] = {
                {VK_LSHIFT, ModifierKeyFlagBits::LeftShift},
                {VK_RSHIFT, ModifierKeyFlagBits::RightShift},
                {VK_LCONTROL, ModifierKeyFlagBits::LeftControl},
                {VK_RCONTROL, ModifierKeyFlagBits::RightControl},
                {VK_LMENU, ModifierKeyFlagBits::LeftAlt},
                {VK_RMENU, ModifierKeyFlagBits::RightAlt},
                {VK_LWIN, ModifierKeyFlagBits::LeftGUI},
                {VK_RWIN, ModifierKeyFlagBits::RightGUI}};

            ModifierKeyFlags flags = 0;
            for(const auto& [vk, bit]: VK_MODS)
            {
                if(vk_keyboard_state[vk] & VIRTUAL_KEY_PRESSED_BIT)
                    flags |= bit;
            }

            return flags;
        }
    };
};