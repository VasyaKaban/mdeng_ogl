#pragma once

#include <functional>
#include "Core/Render/Render.h"

using TransferCallback = std::function<void(Render::CommandBuffer* cmd)>;

struct TransferBufferOperation
{
    Render::Buffer* buffer;
    std::span<const Render::MemoryBufferCopyRegion> regions;
};

struct TransferImageOperation
{
    Render::Image* image;
    std::span<const Render::MemoryImageCopyRegion> regions;
};

struct TransferCallbackOperation
{
    TransferCallback cback;
};

struct TransferRegion
{
    std::variant<TransferBufferOperation, TransferImageOperation, TransferCallbackOperation> op;
};