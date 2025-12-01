#include "RenderBackend.h"

namespace Core
{
    RenderBackend::RenderBackend(GraphicWindow* _parent) noexcept
        : parent(_parent)
    {}

    GraphicWindow* RenderBackend::GetWindow() const noexcept
    {
        return parent;
    }
};