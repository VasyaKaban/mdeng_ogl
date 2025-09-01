#include "EventHandlers.h"

template<typename T>
EventHandlers::EventHandlerHandle<T>::EventHandlerHandle(
    std::list<std::function<T>>::const_iterator _it) noexcept
    : it(_it)
{}

EventHandlers::EventHandlerHandle<EventHandlers::WindowCloseHandler>
EventHandlers::AddHandler(const std::function<WindowCloseHandler>& handler)
{
    return window_close_handlers.insert(window_close_handlers.end(), handler);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowCloseHandler>
EventHandlers::AddHandler(std::function<WindowCloseHandler>&& handler)
{
    return window_close_handlers.insert(window_close_handlers.end(), std::move(handler));
}

void EventHandlers::EraseHandler(EventHandlerHandle<WindowCloseHandler> handle) noexcept
{
    window_close_handlers.erase(handle.it);
}

void EventHandlers::Apply(const WindowCloseEvent& event)
{
    for(auto& cback: window_close_handlers)
        cback(event);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowResizedHandler>
EventHandlers::AddHandler(const std::function<WindowResizedHandler>& handler)
{
    return window_resized_handlers.insert(window_resized_handlers.end(), handler);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowResizedHandler>
EventHandlers::AddHandler(std::function<WindowResizedHandler>&& handler)
{
    return window_resized_handlers.insert(window_resized_handlers.end(), std::move(handler));
}

void EventHandlers::EraseHandler(EventHandlerHandle<WindowResizedHandler> handle) noexcept
{
    window_resized_handlers.erase(handle.it);
}

void EventHandlers::Apply(const WindowResizedEvent& event)
{
    for(auto& cback: window_resized_handlers)
        cback(event);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowExposedHandler>
EventHandlers::AddHandler(const std::function<WindowExposedHandler>& handler)
{
    return window_exposed_handlers.insert(window_exposed_handlers.end(), handler);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowExposedHandler>
EventHandlers::AddHandler(std::function<WindowExposedHandler>&& handler)
{
    return window_exposed_handlers.insert(window_exposed_handlers.end(), std::move(handler));
}

void EventHandlers::EraseHandler(EventHandlerHandle<WindowExposedHandler> handle) noexcept
{
    window_exposed_handlers.erase(handle.it);
}

void EventHandlers::Apply(const WindowExposedEvent& event)
{
    for(auto& cback: window_exposed_handlers)
        cback(event);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowMovedHandler>
EventHandlers::AddHandler(const std::function<WindowMovedHandler>& handler)
{
    return window_moved_handlers.insert(window_moved_handlers.end(), handler);
}

EventHandlers::EventHandlerHandle<EventHandlers::WindowMovedHandler>
EventHandlers::AddHandler(std::function<WindowMovedHandler>&& handler)
{
    return window_moved_handlers.insert(window_moved_handlers.end(), std::move(handler));
}

void EventHandlers::EraseHandler(EventHandlerHandle<WindowMovedHandler> handle) noexcept
{
    window_moved_handlers.erase(handle.it);
}

void EventHandlers::Apply(const WindowMovedEvent& event)
{
    for(auto& cback: window_moved_handlers)
        cback(event);
}