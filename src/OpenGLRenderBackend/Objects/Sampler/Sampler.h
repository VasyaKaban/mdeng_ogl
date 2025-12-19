#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Sampler.h"

namespace OpenGL
{
    class Sampler : public Render::Sampler, hrs::non_copyable, hrs::non_movable
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