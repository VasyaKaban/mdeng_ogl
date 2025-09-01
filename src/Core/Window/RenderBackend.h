#pragma once

enum class RenderBackendType
{
    OpenGL
};

struct RenderBackendInfo
{
    RenderBackendType type;
};

class GraphicWindow;

class RenderBackend
{
public:
    RenderBackend(GraphicWindow* _parent) noexcept;

    virtual RenderBackendType GetType() const noexcept = 0;

    GraphicWindow* GetWindow() const noexcept;
protected:
    GraphicWindow* parent;
};