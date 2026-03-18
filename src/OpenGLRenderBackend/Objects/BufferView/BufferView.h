#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/BufferView.h"

namespace OpenGL
{
    class BufferView final : public Render::BufferView, Core::NonCopyable, Core::NonMovable
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