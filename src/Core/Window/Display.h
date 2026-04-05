#pragma once

#include <string>
#include <vector>
#include "Core/API.h"

namespace Core
{
    class Window;

    struct VideoMode
    {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t refresh_rate;
        std::uint32_t bits_per_pixel;
    };

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

        virtual Window* GetParent() const noexcept = 0;
    };
};