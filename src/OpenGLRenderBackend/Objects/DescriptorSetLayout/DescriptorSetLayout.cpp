#include "DescriptorSetLayout.h"
#include <algorithm>
#include "../../Context/Context.h"

namespace OpenGL
{
    DescriptorSetLayout::DescriptorSetLayout(Context* _parent,
                                             const Render::DescriptorSetLayoutInfo& info)
        : parent(_parent)
    {
        //we do not take into account duplicated linear indices from the same descriptor types -> this can lead to more memory usage(check this from user code before creating layout)

        std::size_t combined_image_sampler_bindings_size = 0;
        std::size_t uniform_buffer_bindings_size = 0;
        std::size_t storage_buffer_bindings_size = 0;
        std::size_t storage_image_bindings_size = 0;
        std::size_t texel_buffer_bindings_size = 0;

        for(const auto& binding: info.bindings)
        {
            switch(binding.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                    combined_image_sampler_bindings_size += binding.descriptor_count;
                    break;
                case Render::DescriptorType::UnifromBuffer:
                    uniform_buffer_bindings_size += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageBuffer:
                    storage_buffer_bindings_size += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageImage:
                    storage_image_bindings_size += binding.descriptor_count;
                    break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                    texel_buffer_bindings_size += binding.descriptor_count;
            }
        }

        std::size_t bindigns_offset = 0;
        std::size_t allocation_offset = 0;
        bindings.resize(combined_image_sampler_bindings_size + uniform_buffer_bindings_size +
                        storage_buffer_bindings_size + storage_image_bindings_size);

        combined_image_samplers.bindings =
            std::span{bindings.data() + bindigns_offset, combined_image_sampler_bindings_size};
        combined_image_samplers.allocation_offset = allocation_offset;
        bindigns_offset += combined_image_sampler_bindings_size;
        allocation_offset +=
            combined_image_sampler_bindings_size * COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE;

        uniform_buffers.bindings =
            std::span{bindings.data() + bindigns_offset, uniform_buffer_bindings_size};
        uniform_buffers.allocation_offset = allocation_offset;
        bindigns_offset += uniform_buffer_bindings_size;
        allocation_offset += uniform_buffer_bindings_size * UNIFORM_BUFFER_DESCRIPTOR_SIZE;

        storage_buffers.bindings =
            std::span{bindings.data() + bindigns_offset, storage_buffer_bindings_size};
        storage_buffers.allocation_offset = allocation_offset;
        bindigns_offset += storage_buffer_bindings_size;
        allocation_offset += storage_buffer_bindings_size * UNIFORM_BUFFER_DESCRIPTOR_SIZE;

        storage_images.bindings =
            std::span{bindings.data() + bindigns_offset, storage_image_bindings_size};
        storage_images.allocation_offset = allocation_offset;
        bindigns_offset += storage_image_bindings_size;
        allocation_offset += storage_image_bindings_size * STORAGE_IMAGE_DESCRIPTOR_SIZE;

        texel_buffers.bindings =
            std::span{bindings.data() + bindigns_offset, texel_buffer_bindings_size};
        texel_buffers.allocation_offset = allocation_offset;
        bindigns_offset += texel_buffer_bindings_size;
        allocation_offset += texel_buffer_bindings_size * TEXEL_BUFFER_DESCRIPTOR_SIZE;

        std::size_t combined_bindings_index = 0;
        std::size_t uniform_bindings_index = 0;
        std::size_t shader_storage_bindings_index = 0;
        std::size_t image_bindings_index = 0;
        std::size_t texel_bindings_index = 0;
        for(const auto& binding: info.bindings)
        {
            switch(binding.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        combined_image_samplers.bindings[combined_bindings_index + i] =
                            binding.binding.linear_binding + i;
                    }
                    combined_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::UnifromBuffer:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        uniform_buffers.bindings[uniform_bindings_index + i] =
                            binding.binding.linear_binding + i;
                    }
                    uniform_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageBuffer:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        storage_buffers.bindings[shader_storage_bindings_index + i] =
                            binding.binding.linear_binding + i;
                    }
                    shader_storage_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageImage:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        storage_images.bindings[image_bindings_index + i] =
                            binding.binding.linear_binding + i;
                    }
                    image_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        texel_buffers.bindings[texel_bindings_index + i] =
                            binding.binding.linear_binding + i;
                    }
                    texel_bindings_index += binding.descriptor_count;
                    break;
            }
        }

        std::ranges::sort(combined_image_samplers.bindings);
        std::ranges::sort(uniform_buffers.bindings);
        std::ranges::sort(storage_buffers.bindings);
        std::ranges::sort(storage_images.bindings);
        std::ranges::sort(texel_buffers.bindings);
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {}

    Render::Context* DescriptorSetLayout::GetContext() const noexcept
    {
        return parent;
    }

    std::size_t DescriptorSetLayout::GetBindingsAllocationSize() const noexcept
    {
        return combined_image_samplers.bindings.size() *
                   sizeof(DescriptorCombinedImageSamplerDesc) +
               (uniform_buffers.bindings.size() + storage_buffers.bindings.size()) *
                   sizeof(DescriptorBufferDesc) +
               storage_images.bindings.size() * sizeof(DescriptorStorageImageDesc) +
               texel_buffers.bindings.size() * sizeof(DescriptorTexelBufferDesc);
    }

    static std::byte* translate(std::uint32_t binding,
                                const BindingsClass& binding_class,
                                std::span<std::byte> descriptors_data,
                                std::size_t descriptor_size)
    {
        auto it = std::ranges::partition_point(binding_class.bindings,
                                               [binding](const std::uint32_t& b)
                                               {
                                                   return b < binding;
                                               });

        if(it == binding_class.bindings.end() || *it != binding)
            return nullptr;

        return descriptors_data.data() + binding_class.allocation_offset +
               (it - binding_class.bindings.begin()) * descriptor_size;
    }

    DescriptorCombinedImageSamplerDesc* DescriptorSetLayout::TranslateCombinedImageSamplerBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address = translate(binding,
                                       combined_image_samplers,
                                       descriptors_data,
                                       COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorCombinedImageSamplerDesc*>(address);
    }

    DescriptorBufferDesc* DescriptorSetLayout::TranslateUniformBufferBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, uniform_buffers, descriptors_data, UNIFORM_BUFFER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorBufferDesc*>(address);
    }

    DescriptorBufferDesc* DescriptorSetLayout::TranslateStorageBufferBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, storage_buffers, descriptors_data, STOARGE_BUFFER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorBufferDesc*>(address);
    }

    DescriptorStorageImageDesc* DescriptorSetLayout::TranslateStorageImageBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, storage_images, descriptors_data, STORAGE_IMAGE_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorStorageImageDesc*>(address);
    }

    DescriptorTexelBufferDesc* DescriptorSetLayout::TranslateTexelBufferBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, texel_buffers, descriptors_data, TEXEL_BUFFER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorTexelBufferDesc*>(address);
    }

    void DescriptorSetLayout::Bind(CommandBuffer& cmd, std::span<std::byte> descriptors_data) const
    {
        for(std::size_t i = 0; i < combined_image_samplers.bindings.size(); i++)
        {
            DescriptorCombinedImageSamplerDesc* desc =
                reinterpret_cast<DescriptorCombinedImageSamplerDesc*>(
                    descriptors_data.data() + combined_image_samplers.allocation_offset +
                    i * COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE);

            parent->GetLoader().BindTextureUnit(combined_image_samplers.bindings[i],
                                                desc->image_view);

            parent->GetLoader().BindSampler(combined_image_samplers.bindings[i], desc->sampler);
        }

        for(std::size_t i = 0; i < uniform_buffers.bindings.size(); i++)
        {
            DescriptorBufferDesc* desc = reinterpret_cast<DescriptorBufferDesc*>(
                descriptors_data.data() + uniform_buffers.allocation_offset +
                i * UNIFORM_BUFFER_DESCRIPTOR_SIZE);

            parent->GetLoader().BindBufferRange(GL_UNIFORM_BUFFER,
                                                combined_image_samplers.bindings[i],
                                                desc->buffer,
                                                desc->offset,
                                                desc->size);
        }

        for(std::size_t i = 0; i < storage_buffers.bindings.size(); i++)
        {
            DescriptorBufferDesc* desc = reinterpret_cast<DescriptorBufferDesc*>(
                descriptors_data.data() + storage_buffers.allocation_offset +
                i * STOARGE_BUFFER_DESCRIPTOR_SIZE);

            parent->GetLoader().BindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                                combined_image_samplers.bindings[i],
                                                desc->buffer,
                                                desc->offset,
                                                desc->size);
        }

        for(std::size_t i = 0; i < storage_images.bindings.size(); i++)
        {
            DescriptorStorageImageDesc* desc = reinterpret_cast<DescriptorStorageImageDesc*>(
                descriptors_data.data() + storage_images.allocation_offset +
                i * STORAGE_IMAGE_DESCRIPTOR_SIZE);

            GLuint image_handle = desc->image_view;
            parent->GetLoader().BindImageTextures(combined_image_samplers.bindings[i],
                                                  1,
                                                  &image_handle);
        }

        for(std::size_t i = 0; i < texel_buffers.bindings.size(); i++)
        {
            DescriptorTexelBufferDesc* desc = reinterpret_cast<DescriptorTexelBufferDesc*>(
                descriptors_data.data() + texel_buffers.allocation_offset +
                i * TEXEL_BUFFER_DESCRIPTOR_SIZE);

            parent->GetLoader().BindTextureUnit(texel_buffers.bindings[i], desc->buffer_view);
        }
    }
};