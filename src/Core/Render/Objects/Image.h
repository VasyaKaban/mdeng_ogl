#pragma once

#include "../Render.h"
#include "Object.h"

namespace Render
{
    class Image : public Object
    {
    public:
        virtual ~Image()
        {}

        virtual const ImageInfo& GetInfo() const noexcept = 0;

        virtual void CopyToBuffer(const CommandBuffer* cmd,
                                  const Buffer* dst,
                                  std::span<const BufferImageCopyRegion> regions) = 0;

        virtual void Update(const CommandBuffer* cmd,
                            std::span<const MemoryImageCopyRegion> regions) = 0;
    };
};