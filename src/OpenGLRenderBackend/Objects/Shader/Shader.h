#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Shader.h"

namespace OpenGL
{
    class Shader : public Render::Shader, hrs::non_copyable, hrs::non_movable
    {
    public:
        Shader(Context* _parent, const Render::ShaderInfo& info);
        virtual ~Shader() override;

        GLHandle GetHandle() const noexcept;

        Render::ShaderStageFlagBits GetStage() const noexcept;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        GLHandle handle;

        Render::ShaderStageFlagBits stage;
    };
};