#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/DescriptorPool.h"

namespace OpenGL
{
    class DescriptorPool : public Render::DescriptorPool, hrs::non_copyable, hrs::non_movable
    {
    public:
        DescriptorPool(Device* _parent, const Render::DescriptorPoolInfo& info);
        virtual ~DescriptorPool() override;

        virtual Render::DescriptorSet*
        Allocate(const Render::DescriptorSetAllocateInfo& info) override;

        virtual Render::Device* GetParent() const noexcept override;
    private:
        Device* parent;
        std::byte* data;
        std::size_t size;
        std::size_t offset;
    };
};