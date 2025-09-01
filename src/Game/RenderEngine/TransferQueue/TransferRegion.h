#pragma once

#include <functional>
#include <variant>
#include <span>
#include "Core/Render/Common.h"

using TransferCallback = std::function<void()>;

class Buffer;

struct TransferBufferRegion
{
    std::span<const std::uint8_t> data;
    GLintptr offset;
    Buffer* buffer;
};

class Image;

struct TransferImageRegion
{
    std::span<const std::uint8_t> data;
    GLsizei buffer_row_length;
    GLsizei buffer_image_height;
    ImageSubresourceLayers subresource_layers;
    Offset3D offset;
    Extent3D extent;
    ImageCopyDataFormat data_format;
    ImageCopyDataType data_type;
    Image* image;
    bool prefer_image_host_copy;
};

struct TransferRegion
{
    std::variant<TransferBufferRegion, TransferImageRegion, TransferCallback> data;
};