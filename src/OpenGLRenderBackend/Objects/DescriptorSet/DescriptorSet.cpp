#include "DescriptorSet.h"
#include "../Device/Device.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../ImageView/ImageView.h"
#include "../Sampler/Sampler.h"
#include "../Buffer/Buffer.h"
#include "../BufferView/BufferView.h"
#include <cassert>

namespace OpenGL
{
    DescriptorSet::DescriptorSet(Device* _parent,
                                 const DescriptorSetLayout* _layout,
                                 std::span<std::byte> _descriptors_data)
        : parent(_parent),
          layout(_layout),
          descriptors_data(_descriptors_data)
    {}

    DescriptorSet::~DescriptorSet()
    {
        //noop due to linear descriptor pool
    }

    void DescriptorSet::Update(std::span<const Render::WriteDescriptorDesc> writes,
                               std::span<const Render::CopyDescriptorDesc> copies)
    {
        for(const auto& desc: writes)
        {
            switch(desc.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                case Render::DescriptorType::SampledImage:
                case Render::DescriptorType::InputAttachment:
                {
                    DescriptorTextureDesc* image_desc =
                        layout->TranslateTextureDescriptor(desc.binding, descriptors_data);

                    assert(image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorTextureDesc* array_desc = image_desc + i;
                        Render::DescriptorImageDesc* array_descriptor = desc.desc.image_desc + i;

                        if(desc.type == Render::DescriptorType::CombinedImageSampler)
                        {
                            array_desc->sampler =
                                static_cast<Sampler*>(array_descriptor->sampler)->GetHandle();
                        }

                        array_desc->image_view =
                            static_cast<ImageView*>(array_descriptor->image_view)->GetHandle();
                    }
                }
                break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                {
                    DescriptorTextureDesc* image_desc =
                        layout->TranslateTextureDescriptor(desc.binding, descriptors_data);

                    assert(image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorTextureDesc* array_desc = image_desc + i;
                        Render::BufferView* array_descriptor = desc.desc.texel_buffer_view[i];

                        array_desc->image_view =
                            static_cast<BufferView*>(array_descriptor)->GetHandle();
                    }
                }
                break;
                case Render::DescriptorType::UnifromBuffer:
                {
                    DescriptorUniformBufferDesc* buffer_desc =
                        layout->TranslateUniformBufferBinding(desc.binding, descriptors_data);

                    assert(buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorUniformBufferDesc* array_desc = buffer_desc + i;
                        Render::DescriptorBufferDesc* array_descriptor = desc.desc.buffer_desc + i;

                        array_desc->buffer =
                            static_cast<Buffer*>(array_descriptor->buffer)->GetHandle();
                        array_desc->offset = array_descriptor->offset;
                        array_desc->size = array_descriptor->size;
                    }
                }
                break;
                case Render::DescriptorType::StorageBuffer:
                {
                    DescriptorStorageBufferDesc* buffer_desc =
                        layout->TranslateStorageBufferBinding(desc.binding, descriptors_data);

                    assert(buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorStorageBufferDesc* array_desc = buffer_desc + i;
                        Render::DescriptorBufferDesc* array_descriptor = desc.desc.buffer_desc + i;

                        array_desc->buffer =
                            static_cast<Buffer*>(array_descriptor->buffer)->GetHandle();
                        array_desc->offset = array_descriptor->offset;
                        array_desc->size = array_descriptor->size;
                    }
                }
                break;
                case Render::DescriptorType::StorageImage:
                {
                    DescriptorStorageImageDesc* image_desc =
                        layout->TranslateStorageImageBinding(desc.binding, descriptors_data);

                    assert(image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorStorageImageDesc* array_desc = image_desc + i;
                        Render::DescriptorImageDesc* array_descriptor = desc.desc.image_desc + i;

                        array_desc->image_view =
                            static_cast<ImageView*>(array_descriptor->image_view)->GetHandle();
                    }
                }
                break;
            }
        }

        for(const auto& desc: copies)
        {
            DescriptorSet* src_set = static_cast<DescriptorSet*>(desc.src_set);

            switch(desc.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                case Render::DescriptorType::SampledImage:
                case Render::DescriptorType::InputAttachment:
                {
                    DescriptorTextureDesc* src_image_desc =
                        src_set->layout->TranslateTextureDescriptor(desc.src_binding,
                                                                    src_set->descriptors_data);

                    DescriptorTextureDesc* dst_image_desc =
                        layout->TranslateTextureDescriptor(desc.dst_binding, descriptors_data);

                    assert(src_image_desc != nullptr && dst_image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorTextureDesc* src_array_desc = src_image_desc + i;
                        DescriptorTextureDesc* dst_array_desc = dst_image_desc + i;

                        if(desc.type == Render::DescriptorType::CombinedImageSampler)
                            dst_array_desc->sampler = src_array_desc->sampler;

                        dst_array_desc->image_view = src_array_desc->image_view;
                    }
                }
                break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                {
                    DescriptorTextureDesc* src_image_desc =
                        src_set->layout->TranslateTextureDescriptor(desc.src_binding,
                                                                    src_set->descriptors_data);

                    DescriptorTextureDesc* dst_image_desc =
                        layout->TranslateTextureDescriptor(desc.dst_binding, descriptors_data);

                    assert(src_image_desc != nullptr && dst_image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorTextureDesc* src_array_desc = src_image_desc + i;
                        DescriptorTextureDesc* dst_array_desc = dst_image_desc + i;

                        dst_array_desc->image_view = src_array_desc->image_view;
                    }
                }
                break;
                case Render::DescriptorType::UnifromBuffer:
                {
                    DescriptorUniformBufferDesc* src_buffer_desc =
                        src_set->layout->TranslateUniformBufferBinding(desc.src_binding,
                                                                       src_set->descriptors_data);

                    DescriptorUniformBufferDesc* dst_buffer_desc =
                        layout->TranslateUniformBufferBinding(desc.dst_binding, descriptors_data);

                    assert(src_buffer_desc != nullptr && dst_buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorUniformBufferDesc* src_array_desc = src_buffer_desc + i;
                        DescriptorUniformBufferDesc* dst_array_desc = dst_buffer_desc + i;

                        dst_array_desc->buffer = src_array_desc->buffer;
                        dst_array_desc->offset = src_array_desc->offset;
                        dst_array_desc->size = src_array_desc->size;
                    }
                }
                break;
                case Render::DescriptorType::StorageBuffer:
                {
                    DescriptorStorageBufferDesc* src_buffer_desc =
                        src_set->layout->TranslateStorageBufferBinding(desc.src_binding,
                                                                       src_set->descriptors_data);

                    DescriptorStorageBufferDesc* dst_buffer_desc =
                        layout->TranslateStorageBufferBinding(desc.dst_binding, descriptors_data);

                    assert(src_buffer_desc != nullptr && dst_buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorStorageBufferDesc* src_array_desc = src_buffer_desc + i;
                        DescriptorStorageBufferDesc* dst_array_desc = dst_buffer_desc + i;

                        dst_array_desc->buffer = src_array_desc->buffer;
                        dst_array_desc->offset = src_array_desc->offset;
                        dst_array_desc->size = src_array_desc->size;
                    }
                }
                break;
                case Render::DescriptorType::StorageImage:
                {
                    DescriptorStorageImageDesc* src_image_desc =
                        src_set->layout->TranslateStorageImageBinding(desc.src_binding,
                                                                      src_set->descriptors_data);

                    DescriptorStorageImageDesc* dst_image_desc =
                        layout->TranslateStorageImageBinding(desc.dst_binding, descriptors_data);

                    assert(src_image_desc != nullptr && dst_image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorStorageImageDesc* src_array_desc = src_image_desc + i;
                        DescriptorStorageImageDesc* dst_array_desc = dst_image_desc + i;

                        dst_array_desc->image_view = src_array_desc->image_view;
                    }
                }
                break;
            }
        }
    }

    Render::Device* DescriptorSet::GetParent() const noexcept
    {
        return parent;
    }

    void DescriptorSet::Bind(CommandBuffer& cmd) const
    {
        layout->Bind(cmd, descriptors_data);
    }
};