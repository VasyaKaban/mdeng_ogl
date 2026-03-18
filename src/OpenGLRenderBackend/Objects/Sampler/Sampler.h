#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Sampler.h"

namespace OpenGL
{
    class Sampler final : public Render::Sampler, Core::NonCopyable, Core::NonMovable
    {
    public:
        Sampler(Device* _parent, const Render::SamplerInfo& info);
        virtual ~Sampler() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;
    private:
        Device* parent;
        GLHandle handle;
    };
};