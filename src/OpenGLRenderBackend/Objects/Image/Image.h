#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Image.h"

namespace OpenGL
{
    class Image : public Render::Image, hrs::non_copyable, hrs::non_movable
    {
    public:
        Image(Context* _parent, const Render::ImageInfo& info);
        virtual ~Image() override;

        virtual const Render::ImageInfo& GetInfo() const noexcept override;

        virtual void CopyToBuffer(const Render::CommandBuffer* cmd,
                                  const Render::Buffer* dst,
                                  std::span<const Render::BufferImageCopyRegion> regions) override;

        virtual void Update(const Render::CommandBuffer* cmd,
                            std::span<const Render::MemoryImageCopyRegion> regions) override;

        GLenum GetInnerType() const noexcept;
        GLenum GetInnerFormat() const noexcept;
        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;

        Render::ImageInfo info;
        GLenum inner_type;
        GLenum inner_format;
    };
};