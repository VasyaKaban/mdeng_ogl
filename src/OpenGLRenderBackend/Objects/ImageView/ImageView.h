#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/ImageView.h"

namespace OpenGL
{
    class ImageView : public Render::ImageView, hrs::non_copyable, hrs::non_movable
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