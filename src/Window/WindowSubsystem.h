#pragma once

#include <memory>
#include "../hrs/non_creatable.hpp"

class GraphicWindow;
struct GraphicWindowInfo;

class WindowSubsystem : hrs::non_copyable, hrs::non_movable
{
    WindowSubsystem(const GraphicWindowInfo& window_info);
public:
    ~WindowSubsystem() = default;

    static WindowSubsystem* Init(const GraphicWindowInfo& window_info);
    static WindowSubsystem* GetSubsystem() noexcept;
    static void Close();

    void PollEvents();

    GraphicWindow* GetGraphicWindow() noexcept;
    const GraphicWindow* GetGraphicWindow() const noexcept;
private:
    static inline std::unique_ptr<WindowSubsystem> subsystem = {};

    std::unique_ptr<GraphicWindow> graphic_window;
};