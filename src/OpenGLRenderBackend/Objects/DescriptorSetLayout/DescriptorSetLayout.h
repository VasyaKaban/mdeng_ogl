#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../../Render.h"
#include "Core/Render/Objects/DescriptorSetLayout.h"

namespace OpenGL
{
    struct BindingsClass
    {
        std::span<std::uint32_t> bindings;
        std::size_t
            allocation_offset; //offset within byte allocation from DescriptorPool(span in DescriptroSet)
    };

    class DescriptorSetLayout final : public Render::DescriptorSetLayout,
                                      Core::NonCopyable,
                                      Core::NonMovable
    {
    public:
        DescriptorSetLayout(Device* _parent, const Render::DescriptorSetLayoutInfo& info);
        virtual ~DescriptorSetLayout() override;

        virtual Render::Device* GetParent() const noexcept override;

        std::size_t GetBindingsAllocationSize() const noexcept;

        DescriptorTextureDesc*
        TranslateTextureDescriptor(std::uint32_t binding,
                                   std::span<std::byte> descriptors_data) const noexcept;

        DescriptorUniformBufferDesc*
        TranslateUniformBufferBinding(std::uint32_t binding,
                                      std::span<std::byte> descriptors_data) const noexcept;
        DescriptorStorageBufferDesc*
        TranslateStorageBufferBinding(std::uint32_t binding,
                                      std::span<std::byte> descriptors_data) const noexcept;
        DescriptorStorageImageDesc*
        TranslateStorageImageBinding(std::uint32_t binding,
                                     std::span<std::byte> descriptors_data) const noexcept;

        void Bind(CommandBuffer& cmd, std::span<std::byte> descriptors_data) const;
    private:
        Device* parent;

        std::vector<std::uint32_t> bindings;

        BindingsClass textures;
        std::vector<GLHandle> immutable_samplers;

        BindingsClass uniform_buffers;
        BindingsClass storage_buffers;
        BindingsClass storage_images;
    };
};