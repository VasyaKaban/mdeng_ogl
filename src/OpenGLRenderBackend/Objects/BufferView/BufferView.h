#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/BufferView.h"

namespace OpenGL
{
    class BufferView : public Render::BufferView, hrs::non_copyable, hrs::non_movable
    {
    public:
        BufferView(Device* _parent, const Render::BufferViewInfo& info);
        virtual ~BufferView() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;
    };
};