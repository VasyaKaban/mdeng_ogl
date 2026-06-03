#include "WindowSubsystem.h"
#ifdef _WIN32
#    include "Win32/WindowSubsystem.h"
#elif defined(linux)
#    error TODO!
#endif

namespace Core
{
    std::vector<WindowSubsystemType> GetAvailableWindowSubsystemTypes()
    {
#ifdef _WIN32
        return {WindowSubsystemType::Win32};
#elif defined(linux)
        return {WindowSubsystemType::XCB};
#endif
    }

    WindowSubsystem* CreateWindowSubsystem(const WindowSubsystemInfo& info)
    {
#ifdef _WIN32
        if(info.type != WindowSubsystemType::Win32)
            throw std::runtime_error("Bad window subsystem type. Only Win32 is supported");

        return new Win32::WindowSubsystem();

#elif defined(linux)
        if(info.type != WindowSubsystemType::XCB)
            throw std::runtime_error("Bad window subsystem type. Only XCB is supported");

#    error TODO!
#endif
    }

    std::string KeyboardKeyToString(KeyboardKey key)
    {
        std::string out;
        switch(key)
        {
            case SpecialKey::Unknown:
                out = "Unknown";
                break;
            case SpecialKey::Enter:
                out = "Enter";
                break;
            case SpecialKey::Escape:
                out = "Escape";
                break;
            case SpecialKey::BackSpace:
                out = "BackSpace";
                break;
            case SpecialKey::Tab:
                out = "Tab";
                break;
            case SpecialKey::Space:
                out = "Space";
                break;
            case SpecialKey::CapsLock:
                out = "CapsLock";
                break;
            case SpecialKey::F1:
                out = "F1";
                break;
            case SpecialKey::F2:
                out = "F2";
                break;
            case SpecialKey::F3:
                out = "F3";
                break;
            case SpecialKey::F4:
                out = "F4";
                break;
            case SpecialKey::F5:
                out = "F5";
                break;
            case SpecialKey::F6:
                out = "F6";
                break;
            case SpecialKey::F7:
                out = "F7";
                break;
            case SpecialKey::F8:
                out = "F8";
                break;
            case SpecialKey::F9:
                out = "F9";
                break;
            case SpecialKey::F10:
                out = "F10";
                break;
            case SpecialKey::F11:
                out = "F11";
                break;
            case SpecialKey::F12:
                out = "F12";
                break;
            case SpecialKey::PrintScreen:
                out = "PrintScreen";
                break;
            case SpecialKey::SysRq:
                out = "SysRq";
                break;
            case SpecialKey::ScrollLock:
                out = "ScrollLock";
                break;
            case SpecialKey::Break:
                out = "Break";
                break;
            case SpecialKey::Pause:
                out = "Pause";
                break;
            case SpecialKey::Insert:
                out = "Insert";
                break;
            case SpecialKey::Home:
                out = "Home";
                break;
            case SpecialKey::PageUp:
                out = "PageUp";
                break;
            case SpecialKey::Delete:
                out = "Delete";
                break;
            case SpecialKey::End:
                out = "End";
                break;
            case SpecialKey::PageDown:
                out = "PageDown";
                break;
            case SpecialKey::RightArrow:
                out = "RightArrow";
                break;
            case SpecialKey::LeftArrow:
                out = "LeftArrow";
                break;
            case SpecialKey::DownArrow:
                out = "DownArrow";
                break;
            case SpecialKey::UpArrow:
                out = "UpArrow";
                break;
            case SpecialKey::NumLock:
                out = "NumLock";
                break;
            case SpecialKey::KeypadDivide:
                out = "KeypadDivide";
                break;
            case SpecialKey::KeypadMultiply:
                out = "KeypadMultiply";
                break;
            case SpecialKey::KeypadSubtract:
                out = "KeypadSubtract";
                break;
            case SpecialKey::KeypadAdd:
                out = "KeypadAdd";
                break;
            case SpecialKey::KeypadEnter:
                out = "KeypadEnter";
                break;
            case SpecialKey::Keypad1:
                out = "Keypad1";
                break;
            case SpecialKey::Keypad2:
                out = "Keypad2";
                break;
            case SpecialKey::Keypad3:
                out = "Keypad3";
                break;
            case SpecialKey::Keypad4:
                out = "Keypad4";
                break;
            case SpecialKey::Keypad5:
                out = "Keypad5";
                break;
            case SpecialKey::Keypad6:
                out = "Keypad6";
                break;
            case SpecialKey::Keypad7:
                out = "Keypad7";
                break;
            case SpecialKey::Keypad8:
                out = "Keypad8";
                break;
            case SpecialKey::Keypad9:
                out = "Keypad9";
                break;
            case SpecialKey::Keypad0:
                out = "Keypad0";
                break;
            case SpecialKey::KeypadPeriod:
                out = "KeypadPeriod";
                break;
            case SpecialKey::Menu:
                out = "Menu";
                break;
            case SpecialKey::KeyboardPower:
                out = "KeyboardPower";
                break;
            case SpecialKey::KeypadEqual:
                out = "KeypadEqual";
                break;
            case SpecialKey::F13:
                out = "F13";
                break;
            case SpecialKey::F14:
                out = "F14";
                break;
            case SpecialKey::F15:
                out = "F15";
                break;
            case SpecialKey::F16:
                out = "F16";
                break;
            case SpecialKey::F17:
                out = "F17";
                break;
            case SpecialKey::F18:
                out = "F18";
                break;
            case SpecialKey::F19:
                out = "F19";
                break;
            case SpecialKey::F20:
                out = "F20";
                break;
            case SpecialKey::F21:
                out = "F21";
                break;
            case SpecialKey::F22:
                out = "F22";
                break;
            case SpecialKey::F23:
                out = "F23";
                break;
            case SpecialKey::F24:
                out = "F24";
                break;
            case SpecialKey::BrazilKeypad:
                out = "BrazilKeypad";
                break;
            case SpecialKey::Ro:
                out = "Ro";
                break;
            case SpecialKey::Kana:
                out = "Kana";
                break;
            case SpecialKey::Yen:
                out = "Yen";
                break;
            case SpecialKey::Henkan:
                out = "Henkan";
                break;
            case SpecialKey::Muhenkan:
                out = "Muhenkan";
                break;
            case SpecialKey::PC9800KeypadPeriod:
                out = "PC9800KeypadPeriod";
                break;
            case SpecialKey::Hanguel:
                out = "Hanguel";
                break;
            case SpecialKey::Hanja:
                out = "Hanja";
                break;
            case SpecialKey::Katakana:
                out = "Katakana";
                break;
            case SpecialKey::Hiragana:
                out = "Hiragana";
                break;
            case SpecialKey::ZenkakuHankaku:
                out = "ZenkakuHankaku";
                break;
            case SpecialKey::LeftControl:
                out = "LeftControl";
                break;
            case SpecialKey::LeftShift:
                out = "LeftShift";
                break;
            case SpecialKey::LeftAlt:
                out = "LeftAlt";
                break;
            case SpecialKey::LeftGUI:
                out = "LeftGUI";
                break;
            case SpecialKey::RightControl:
                out = "RightControl";
                break;
            case SpecialKey::RightShift:
                out = "RightShift";
                break;
            case SpecialKey::RightAlt:
                out = "RightAlt";
                break;
            case SpecialKey::RightGUI:
                out = "RightGUI";
                break;
            case SpecialKey::ScanNextTrack:
                out = "ScanNextTrack";
                break;
            case SpecialKey::ScanPreviousTrack:
                out = "ScanPreviousTrack";
                break;
            case SpecialKey::Stop:
                out = "Stop";
                break;
            case SpecialKey::PlayPause:
                out = "PlayPause";
                break;
            case SpecialKey::Mute:
                out = "Mute";
                break;
            case SpecialKey::VolumeUp:
                out = "VolumeUp";
                break;
            case SpecialKey::VolumeDown:
                out = "VolumeDown";
                break;
            case SpecialKey::MediaSelect:
                out = "MediaSelect";
                break;
            case SpecialKey::Mail:
                out = "Mail";
                break;
            case SpecialKey::Calculator:
                out = "Calculator";
                break;
            case SpecialKey::MyComputer:
                out = "MyComputer";
                break;
            case SpecialKey::WWWSearch:
                out = "WWWSearch";
                break;
            case SpecialKey::WWWHome:
                out = "WWWHome";
                break;
            case SpecialKey::WWWBack:
                out = "WWWBack";
                break;
            case SpecialKey::WWWForward:
                out = "WWWForward";
                break;
            case SpecialKey::WWWStop:
                out = "WWWStop";
                break;
            case SpecialKey::WWWRefresh:
                out = "WWWRefresh";
                break;
            case SpecialKey::WWWFavorites:
                out = "WWWFavorites";
                break;
            default:
            {
                UTF8Result res = Core::System::UTF32ToUTF8(key);
                out.assign(res.data, res.length);
            }
            break;
        }

        return out;
    }
};