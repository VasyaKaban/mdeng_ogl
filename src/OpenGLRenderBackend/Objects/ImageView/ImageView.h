#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/ImageView.h"

namespace OpenGL
{
    class ImageView : public Render::ImageView, hrs::non_copyable, hrs::non_movable
    {
    public:
        ImageView(Context* _parent, const Render::ImageViewInfo& info);
        virtual ~ImageView() override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
    };
};