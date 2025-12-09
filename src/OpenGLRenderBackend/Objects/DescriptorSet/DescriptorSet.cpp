#include "DescriptorSet.h"
#include "../../Context/Context.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../ImageView/ImageView.h"
#include "../Sampler/Sampler.h"
#include "../Buffer/Buffer.h"
#include "../BufferView/BufferView.h"
#include <cassert>

namespace OpenGL
{
    DescriptorSet::DescriptorSet(Context* _parent,
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

    void DescriptorSet::Write(std::span<const Render::UpdateDescriptorDesc> descs)
    {
        for(const auto& desc: descs)
        {
            switch(desc.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                {
                    DescriptorCombinedImageSamplerDesc* image_desc =
                        layout->TranslateCombinedImageSamplerBinding(desc.binding.linear_binding,
                                                                     descriptors_data);

                    assert(image_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorCombinedImageSamplerDesc* array_desc = image_desc + i;
                        Render::DescriptorImageDesc* array_descriptor = desc.desc.image_desc + i;

                        array_desc->sampler =
                            static_cast<Sampler*>(array_descriptor->sampler)->GetHandle();

                        array_desc->image_view =
                            static_cast<ImageView*>(array_descriptor->image_view)->GetHandle();
                    }
                }
                break;
                case Render::DescriptorType::UnifromBuffer:
                {
                    DescriptorBufferDesc* buffer_desc =
                        layout->TranslateUniformBufferBinding(desc.binding.linear_binding,
                                                              descriptors_data);

                    assert(buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorBufferDesc* array_desc = buffer_desc + i;
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
                    DescriptorBufferDesc* buffer_desc =
                        layout->TranslateStorageBufferBinding(desc.binding.linear_binding,
                                                              descriptors_data);

                    assert(buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorBufferDesc* array_desc = buffer_desc + i;
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
                        layout->TranslateStorageImageBinding(desc.binding.linear_binding,
                                                             descriptors_data);

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
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                {
                    DescriptorTexelBufferDesc* buffer_desc =
                        layout->TranslateTexelBufferBinding(desc.binding.linear_binding,
                                                            descriptors_data);

                    assert(buffer_desc != nullptr);

                    for(std::uint32_t i = 0; i < desc.descriptor_count; i++)
                    {
                        DescriptorTexelBufferDesc* array_desc = buffer_desc + i;
                        Render::BufferView* array_descriptor = desc.desc.texel_buffer_view[i];

                        array_desc->buffer_view =
                            static_cast<BufferView*>(array_descriptor)->GetHandle();
                    }
                }
                break;
            }
        }
    }

    Render::Context* DescriptorSet::GetContext() const noexcept
    {
        return parent;
    }

    void DescriptorSet::Bind(CommandBuffer& cmd) const
    {
        layout->Bind(cmd, descriptors_data);
    }
};