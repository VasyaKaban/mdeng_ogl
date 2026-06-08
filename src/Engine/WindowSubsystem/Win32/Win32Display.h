#pragma once

#include <string>
#include <vector>
#include <array>
#include "Core/Utils/System.h"
#include "Core/Utils/NonCreatable.hpp"
#include "../Display.h"
#include "Win32WindowSubsystem.h"

namespace Core
{
    using DisplayChangesFlags = std::uint32_t;
    namespace DisplayChangesFlagBits
    {

        constexpr inline DisplayChangesFlags ScaleFactor = 1 << 0;
        constexpr inline DisplayChangesFlags VideoMode = 1 << 1;
        constexpr inline DisplayChangesFlags Position = 1 << 2;

    };

    class CORE_API Win32Display final : public Core::Display
    {
    public:
        using EventEmitter::Emit;

        CORE_NON_COPYABLE(Win32Display)
        CORE_NON_MOVABLE(Win32Display)

        Win32Display(Win32WindowSubsystemConnection* parent, HMONITOR handle);

        virtual ~Win32Display() override;

        virtual std::string GetName() const override;
        virtual std::vector<VideoMode> GetVideoModes() const override;
        virtual VideoMode GetCurrentVideoMode() const override;
        virtual WindowPosition GetPosition() const override;
        virtual float GetScaleFactor() const override;

        virtual void SetVideoMode(std::uint32_t index) override;

        virtual WindowSubsystemConnection* GetParent() const noexcept override;

        DisplayChangesFlags Update();
        HMONITOR GetHandle() const noexcept;
        const wchar_t* GetDeviceName() const noexcept;
    private:
        Win32WindowSubsystemConnection* parent;
        HMONITOR handle;

        std::uint32_t dpi;
        VideoMode video_mode;
        WindowPosition position;

        std::vector<DEVMODEW> dev_modes;
        std::vector<VideoMode> video_modes;
        std::array<wchar_t, CCHDEVICENAME> device_name;
        std::string device_description;
    };
};