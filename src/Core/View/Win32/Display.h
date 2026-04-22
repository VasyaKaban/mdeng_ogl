#pragma once

#include <string>
#include <vector>
#include <array>
#include "Core/Utils/System.h"
#include "Core/Utils/NonCreatable.hpp"
#include "../Display.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        using DisplayChangesFlags = std::uint32_t;
        namespace DisplayChangesFlagBits
        {
            enum : DisplayChangesFlags
            {
                ScaleFactor = 1 << 0,
                VideoMode = 1 << 1,
                Position = 1 << 2
            };
        };

        class CORE_API Display final : public Core::Display, Core::NonMovable
        {
        public:
            using EventEmitter::Emit;

            Display(WindowSubsystem* _parent, HMONITOR _handle);

            virtual ~Display() override;

            virtual std::string GetName() const override;
            virtual std::vector<VideoMode> GetVideoModes() const override;
            virtual VideoMode GetCurrentVideoMode() const override;

            virtual float GetScaleFactor() const override; // return dpi / default_dpi;

            virtual void SetVideoMode(std::uint32_t index) override;

            virtual WindowPosition GetPosition() const override;

            virtual Core::WindowSubsystem* GetParent() const noexcept override;

            DisplayChangesFlags Update();
            HMONITOR GetHandle() const noexcept;
            const wchar_t* GetDeviceName() const noexcept;
        private:
            WindowSubsystem* parent;
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
};