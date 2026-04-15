#pragma once

#include <string>
#include <vector>
#include "Core/API.h"
#include "View.h"

namespace Core
{
    class Window;

    class CORE_API Display
    {
    protected:
        virtual ~Display() = 0;
    public:
        virtual std::string GetName() const = 0;
        virtual std::vector<VideoMode> GetVideoModes() const = 0;
        virtual VideoMode GetCurrentVideoMode() const = 0;

        virtual float GetScaleFactor() const = 0; // return dpi / default_dpi;
        virtual float GetDisplayScaleFactor() const = 0; // return scaled_resolution / resolution;

        virtual void SetVideoMode(std::uint32_t index) = 0;

        virtual WindowPosition GetPosition() const = 0;

        virtual Window* GetParent() const noexcept = 0;
    };
};