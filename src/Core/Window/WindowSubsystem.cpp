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

    subsystem = new WindowSubsystem;

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
                            const WindowCloseEvent event = {};
                            Events::Emit<WindowCloseEvent>(window, event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const WindowResizedEvent event = {
                                .resolution = WindowResolution{.width = window_event.data1,
                                                               .height = window_event.data2},
                                .drawable_resolution = window->GetDrawableResolution()};

                            Events::Emit<WindowResizedEvent>(window, event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_EXPOSED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const WindowExposedEvent event = {};
                            Events::Emit<WindowExposedEvent>(window, event);
                        }
                    }
                    break;
                    case SDL_WINDOWEVENT_MOVED:
                    {
                        GraphicWindow* window = GetGraphicWindow(window_event.windowID);
                        if(window)
                        {
                            const WindowMovedEvent event = {
                                .position = WindowPosition{.x = window_event.data1,
                                                           .y = window_event.data2}};

                            Events::Emit<WindowMovedEvent>(window, event);
                        }
                    }
                    break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                SDL_MouseMotionEvent& motion_event = event.motion;
                GraphicWindow* window = GetGraphicWindow(motion_event.windowID);
                if(window)
                {
                    const MouseMotionEvent event = {.mask = motion_event.state,
                                                    .x = motion_event.x,
                                                    .y = motion_event.y,
                                                    .motion_x = motion_event.xrel,
                                                    .motion_y = motion_event.yrel};

                    Events::Emit<MouseMotionEvent>(window, event);
                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent& button_event = event.button;
                GraphicWindow* window = GetGraphicWindow(button_event.windowID);
                if(window)
                {
                    const MouseButtonEvent event = {
                        .button = static_cast<MouseButtonBits>(1u << button_event.button),
                        .state = (button_event.type == SDL_MOUSEBUTTONDOWN ? ButtonState::Pressed :
                                                                             ButtonState::Released),
                        .clicks = button_event.clicks,
                        .x = button_event.x,
                        .y = button_event.y};

                    Events::Emit<MouseButtonEvent>(window, event);
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                SDL_MouseWheelEvent& wheel_event = event.wheel;
                GraphicWindow* window = GetGraphicWindow(wheel_event.windowID);
                if(window)
                {
                    const MouseWheelEvent event = {.scrolled_x = wheel_event.x,
                                                   .scrolled_y = wheel_event.y,
                                                   .x = wheel_event.mouseX,
                                                   .y = wheel_event.mouseY};

                    Events::Emit<MouseWheelEvent>(window, event);
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

bool WindowSubsystem::ShowMessageBox(const GraphicWindow* parent,
                                     MessageBoxType type,
                                     const char* title,
                                     const char* message)
{
    int res = SDL_ShowSimpleMessageBox(static_cast<Uint32>(type),
                                       title,
                                       message,
                                       parent ? parent->GetWindowHandle() : nullptr);

    return res == 0;
}