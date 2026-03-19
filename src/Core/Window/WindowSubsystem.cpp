#include "WindowSubsystem.h"
#ifdef _WIN32
#    include "Win32/WindowSubsystem.h"
#elif defined(linux)
#    error TODO!
#endif

namespace Core
{
    WindowSubsystem::~WindowSubsystem()
    {}

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
            throw std::runtime_error("Bad window subsystem type. Only Win32 is supported");

#    error TODO!
#endif
    }
};