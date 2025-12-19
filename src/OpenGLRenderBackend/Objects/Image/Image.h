#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Image.h"

namespace OpenGL
{
    class Image : public Render::Image, hrs::non_copyable, hrs::non_movable
    {
    public:
        Image(Device* _parent, const Render::ImageInfo& info);
        virtual ~Image() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLenum GetInnerType() const noexcept;
        GLenum GetInnerFormat() const noexcept;
        Render::Format GetFormat() const noexcept;

        const TransferImageTypeFormat& GetTransferImageTypeFormatPair() const noexcept;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;

        GLenum inner_type;
        GLenum inner_format;
        Render::Format format;

        TransferImageTypeFormat transfer_type_format_pair;
    };
};