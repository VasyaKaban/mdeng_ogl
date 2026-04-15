#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "Core/Utils/System.h"
#include "../View.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        struct KeyState
        {
            std::uint32_t repeat_count;
            KeyboardKey key;
        };

        class KeyboardState : Core::NonCopyable
        {
        private:
            constexpr static wchar_t CLASS_NAME[] = L"WIN32_KEYBOARD_STATE_CLASS";

            static LRESULT CALLBACK RawInputWindowProc(HWND handle,
                                                       UINT message,
                                                       WPARAM w_param,
                                                       LPARAM l_param);
        public:
            KeyboardState(WindowSubsystem* _parent);
            ~KeyboardState();
            KeyboardState(KeyboardState&& state) noexcept;
            KeyboardState& operator=(KeyboardState&& state) noexcept;

            KeyboardKey GetKeyByScancode(ScanCode scancode);
            std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key);
        private:
            WindowSubsystem* parent;
            HWND service_window_handle;
            std::array<BYTE, 256> vk_keyboard_state;
            std::unordered_map<ScanCode, KeyState> scancode_to_key_state_mapping;
            std::unordered_map<KeyboardKey, ScanCode> key_to_scancode_mapping;

            std::uint16_t prev_scancode;
        };
    };
};