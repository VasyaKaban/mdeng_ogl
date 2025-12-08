#include "DescriptorSet.h"
#include "../../Context/Context.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../ImageView/ImageView.h"
#include "../Sampler/Sampler.h"
#include "../Buffer/Buffer.h"
#include "../BufferView/BufferView.h"

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

                    if(image_desc != nullptr)
                    {
                        image_desc->sampler =
                            static_cast<Sampler*>(desc.desc.image_desc.sampler)->GetHandle();

                        image_desc->image_view =
                            static_cast<ImageView*>(desc.desc.image_desc.image_view)->GetHandle();
                    }
                }
                break;
                case Render::DescriptorType::UnifromBuffer:
                {
                    DescriptorBufferDesc* buffer_desc =
                        layout->TranslateUniformBufferBinding(desc.binding.linear_binding,
                                                              descriptors_data);

                    if(buffer_desc != nullptr)
                    {
                        buffer_desc->buffer =
                            static_cast<Buffer*>(desc.desc.buffer_desc.buffer)->GetHandle();
                        buffer_desc->offset = desc.desc.buffer_desc.offset;
                        buffer_desc->size = desc.desc.buffer_desc.size;
                    }
                }
                break;
                case Render::DescriptorType::StorageBuffer:
                {
                    DescriptorBufferDesc* buffer_desc =
                        layout->TranslateStorageBufferBinding(desc.binding.linear_binding,
                                                              descriptors_data);

                    if(buffer_desc != nullptr)
                    {
                        buffer_desc->buffer =
                            static_cast<Buffer*>(desc.desc.buffer_desc.buffer)->GetHandle();
                        buffer_desc->offset = desc.desc.buffer_desc.offset;
                        buffer_desc->size = desc.desc.buffer_desc.size;
                    }
                }
                break;
                case Render::DescriptorType::StorageImage:
                {
                    DescriptorStorageImageDesc* image_desc =
                        layout->TranslateStorageImageBinding(desc.binding.linear_binding,
                                                             descriptors_data);

                    if(image_desc != nullptr)
                        image_desc->image_view =
                            static_cast<ImageView*>(desc.desc.image_desc.image_view)->GetHandle();
                }
                break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                {
                    DescriptorTexelBufferDesc* buffer_desc =
                        layout->TranslateTexelBufferBinding(desc.binding.linear_binding,
                                                            descriptors_data);

                    if(buffer_desc != nullptr)
                        buffer_desc->buffer_view =
                            static_cast<BufferView*>(desc.desc.texel_buffer_view)->GetHandle();
                }
                break;
            }
        }
    }

    Render::Context* DescriptorSet::GetContext() const noexcept
    {
        return parent;
    }

    void DescriptorSet::Bind(CommandBuffer& cmd)
    {
        layout->Bind(cmd, descriptors_data);
    }
};