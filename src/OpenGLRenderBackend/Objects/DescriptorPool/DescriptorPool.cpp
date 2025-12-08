#include "DescriptorPool.h"
#include "../DescriptorSetLayout/DescriptorSetLayout.h"
#include "../DescriptorSet/DescriptorSet.h"
#include "../../Context/Context.h"

namespace OpenGL
{
    DescriptorPool::DescriptorPool(Context* _parent, const Render::DescriptorPoolInfo& info)
        : parent(_parent),
          offset(0)
    {
        std::size_t allocation_size = 0;
        for(const auto& size: info.sizes)
        {
            switch(size.type)
            {
                case Render::DescriptorType::CombinedImageSampler:
                    allocation_size += COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE;
                    static_assert(COMBINED_IMAGE_SAMPLER_DESCRIPTOR_SIZE % 8 == 0);
                    break;
                case Render::DescriptorType::UnifromBuffer:
                    allocation_size += UNIFORM_BUFFER_DESCRIPTOR_SIZE;
                    static_assert(UNIFORM_BUFFER_DESCRIPTOR_SIZE % 8 == 0);
                    break;
                case Render::DescriptorType::StorageBuffer:
                    allocation_size += STOARGE_BUFFER_DESCRIPTOR_SIZE;
                    static_assert(STOARGE_BUFFER_DESCRIPTOR_SIZE % 8 == 0);
                    break;
                case Render::DescriptorType::StorageImage:
                    allocation_size += STORAGE_IMAGE_DESCRIPTOR_SIZE;
                    static_assert(STORAGE_IMAGE_DESCRIPTOR_SIZE % 8 != 0);
                    break;
                case Render::DescriptorType::UniformTexelBuffer:
                case Render::DescriptorType::StorageTexelBuffer:
                    allocation_size += TEXEL_BUFFER_DESCRIPTOR_SIZE;
                    static_assert(TEXEL_BUFFER_DESCRIPTOR_SIZE % 8 != 0);
                    break;
            }
        }

        data = static_cast<std::byte*>(
            ::operator new[](allocation_size, std::align_val_t(DESCRIPTOR_ALIGNMENT)));
        size = allocation_size;
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

        Render::DescriptorSet* set =
            new DescriptorSet(parent, layout, std::span{data + offset, allocation_size});

        offset += allocation_size;

        return set;
    }

    Render::Context* DescriptorPool::GetContext() const noexcept
    {
        return parent;
    }
};