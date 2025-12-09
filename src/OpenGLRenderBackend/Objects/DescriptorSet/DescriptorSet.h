#pragma once

#include "hrs/non_creatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/DescriptorSet.h"

namespace OpenGL
{
    class DescriptorSet : public Render::DescriptorSet, hrs::non_copyable, hrs::non_movable
    {
    public:
        DescriptorSet(Context* _parent,
                      const DescriptorSetLayout* _layout,
                      std::span<std::byte> _descriptors_data);
        ~DescriptorSet();

        virtual void Write(std::span<const Render::UpdateDescriptorDesc> descs) override;

        virtual Render::Context* GetContext() const noexcept override;

        void Bind(CommandBuffer& cmd) const;
    private:
        Context* parent;
        const DescriptorSetLayout* layout;
        std::span<std::byte> descriptors_data;

        /*
        OGL:
            combined(image + sampler)
            uniform buffers
            shader buffers
            images
        */
    };
};