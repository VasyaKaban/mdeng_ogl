#pragma once

#include <functional>
#include "Core/Render/Render.h"

using TransferCallback = std::function<void()>;

struct TransferBufferOperation
{
    Render::Buffer* buffer;
    std::vector<Render::MemoryBufferCopyRegion> regions;
};

struct TransferImageOperation
{
    Render::Image* image;
    std::vector<Render::MemoryImageCopyRegion> regions;
};

struct TransferCallbackOperation
{
    TransferCallback cback;
};

struct TransferRegion
{
    std::variant<TransferBufferOperation, TransferImageOperation, TransferCallbackOperation> op;
};