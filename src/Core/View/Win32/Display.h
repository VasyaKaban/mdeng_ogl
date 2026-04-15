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
        class Window;

        class CORE_API Display final : public Core::Display, Core::NonCopyable, Core::NonMovable
        {
        public:
            Display(Window* _parent, HMONITOR _handle);

            virtual ~Display() override;

            virtual std::string GetName() const override;
            virtual std::vector<VideoMode> GetVideoModes() const override;
            virtual VideoMode GetCurrentVideoMode() const override;

            virtual float GetScaleFactor() const override; // return dpi / default_dpi;
            virtual float GetDisplayScaleFactor() const override; //always 1.0f

            virtual void SetVideoMode(std::uint32_t index) override;

            virtual WindowPosition GetPosition() const override;

            virtual Core::Window* GetParent() const noexcept override;
        private:
            Window* parent;
            HMONITOR handle;

            std::vector<DEVMODEW> dev_modes;
            std::array<wchar_t, CCHDEVICENAME> device_name;
            std::string device_description;
        };
    };
};