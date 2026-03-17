#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/DescriptorSet.h"

namespace OpenGL
{
    class DescriptorSet : public Render::DescriptorSet, Core::NonCopyable, Core::NonMovable
    {
    public:
        DescriptorSet(Device* _parent,
                      const DescriptorSetLayout* _layout,
                      std::span<std::byte> _descriptors_data);
        ~DescriptorSet();

        virtual void Update(std::span<const Render::WriteDescriptorDesc> writes,
                            std::span<const Render::CopyDescriptorDesc> copies) override;

        virtual Render::Device* GetParent() const noexcept override;

        void Bind(CommandBuffer& cmd) const;
    private:
        Device* parent;
        const DescriptorSetLayout* layout;
        std::span<std::byte> descriptors_data;
    };
};