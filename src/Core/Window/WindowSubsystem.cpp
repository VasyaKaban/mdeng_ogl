#include "WindowSubsystem.h"
#include "GraphicWindow.h"
#include <stdexcept>

WindowSubsystem::WindowSubsystem()
{}

WindowSubsystem* WindowSubsystem::Init()
{
    if(subsystem)
        return subsystem;

    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(SDL_GetError());

    return subsystem;
}

WindowSubsystem* WindowSubsystem::GetSubsystem() noexcept
{
    return subsystem;
}

void WindowSubsystem::Close()
{
    if(subsystem)
    {
        delete subsystem;
        SDL_Quit();
    }
}

void WindowSubsystem::PollEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT:
            {
                SDL_WindowEvent& window_event = event.window;
                switch(window_event.event)
                {
                    case SDL_WINDOWEVENT_CLOSE:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const EventHandlers::WindowCloseEvent event = {};
                            window->GetEventHandlers().Apply(event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const EventHandlers::WindowResizedEvent event = {
                                .resolution = WindowResolution{.width = window_event.data1,
                                                               .height = window_event.data2},
                                .drawable_resolution = window->GetDrawableResolution()};

                            window->GetEventHandlers().Apply(event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_EXPOSED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const EventHandlers::WindowExposedEvent event = {};
                            window->GetEventHandlers().Apply(event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_MOVED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const EventHandlers::WindowMovedEvent event = {
                                .position = WindowPosition{.x = window_event.data1,
                                                           .y = window_event.data2}};

                            window->GetEventHandlers().Apply(event);
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
}

GraphicWindow* WindowSubsystem::CreateGraphicWindow(const GraphicWindowInfo& info,
                                                    const RenderBackendInfo& render_info)
{
    std::unique_ptr<GraphicWindow> window(new GraphicWindow(info, render_info));
    auto it = graphic_windows.insert({window->GetID(), std::move(window)});
    return it.first->second.get();
}

GraphicWindow* WindowSubsystem::GetGraphicWindow(std::uint32_t id) const noexcept
{
    auto it = graphic_windows.find(id);
    if(it == graphic_windows.end())
        return nullptr;

    return it->second.get();
}

void WindowSubsystem::ShowMessageBox(const GraphicWindow* parent,
                                     MessageBoxType type,
                                     const char* title,
                                     const char* message)
{
    int res = SDL_ShowSimpleMessageBox(static_cast<Uint32>(type),
                                       title,
                                       message,
                                       parent ? parent->GetWindowHandle() : nullptr);
    if(res != 0)
        throw std::runtime_error(SDL_GetError());
}