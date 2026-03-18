#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/ImageView.h"

namespace OpenGL
{
    class ImageView final : public Render::ImageView, Core::NonCopyable, Core::NonMovable
    {
    public:
        ImageView(Device* _parent, const Render::ImageViewInfo& info);
        virtual ~ImageView() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;
    };
};