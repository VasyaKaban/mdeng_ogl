#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/Shader.h"

namespace OpenGL
{
    class Shader : public Render::Shader, Core::NonCopyable, Core::NonMovable
    {
    public:
        Shader(Device* _parent, const Render::ShaderInfo& info);
        virtual ~Shader() override;

        virtual Render::Device* GetParent() const noexcept override;

        GLHandle GetHandle() const noexcept;

        Render::ShaderStageFlagBits GetStage() const noexcept;
    private:
        Device* parent;
        GLHandle handle;

        Render::ShaderStageFlagBits stage;
    };
};