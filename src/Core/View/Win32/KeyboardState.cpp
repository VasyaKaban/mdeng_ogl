#include "KeyboardState.h"
#include "Core/Utils/ScopedCall.hpp"
#include "WindowSubsystem.h"
#include "hidusage.h"

namespace Core
{
    namespace Win32
    {
        constexpr static std::pair<ScanCode, KeyboardKey> STABLE_SCANCODES[] = {
#error TODO !
        };

        enum class StableScanCode : ScanCode
        {
            Enter = 0x1C,
            Escape = 0x01,
            BackSpace = 0x0E,
            Tab = 0x0F,
            Space = 0x39,
            CapsLock = 0x3A,
            F1 = 0x3B,
            F2 = 0x3C,
            F3 = 0x3D,
            F4 = 0x3E,
            F5 = 0x3F,
            F6 = 0x40,
            F7 = 0x41,
            F8 = 0x42,
            F9 = 0x43,
            F10 = 0x44,
            F11 = 0x57,
            F12 = 0x58,
            PrintScreen = 0xE0'37,
            ScrollLock = 0x46,
            Break = 0xE0'46, //also E0 C6
            Pause = 0xE1'1D'45, //also E1 9D C5
            Insert = 0xE0'52,
            Home = 0xE0'47,
            PageUp = 0xE0'49,
            Delete = 0xE0'53,
            End = 0xE0'4F,
            PageDown = 0xE0'51,
            RightArrow = 0xE0'4D,
            LeftArrow = 0xE0'4B,
            DownArrow = 0xE0'50,
            UpArrow = 0xE0'48,
            NumLock = 0x45,
            KeypadDivide = 0xE0'35,
            KeypadMultiply = 0x37,
            KeypadSubtract = 0x4A,
            KeypadAdd = 0x4E,
            KeypadEnter = 0xE0'1C,
            Keypad1 = 0x4F,
            Keypad2 = 0x50,
            Keypad3 = 0x51,
            Keypad4 = 0x4B,
            Keypad5 = 0x4C,
            Keypad6 = 0x4D,
            Keypad7 = 0x47,
            Keypad8 = 0x48,
            Keypad9 = 0x49,
            Keypad0 = 0x52,
            KeypadPeriod = 0x53,
            Menu = 0xE0'5D,
            KeyboardPower = 0xE0'5E,
            KeypadEqual = 0x59,
            F13 = 0x64,
            F14 = 0x65,
            F15 = 0x66,
            F16 = 0x67,
            F17 = 0x68,
            F18 = 0x69,
            F19 = 0x6A,
            F20 = 0x6B,
            F21 = 0x6C,
            F22 = 0x6D,
            F23 = 0x6E,
            F24 = 0x76,
            BrazilKeypad = 0x7E,
            Ro = 0x73,
            Kana = 0x70,
            Yen = 0x7D,
            Henkan = 0x79,
            Muhenkan = 0x7B,
            PC9800KeypadPeriod = 0x5C,
            Hanguel = 0xF2,
            Hanja = 0xF1,
            Katakana = 0x78,
            Hiragana = 0x77,
            ZenkakuHankaku = 0x76,
            LeftControl = 0x1D,
            LeftShift = 0x2A,
            LeftAlt = 0x38,
            LeftGUI = 0xE0'5B,
            RightControl = 0xE0'1D,
            RightShift = 0x36,
            RightAlt = 0xE0'38,
            RightGUI = 0xE0'5C,
            ScanNextTrack = 0xE0'19,
            ScanPreviousTrack = 0xE0'10,
            Stop = 0xE0'24,
            PlayPause = 0xE0'22,
            Mute = 0xE0'20,
            VolumeUp = 0xE0'30,
            VolumeDown = 0xE0'2E,
            MediaSelect = 0xE0'6D,
            Mail = 0xE0'6C,
            Calculator = 0xE0'21,
            MyComputer = 0xE0'6B,
            WWWSearch = 0xE0'65,
            WWWHome = 0xE0'32,
            WWWBack = 0xE0'6A,
            WWWForward = 0xE0'69,
            WWWStop = 0xE0'68,
            WWWRefresh = 0xE0'67,
            WWWFavorites = 0xE0'66
        };

