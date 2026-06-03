#pragma once

#include <string>
#include <vector>
#include "Core/Utils/RC.hpp"
#include "Core/API.h"
#include "WindowEvents.h"
#include "WindowSubsystem.h"
#include "Core/Events/Events.h"

namespace Core
{
    class Window;

    class CORE_API Display : public RC,
                             protected ReservedEventEmitter<DisplayRemovedEvent,
                                                            DisplayMovedEvent,
                                                            DisplayVideoModeChangedEvent,
                                                            DisplayScaleChangedEvent>
    {
    public:
        using EventEmitter::Connect;

        virtual ~Display() = 0;

        virtual std::string GetName() const = 0;
        virtual VideoMode GetCurrentVideoMode() const = 0;
        virtual WindowPosition GetPosition() const = 0;
        virtual std::vector<VideoMode> GetVideoModes() const = 0;

        virtual float GetScaleFactor() const = 0; // return dpi / default_dpi;
        /*
        HIDPI handling:
        100 -> 800x600, 1
        200 -> 1600x1200, 2
        150 -> (1600x600 / 2 * 1.5) 1200x900, 1.5

        (window resolution / current window scale factor(inner user)) * scale factor
        current window scale factor(inner user) = scale factor
        
        */

        virtual void SetVideoMode(std::uint32_t index) = 0;

        virtual WindowSubsystemConnection* GetParent() const noexcept = 0;
    };
};