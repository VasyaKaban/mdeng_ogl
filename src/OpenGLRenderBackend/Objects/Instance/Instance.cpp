#include "Instance.h"
#include "Core/Render/Objects/PhysicalDevice.h"
#include "../Surface/Surface.h"
#include "../Device/Device.h"
#include <format>
#include <iostream>

namespace OpenGL
{
    static void default_debug_messenger(Render::DebugMessengerSeverityFlagBits severity,
                                        Render::DebugMessengerTypeFlags types,
                                        std::int64_t id,
                                        std::string_view message)
    {
        std::string_view severity_string = "Unknown";
        switch(severity)
        {
            case Render::DebugMessengerSeverityFlagBits::Error:
                severity_string = "Error";
                break;
            case Render::DebugMessengerSeverityFlagBits::Warning:
                severity_string = "Warning";
                break;
            case Render::DebugMessengerSeverityFlagBits::Info:
                severity_string = "Info";
                break;
            case Render::DebugMessengerSeverityFlagBits::Verbose:
                severity_string = "Verbose";
                break;
        }

        static std::string types_string;
        constexpr std::pair<Render::DebugMessengerTypeFlagBits, std::string_view>
            TYPE_NAMES_MAPPING[] = {
                {Render::DebugMessengerTypeFlagBits::General, "General"},
                {Render::DebugMessengerTypeFlagBits::Validation, "Validation"},
                {Render::DebugMessengerTypeFlagBits::Performance, "Performance"}};

        for(const auto& [type, name]: TYPE_NAMES_MAPPING)
        {
            if(types_string.empty())
                types_string += name;
            else
                types_string += std::format(" | {}", name);
        }

        std::format_to(std::ostream_iterator<char>(std::cout, "\n"),
                       "[Severity: {}][Types: {}] ID: {}; {}",
                       severity_string,
                       types_string.empty() ? "Unknown" : types_string,
                       id,
                       message);

        types_string.clear();
    }

    Instance::Instance(const Render::InstanceInfo& info)
        : enabled_features(info.enabled_features)
    {
        if(enabled_features.debug_messenger)
        {
            debug_messenger_info = info.debug_messenger_info;
            if(!debug_messenger_info.callback)
                debug_messenger_info.callback = default_debug_messenger;
        }
        else if(enabled_features.validation_layer)
        {
            debug_messenger_info = Render::DebugMessengerInfo{
                .severities = Render::DebugMessengerSeverityFlagBits::Info |
                              Render::DebugMessengerSeverityFlagBits::Error |
                              Render::DebugMessengerSeverityFlagBits::Verbose |
                              Render::DebugMessengerSeverityFlagBits::Warning,
                .types = Render::DebugMessengerTypeFlagBits::General |
                         Render::DebugMessengerTypeFlagBits::Performance |
                         Render::DebugMessengerTypeFlagBits::Validation,
                .callback = default_debug_messenger};
        }
    }

    Instance::~Instance()
    {}

    Render::ContextMode Instance::GetContextMode() const noexcept
    {
        return Render::ContextMode::Legacy;
    }

    Render::Device* Instance::CreateLegacyDevice(const Render::LegacyDeviceInfo& info)
    {
        Surface* impl_surface = static_cast<Surface*>(info.surface);
        impl_surface->Connect(info);

        return new Device(info);
    }

    std::vector<Render::PhysicalDevice*> Instance::GetPhysicalDevices() const
    {
        return {};
    }

#ifdef _WIN32
    Render::Surface* Instance::CreateSurface(const Render::Win32SurfaceInfo& info)
    {
        return new Surface(this, info);
    }
#elif defined(linux)
    virtual Surface* Instance::CreateSurface(const XCBSurfaceInfo& info)
    {
#    error TODO!
    }
#endif

    const Render::InstanceFeatures& Instance::GetEnabledFeatures() const noexcept
    {
        return enabled_features;
    }

    const Render::DebugMessengerInfo& Instance::GetDebugMessengerInfo() const noexcept
    {
        return debug_messenger_info;
    }
};