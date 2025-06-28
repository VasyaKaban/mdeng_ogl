#include "WindowSubsystem.h"
#include "GraphicWindow.h"
#include <stdexcept>
#include "../hrs/scoped_call.hpp"

WindowSubsystem::WindowSubsystem(const GraphicWindowInfo& window_info)
{
    graphic_window.reset(new GraphicWindow(window_info));
}

WindowSubsystem* WindowSubsystem::Init(const GraphicWindowInfo& window_info)
{
    if(subsystem)
        return subsystem.get();

    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(SDL_GetError());

    hrs::scoped_call cleanup(
        []()
        {
            SDL_Quit();
        });

    subsystem.reset(new WindowSubsystem(window_info));

    cleanup.drop();

    return subsystem.get();
}

WindowSubsystem* WindowSubsystem::GetSubsystem() noexcept
{
    return subsystem.get();
}

void WindowSubsystem::Close()
{
    if(subsystem)
    {
        subsystem.reset();
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
                        if(window_event.windowID == graphic_window->id)
                            graphic_window->GetEventHandlers().close_handler();
                    }
                    break;
                }
            }
            break;
        }
    }
}

GraphicWindow* WindowSubsystem::GetGraphicWindow() noexcept
{
    return graphic_window.get();
}

const GraphicWindow* WindowSubsystem::GetGraphicWindow() const noexcept
{
    return graphic_window.get();
}
