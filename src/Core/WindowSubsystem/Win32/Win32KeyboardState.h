#pragma once

#include <array>
#include <unordered_map>
#include "Core/Utils/NonCreatable.hpp"
#include "Core/Utils/System.h"
#include "Win32WindowSubsystem.h"

namespace Core
{
    class CORE_API Win32KeyboardState final
    {
    private:
        constexpr static wchar_t WIN32_KEYBOARD_STATE_CLASS_NAME[] = L"WIN32_KEYBOARD_STATE_CLASS";

        static LRESULT CALLBACK RawInputWindowProc(HWND handle,
                                                   UINT message,
                                                   WPARAM w_param,
                                                   LPARAM l_param);
    public:
        CORE_NON_COPYABLE(Win32KeyboardState)
        CORE_NON_MOVABLE(Win32KeyboardState)

        Win32KeyboardState(Win32WindowSubsystemConnection* parent,
                           KeyboardAccessState access_state);
        ~Win32KeyboardState();

        KeyboardKey GetKeyByScancode(ScanCode scancode);
        std::optional<ScanCode> GetScanCodeFromKey(KeyboardKey key);

        KeyboardAccessState GetKeyboardAccessState() const noexcept;
        void SetKeyboardAccessState(KeyboardAccessState state);

        void UpdateCurrentLayout(HKL layout) noexcept;
        void Reset(); //resets due to the focus gain/loose/init
    private:
        LRESULT HandleWMCreate(HWND handle);
        LRESULT HandleWMInput(HRAWINPUT raw_input_ptr);
    private:
        Win32WindowSubsystemConnection* parent;
        KeyboardAccessState access_state;
        ATOM service_window_class_atom;
        HWND service_window_handle;
        std::array<BYTE, 256> vk_keyboard_state;
        std::unordered_map<ScanCode, KeyboardKey> scancode_to_key_mapping;
        std::unordered_map<KeyboardKey, ScanCode> key_to_scancode_mapping;
        HKL current_layout;

        std::uint16_t prev_scancode;
    };
};