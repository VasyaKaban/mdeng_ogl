#pragma once

#include <functional>
#include "hrs/non_creatable.hpp"

struct WindowResolution
{
    int width;
    int height;
};

struct WindowPosition
{
    int x;
    int y;
};

class EventHandlers : hrs::non_copyable, hrs::non_movable
{
public:
    struct WindowCloseEvent
    {};

    struct WindowResizedEvent
    {
        WindowResolution resolution;
        WindowResolution drawable_resolution;
    };

    struct WindowExposedEvent
    {};

    struct WindowMovedEvent
    {
        WindowPosition position;
    };

    using WindowCloseHandler = void(const WindowCloseEvent&);
    using WindowResizedHandler = void(const WindowResizedEvent&);
    using WindowExposedHandler = void(const WindowExposedEvent&);
    using WindowMovedHandler = void(const WindowMovedEvent&);

    template<typename T>
    class EventHandlerHandle
    {
        friend class EventHandlers;

        EventHandlerHandle(std::list<std::function<T>>::const_iterator _it) noexcept;
    public:
        EventHandlerHandle() = default;
        ~EventHandlerHandle() = default;
        EventHandlerHandle(const EventHandlerHandle&) = default;
        EventHandlerHandle(EventHandlerHandle&&) = default;
        EventHandlerHandle& operator=(const EventHandlerHandle&) = default;
        EventHandlerHandle& operator=(EventHandlerHandle&&) = default;
    private:
        std::list<std::function<T>>::const_iterator it;
    };

    EventHandlers() = default;
    ~EventHandlers() = default;

    EventHandlerHandle<WindowCloseHandler>
    AddHandler(const std::function<WindowCloseHandler>& handler);
    EventHandlerHandle<WindowCloseHandler> AddHandler(std::function<WindowCloseHandler>&& handler);
    void EraseHandler(EventHandlerHandle<WindowCloseHandler> handle) noexcept;
    void Apply(const WindowCloseEvent& event);

    EventHandlerHandle<WindowResizedHandler>
    AddHandler(const std::function<WindowResizedHandler>& handler);
    EventHandlerHandle<WindowResizedHandler>
    AddHandler(std::function<WindowResizedHandler>&& handler);
    void EraseHandler(EventHandlerHandle<WindowResizedHandler> handle) noexcept;
    void Apply(const WindowResizedEvent& event);

    EventHandlerHandle<WindowExposedHandler>
    AddHandler(const std::function<WindowExposedHandler>& handler);
    EventHandlerHandle<WindowExposedHandler>
    AddHandler(std::function<WindowExposedHandler>&& handler);
    void EraseHandler(EventHandlerHandle<WindowExposedHandler> handle) noexcept;
    void Apply(const WindowExposedEvent& event);

    EventHandlerHandle<WindowMovedHandler>
    AddHandler(const std::function<WindowMovedHandler>& handler);
    EventHandlerHandle<WindowMovedHandler> AddHandler(std::function<WindowMovedHandler>&& handler);
    void EraseHandler(EventHandlerHandle<WindowMovedHandler> handle) noexcept;
    void Apply(const WindowMovedEvent& event);
private:
    std::list<std::function<WindowCloseHandler>> window_close_handlers;
    std::list<std::function<WindowResizedHandler>> window_resized_handlers;
    std::list<std::function<WindowExposedHandler>> window_exposed_handlers;
    std::list<std::function<WindowMovedHandler>> window_moved_handlers;
};

//SDL_WINDOWEVENT_SHOWN,          /**< Window has been shown */
//SDL_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */

//SDL_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
//                                     a result of an API call or through the
//                                     system or user changing the window size. */
//SDL_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
//SDL_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
//SDL_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
//                                     and position */
//SDL_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
//SDL_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
//SDL_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
//SDL_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
//SDL_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
//SDL_WINDOWEVENT_HIT_TEST,       /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */
//SDL_WINDOWEVENT_ICCPROF_CHANGED,/**< The ICC profile of the window's display has changed. */
//SDL_WINDOWEVENT_DISPLAY_CHANGED /**< Window has been moved to display data1. */