#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/BufferView.h"

namespace OpenGL
{
    class BufferView : public Render::BufferView, hrs::non_copyable, hrs::non_movable
    {
    public:
        BufferView(Context* _parent, const Render::BufferViewInfo& info);
        virtual ~BufferView() override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
    };
};