        constexpr static std::size_t SCANCODES_COUNT =
            256; //approximate count. There is still around ~126 keys on keyboard, so we have a reserve

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

        LRESULT CALLBACK KeyboardState::RawInputWindowProc(HWND handle,
                                                           UINT message,
                                                           WPARAM w_param,
                                                           LPARAM l_param)
        {
#error TODO!!!
        }

        KeyboardState::KeyboardState(WindowSubsystem* _parent)
            : parent(_parent),
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

            Core::System::SetLastError(ERROR_SUCCESS);
            if(SetWindowLongPtrW(service_window_handle,
                                 GWLP_USERDATA,
                                 reinterpret_cast<LONG_PTR>(this)) == 0)
            {
                if(Core::System::GetLastError() != ERROR_SUCCESS)
                    Core::System::ThrowLastError();
            }

            //update keyboard state
            scancode_to_key_state_mapping.reserve(SCANCODES_COUNT);
            key_to_scancode_mapping.reserve(SCANCODES_COUNT);
            for(const auto& [sc, key]: STABLE_SCANCODES)
            {
                scancode_to_key_state_mapping.insert(
                    std::pair{sc, KeyState{.repeat_count = 0, .key = key}});
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
                        auto [it, inserted] = scancode_to_key_state_mapping.insert(
                            std::pair{scancode,
                                      KeyState{.repeat_count = 0, .key = SpecialKey::KeyUnknown}});
                        if(!inserted)
                        {
                            if(it->second.key != SpecialKey::KeyUnknown)
                                continue;
                        }

                        char16_t character = MapVirtualKeyExW(vk, MAPVK_VK_TO_CHAR, l);
                        if(character != 0)
                        {
                            char16_t utf16_seq[2] = {character, 0};
                            auto utf32 = Core::System::UTF16ToUTF32(utf16_seq);
                            if(!utf32.has_value())
                                continue;

                            if(utf32 <= 0xFF)
                            {
                                if(Core::System::IsUnicodeC0ControlCodeOrSpace(*utf32))
                                    continue;
                            }

                            it->second.key = *utf32;

                            auto [s_it, s_inserted] = key_to_scancode_mapping.insert(std::pair{
                                *utf32,
                                scancode}); //there can be duplication of name(very very rare event but let's handle it)

                            if(!s_inserted)
                                scancode_to_key_state_mapping.erase(it); //just remove it
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

        std::unordered_map<ScanCode, KeyState> key_states;

        KeyboardState::KeyboardState(KeyboardState&& state) noexcept
            : parent(state.parent),
              service_window_handle(std::exchange(state.service_window_handle, nullptr)),
              vk_keyboard_state(state.vk_keyboard_state),
              scancode_to_key_state_mapping(std::move(state.scancode_to_key_state_mapping)),
              key_to_scancode_mapping(std::move(state.key_to_scancode_mapping)),
              prev_scancode(state.prev_scancode)
        {}

        KeyboardState& KeyboardState::operator=(KeyboardState&& state) noexcept
        {
            this->~KeyboardState();

            parent = state.parent;
            service_window_handle = std::exchange(state.service_window_handle, nullptr);
            vk_keyboard_state = state.vk_keyboard_state;
            scancode_to_key_state_mapping = std::move(state.scancode_to_key_state_mapping);
            key_to_scancode_mapping = std::move(state.key_to_scancode_mapping);
            prev_scancode = state.prev_scancode;

            return *this;
        }

        KeyboardKey KeyboardState::GetKeyByScancode(ScanCode scancode)
        {
            auto it = scancode_to_key_state_mapping.find(scancode);
            if(it != scancode_to_key_state_mapping.end())
                return it->second.key;

            return SpecialKey::KeyUnknown;
        }

        std::optional<ScanCode> KeyboardState::GetScanCodeFromKey(KeyboardKey key)
        {
            auto it = key_to_scancode_mapping.find(key);
            if(it != key_to_scancode_mapping.end())
                return it->second;

            return std::nullopt;
        }

    };
};