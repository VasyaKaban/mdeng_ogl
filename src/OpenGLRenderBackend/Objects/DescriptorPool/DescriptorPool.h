#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/DescriptorPool.h"

namespace OpenGL
{
    class DescriptorPool : public Render::DescriptorPool, hrs::non_copyable, hrs::non_movable
    {
    public:
        DescriptorPool(Context* _parent, const Render::DescriptorPoolInfo& info);
        virtual ~DescriptorPool() override;

        virtual Render::DescriptorSet*
        Allocate(const Render::DescriptorSetAllocateInfo& info) override;

        virtual Render::Context* GetContext() const noexcept override;
    private:
        Context* parent;
        std::byte* data;
        std::size_t size;
        std::size_t offset;
    };
};