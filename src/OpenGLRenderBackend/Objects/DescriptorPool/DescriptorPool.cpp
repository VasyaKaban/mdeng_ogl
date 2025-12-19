#include "DescriptorPool.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../DescriptorSet/DescriptorSet.h"
#include "../Device/Device.h"
#include <algorithm>

namespace OpenGL
{
    DescriptorPool::DescriptorPool(Device* _parent, const Render::DescriptorPoolInfo& info)
        : parent(_parent),
          offset(0)
    {
        std::size_t allocation_size = 0;
        for(const auto& size: info.sizes)
        {
            switch(size.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                case Render::DescriptorType::SampledImage:
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                case Render::DescriptorType::InputAttachment:
                    allocation_size += TEXTURE_DESCRIPTOR_SIZE;
                    break;
                case Render::DescriptorType::UnifromBuffer:
                    allocation_size += UNIFORM_BUFFER_DESCRIPTOR_SIZE;
                    break;
                case Render::DescriptorType::StorageBuffer:
                    allocation_size += STORAGE_BUFFER_DESCRIPTOR_SIZE;
                    break;
                case Render::DescriptorType::StorageImage:
                    allocation_size += STORAGE_IMAGE_DESCRIPTOR_SIZE;
                    break;
            }
        }

        data = static_cast<std::byte*>(
            ::operator new[](allocation_size, std::align_val_t(DESCRIPTOR_ALIGNMENT)));
        size = allocation_size;

        std::ranges::fill_n(
            data,
            size,
            std::byte{
                0}); //fill with zeros because sampled images, input attachments, texel buffers(Uniform and storage) should not use sampler
    }

    DescriptorPool::~DescriptorPool()
    {
        ::operator delete[](data, std::align_val_t(DESCRIPTOR_ALIGNMENT));
    }

    Render::DescriptorSet* DescriptorPool::Allocate(const Render::DescriptorSetAllocateInfo& info)
    {
        const DescriptorSetLayout* layout = static_cast<const DescriptorSetLayout*>(info.layout);

        auto allocation_size = layout->GetBindingsAllocationSize();
        if(layout->GetBindingsAllocationSize() > size - offset)
            return nullptr;

        auto descriptors_data = std::span{data + offset, allocation_size};
        Render::DescriptorSet* set = new DescriptorSet(parent, layout, descriptors_data);

        offset += allocation_size;

        return set;
    }

    Render::Device* DescriptorPool::GetParent() const noexcept
    {
        return parent;
    }
};