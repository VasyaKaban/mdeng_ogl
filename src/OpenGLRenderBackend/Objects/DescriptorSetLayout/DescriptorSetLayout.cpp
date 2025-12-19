#include "DescriptorSetLayout.h"
#include <algorithm>
#include "../Device/Device.h"

namespace OpenGL
{
    static void append_layout_bindings(BindingsClass& binding_class,
                                       std::size_t size,
                                       std::size_t descriptor_size,
                                       std::size_t& bindigns_offset,
                                       std::size_t& allocation_offset,
                                       std::vector<std::uint32_t>& bindings)
    {
        binding_class.bindings = std::span{bindings.data() + bindigns_offset, size};
        binding_class.allocation_offset = allocation_offset;
        bindigns_offset += size;
        allocation_offset += size * descriptor_size;
    }

    template<typename T>
    static T* get_descriptor_desc(std::span<std::byte>& descriptors_data,
                                  std::size_t allocation_offset,
                                  std::size_t decsriptor_size,
                                  std::size_t index) noexcept
    {
        return reinterpret_cast<T*>(descriptors_data.data() + allocation_offset +
                                    index * decsriptor_size);
    }

    DescriptorSetLayout::DescriptorSetLayout(Device* _parent,
                                             const Render::DescriptorSetLayoutInfo& info)
        : parent(_parent)
    {
        //we do not take into account duplicated linear indices from the same descriptor types -> this can lead to more memory usage(check this from user code before creating layout)

        std::size_t texture_bindings_size = 0;
        std::size_t uniform_buffer_bindings_size = 0;
        std::size_t storage_buffer_bindings_size = 0;
        std::size_t storage_image_bindings_size = 0;

        for(const auto& binding: info.bindings)
        {
            switch(binding.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                case Render::DescriptorType::SampledImage:
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                case Render::DescriptorType::InputAttachment:
                    texture_bindings_size += binding.descriptor_count;
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
            }
        }

        std::size_t bindigns_offset = 0;
        std::size_t allocation_offset = 0;
        bindings.resize(texture_bindings_size + uniform_buffer_bindings_size +
                        storage_buffer_bindings_size + storage_image_bindings_size);

        append_layout_bindings(textures,
                               texture_bindings_size,
                               TEXTURE_DESCRIPTOR_SIZE,
                               bindigns_offset,
                               allocation_offset,
                               bindings);
        append_layout_bindings(uniform_buffers,
                               uniform_buffer_bindings_size,
                               UNIFORM_BUFFER_DESCRIPTOR_SIZE,
                               bindigns_offset,
                               allocation_offset,
                               bindings);
        append_layout_bindings(storage_buffers,
                               storage_buffer_bindings_size,
                               STORAGE_BUFFER_DESCRIPTOR_SIZE,
                               bindigns_offset,
                               allocation_offset,
                               bindings);
        append_layout_bindings(storage_images,
                               storage_image_bindings_size,
                               STORAGE_IMAGE_DESCRIPTOR_SIZE,
                               bindigns_offset,
                               allocation_offset,
                               bindings);

        std::size_t texture_bindings_index = 0;
        std::size_t uniform_bindings_index = 0;
        std::size_t shader_storage_bindings_index = 0;
        std::size_t image_bindings_index = 0;
        for(const auto& binding: info.bindings)
        {
            switch(binding.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                case Render::DescriptorType::SampledImage:
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                case Render::DescriptorType::InputAttachment:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        textures.bindings[texture_bindings_index + i] = binding.binding + i;
                    }
                    texture_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::UnifromBuffer:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        uniform_buffers.bindings[uniform_bindings_index + i] = binding.binding + i;
                    }
                    uniform_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageBuffer:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        storage_buffers.bindings[shader_storage_bindings_index + i] =
                            binding.binding + i;
                    }
                    shader_storage_bindings_index += binding.descriptor_count;
                    break;
                case Render::DescriptorType::StorageImage:
                    for(std::uint32_t i = 0; i < binding.descriptor_count; i++)
                    {
                        storage_images.bindings[image_bindings_index + i] = binding.binding + i;
                    }
                    image_bindings_index += binding.descriptor_count;
                    break;
            }
        }

        std::ranges::sort(textures.bindings);
        std::ranges::sort(uniform_buffers.bindings);
        std::ranges::sort(storage_buffers.bindings);
        std::ranges::sort(storage_images.bindings);
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {}

    Render::Device* DescriptorSetLayout::GetParent() const noexcept
    {
        return parent;
    }

    std::size_t DescriptorSetLayout::GetBindingsAllocationSize() const noexcept
    {
        return textures.bindings.size() * TEXTURE_DESCRIPTOR_SIZE +
               uniform_buffers.bindings.size() * UNIFORM_BUFFER_DESCRIPTOR_SIZE +
               storage_buffers.bindings.size() * STORAGE_BUFFER_DESCRIPTOR_SIZE +
               storage_images.bindings.size() * STORAGE_IMAGE_DESCRIPTOR_SIZE;
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

    DescriptorTextureDesc* DescriptorSetLayout::TranslateTextureDescriptor(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, textures, descriptors_data, TEXTURE_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorTextureDesc*>(address);
    }

    DescriptorUniformBufferDesc* DescriptorSetLayout::TranslateUniformBufferBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, uniform_buffers, descriptors_data, UNIFORM_BUFFER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorUniformBufferDesc*>(address);
    }

    DescriptorStorageBufferDesc* DescriptorSetLayout::TranslateStorageBufferBinding(
        std::uint32_t binding,
        std::span<std::byte> descriptors_data) const noexcept
    {
        std::byte* address =
            translate(binding, storage_buffers, descriptors_data, STORAGE_BUFFER_DESCRIPTOR_SIZE);
        if(address == nullptr)
            return nullptr;

        return reinterpret_cast<DescriptorStorageBufferDesc*>(address);
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

    void DescriptorSetLayout::Bind(CommandBuffer& cmd, std::span<std::byte> descriptors_data) const
    {
        for(std::size_t i = 0; i < textures.bindings.size(); i++)
        {
            auto desc = get_descriptor_desc<DescriptorTextureDesc>(descriptors_data,
                                                                   textures.allocation_offset,
                                                                   TEXTURE_DESCRIPTOR_SIZE,
                                                                   i);

            parent->GetLoader().BindTextureUnit(textures.bindings[i], desc->image_view);

            if(desc->sampler != OGL_NULL_HANDLE)
                parent->GetLoader().BindSampler(textures.bindings[i], desc->sampler);
        }

        for(std::size_t i = 0; i < uniform_buffers.bindings.size(); i++)
        {
            auto desc =
                get_descriptor_desc<DescriptorUniformBufferDesc>(descriptors_data,
                                                                 uniform_buffers.allocation_offset,
                                                                 UNIFORM_BUFFER_DESCRIPTOR_SIZE,
                                                                 i);

            parent->GetLoader().BindBufferRange(GL_UNIFORM_BUFFER,
                                                uniform_buffers.bindings[i],
                                                desc->buffer,
                                                desc->offset,
                                                desc->size);
        }

        for(std::size_t i = 0; i < storage_buffers.bindings.size(); i++)
        {
            auto desc =
                get_descriptor_desc<DescriptorStorageBufferDesc>(descriptors_data,
                                                                 storage_buffers.allocation_offset,
                                                                 STORAGE_BUFFER_DESCRIPTOR_SIZE,
                                                                 i);

            parent->GetLoader().BindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                                storage_buffers.bindings[i],
                                                desc->buffer,
                                                desc->offset,
                                                desc->size);
        }

        for(std::size_t i = 0; i < storage_images.bindings.size(); i++)
        {
            auto desc =
                get_descriptor_desc<DescriptorStorageImageDesc>(descriptors_data,
                                                                storage_images.allocation_offset,
                                                                STORAGE_IMAGE_DESCRIPTOR_SIZE,
                                                                i);

            GLuint image_handle = desc->image_view;
            parent->GetLoader().BindImageTextures(storage_images.bindings[i], 1, &image_handle);
        }
    }
};