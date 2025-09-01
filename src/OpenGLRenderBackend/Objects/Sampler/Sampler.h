#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Sampler.h"

namespace OpenGL
{
    class Sampler : public Render::Sampler, hrs::non_copyable, hrs::non_movable
    {
    public:
        Sampler(Context* _parent, const Render::SamplerInfo& info);
        virtual ~Sampler() override;

        GLHandle GetHandle() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;
    };
};