#pragma once

#include <memory>
#include <map>
#include "RenderBackend.h"
#include "hrs/non_creatable.hpp"
#include <SDL2/SDL_messagebox.h>

class GraphicWindow;
struct GraphicWindowInfo;

enum class MessageBoxType
{
    Error = SDL_MESSAGEBOX_ERROR,
    Warning = SDL_MESSAGEBOX_WARNING,
    Info = SDL_MESSAGEBOX_INFORMATION
};

class WindowSubsystem : hrs::non_copyable, hrs::non_movable
{
    WindowSubsystem();
public:
    ~WindowSubsystem() = default;

    static WindowSubsystem* Init();
    static WindowSubsystem* GetSubsystem() noexcept;
    static void Close();

    void PollEvents();

    GraphicWindow* CreateGraphicWindow(const GraphicWindowInfo& info,
                                       const RenderBackendInfo& render_info);

    GraphicWindow* GetGraphicWindow(std::uint32_t id) const noexcept;

    static void ShowMessageBox(const GraphicWindow* parent,
                               MessageBoxType type,
                               const char* title,
                               const char* message);
private:
    static inline WindowSubsystem* subsystem = nullptr;

    std::map<std::uint32_t, std::unique_ptr<GraphicWindow>> graphic_windows;
};