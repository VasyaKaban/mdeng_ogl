#pragma once

#include <functional>
#include "Core/Render/Render.h"

namespace Engine
{
    struct TransferBufferOperation
    {
        Render::Buffer* buffer;
        std::span<const Render::MemoryBufferCopyRegion> regions;
    };

    struct TransferImageOperation
    {
        Render::Image* image;
        Render::Format format;
        std::span<const Render::MemoryImageCopyRegion> regions;
    };

    struct TransferRegion
    {
        std::variant<TransferBufferOperation, TransferImageOperation> op;
    };
};