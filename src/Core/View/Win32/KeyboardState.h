#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "Core/Utils/System.h"
#include "../View.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        class CORE_API KeyboardState : Core::NonCopyable
        {
        private:
            constexpr static wchar_t WIN32_KEYBOARD_STATE_CLASS_NAME[] =
                L"WIN32_KEYBOARD_STATE_CLASS";

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

            KeyboardAccessState GetKeyboardAccessState() const noexcept;
            void SetKeyboardAccessState(KeyboardAccessState state);

            void UpdateCurrentLayout(HKL layout) noexcept;
            void Reset(); //resets due to the focus gain/loose/init
        private:
            ModifierKeyFlags GetModifierFlags() const noexcept;
        private:
            WindowSubsystem* parent;
            KeyboardAccessState access_state;
            HWND service_window_handle;
            std::array<BYTE, 256> vk_keyboard_state;
            std::unordered_map<ScanCode, KeyboardKey> scancode_to_key_mapping;
            std::unordered_map<KeyboardKey, ScanCode> key_to_scancode_mapping;
            HKL current_layout;

            std::uint16_t prev_scancode;
        };
    };
};