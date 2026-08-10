#include "../MemoryPool.h"
#include "../Binary.hpp"

#include <cassert>

namespace Core
{
    static DeviceSize SetBlockSpecialField(DeviceSize data_offset, Bool is_free) noexcept
    {
        DeviceSize field = 0;
        if(is_free)
            field |= PoolNodeIsFreeFlagMask;

        if(data_offset != 0)
            field |= (data_offset & PoolNodeDataOffsetsMask);

        return field;
    }

    static Bool GetBlockSpecialFieldIsFree(DeviceSize field) noexcept
    {
        return static_cast<Bool>(field & PoolNodeIsFreeFlagMask);
    }

    static DeviceSize GetBlockSpecialFieldDataOffset(DeviceSize field) noexcept
    {
        return field & PoolNodeDataOffsetsMask;
    }

    MemoryPool::MemoryPool(Allocator allocator, DeviceSize size)
        : free_list(nullptr),
          allocator(allocator)
    {
        if(MemoryPool::BaseHeaderGranularity > size)
            size = MemoryPool::BaseHeaderGranularity;
        else
        {
            if(!Align(size, MemoryPool::BaseHeaderGranularity)) //go to the lower bound if there is not enough memory
            {
                size = (size / MemoryPool::BaseHeaderGranularity) * MemoryPool::BaseHeaderGranularity;
            }
        }

        Void* ptr = this->allocator.Allocate(MemoryRequirements{.alignment = MemoryPool::BaseHeaderGranularity, .size = size});

        this->memory = Span(reinterpret_cast<UInt8*>(ptr), size);

        new(this->memory.GetData()) MemoryPoolFreeNode{.header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = nullptr, .global_list_next = nullptr, .size = size},
                                                       .free_list_prev = nullptr,
                                                       .free_list_next = nullptr};
    }

    MemoryPool::~MemoryPool()
    {
        if(!this->memory.IsEmpty())
            this->allocator.Deallocate(this->memory.GetData());
    }

    MemoryPool::MemoryPool(MemoryPool&& pool) noexcept
        : free_list(Exchange(pool.free_list, nullptr)),
          allocator(pool.allocator),
          memory(Exchange(pool.memory, Span<UInt8>{}))
    {}

    MemoryPool& MemoryPool::operator=(MemoryPool&& pool) noexcept
    {
        this->~MemoryPool();

        this->free_list = Exchange(pool.free_list, nullptr);
        this->allocator = pool.allocator;
        this->memory = Exchange(pool.memory, Span<UInt8>{});

        return *this;
    }

    Void* MemoryPool::Allocate(const MemoryRequirements& req) noexcept
    {
        if(req.size == 0)
            return nullptr;

        MemoryPoolFreeNode* node = this->free_list;
        while(node != nullptr)
        {
            //header, padding, prefix, data, padding
            UInt8* base_address = reinterpret_cast<UInt8*>(node);
            UInt8* data_address = base_address + sizeof(MemoryPoolAllocatedNode) + sizeof(MemoryPoolAllocatedNodeDataPrefixHeader);

            if(!AlignAddress(data_address, node->header.size - (data_address - base_address), req.alignment))
                node = node->free_list_next;
            else
            {
                DeviceSize remain_size = (base_address + node->header.size) - data_address;
                if(remain_size < req.size)
                {
                    node = node->free_list_next;
                }
                else
                {
                    UInt8* border_node_address = data_address;
                    if(!AlignAddress(border_node_address, node->header.size - (border_node_address - base_address), MemoryPool::BaseHeaderGranularity))
                        node = node->free_list_next;
                    else
                    {
                        MemoryPoolFreeNode* prev_free = node->free_list_prev;
                        MemoryPoolFreeNode* next_free = node->free_list_next;

                        MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
                        MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

                        DeviceSize node_size = node->header.size;

                        DeviceSize remain_node_size = node->header.size - (border_node_address - base_address);
                        if(remain_node_size < MemoryPool::BaseHeaderGranularity) //small node -> take all
                        {
                            //place header
                            MemoryPoolAllocatedNode* main_node =
                                new(base_address) MemoryPoolAllocatedNode{MemoryPoolNodeHeader{.block_special = SetBlockSpecialField(data_address - base_address, false),
                                                                                               .global_list_prev = prev_ablk,
                                                                                               .global_list_next = next_ablk,
                                                                                               .size = node_size}};
                            //place prefix
                            new(data_address - sizeof(MemoryPoolAllocatedNodeDataPrefixHeader))
                                MemoryPoolAllocatedNodeDataPrefixHeader{.header_offset = static_cast<DeviceSize>((data_address - base_address))};

                            if(prev_free != nullptr)
                            {
                                prev_free->free_list_next = next_free;
                                prev_free->header.global_list_next = &main_node->header;
                            }

                            if(next_free != nullptr)
                            {
                                next_free->free_list_prev = prev_free;
                                next_free->header.global_list_prev = &main_node->header;
                            }

                            if(prev_ablk != nullptr)
                                prev_ablk->global_list_next = &main_node->header;

                            if(next_ablk != nullptr)
                                next_ablk->global_list_prev = &main_node->header;

                            if(prev_free == nullptr)
                                this->free_list = next_free;
                        }
                        else //big node -> split
                        {
                            MemoryPoolFreeNode* split_node = reinterpret_cast<MemoryPoolFreeNode*>(border_node_address);

                            //place main node header
                            MemoryPoolAllocatedNode* main_node = new(base_address) MemoryPoolAllocatedNode{.header = {.block_special = SetBlockSpecialField(data_address - base_address, false),
                                                                                                                      .global_list_prev = prev_ablk,
                                                                                                                      .global_list_next = &split_node->header,
                                                                                                                      .size = static_cast<DeviceSize>((border_node_address - base_address))}};

                            //place main node prefix
                            new(data_address - sizeof(MemoryPoolAllocatedNodeDataPrefixHeader))
                                MemoryPoolAllocatedNodeDataPrefixHeader{.header_offset = static_cast<DeviceSize>((data_address - base_address))};

                            //place split node header
                            new(border_node_address) MemoryPoolFreeNode{
                                .header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = &main_node->header, .global_list_next = next_ablk, .size = remain_node_size},
                                .free_list_prev = prev_free,
                                .free_list_next = next_free};

                            if(prev_free != nullptr)
                            {
                                prev_free->free_list_next = split_node;
                                prev_free->header.global_list_next = &main_node->header;
                            }

                            if(next_free != nullptr)
                            {
                                next_free->free_list_prev = split_node;
                                next_free->header.global_list_prev = &split_node->header;
                            }

                            if(prev_ablk != nullptr)
                                prev_ablk->global_list_next = &main_node->header;

                            if(next_ablk != nullptr)
                                next_ablk->global_list_prev = &split_node->header;

                            if(prev_free == nullptr)
                                this->free_list = split_node;
                        }

                        return data_address;
                    }
                }
            }
        }

        return nullptr;
    }

    Void MemoryPool::Deallocate(Void* ptr) noexcept
    {
        MemoryPoolAllocatedNodeDataPrefixHeader* prefix = reinterpret_cast<MemoryPoolAllocatedNodeDataPrefixHeader*>(static_cast<UInt8*>(ptr) - sizeof(MemoryPoolAllocatedNodeDataPrefixHeader));
        MemoryPoolAllocatedNode* node = reinterpret_cast<MemoryPoolAllocatedNode*>(static_cast<UInt8*>(ptr) - prefix->header_offset);

        assert(!GetBlockSpecialFieldIsFree(node->header.block_special));

        MemoryPoolNodeHeader* prev_free_header = node->header.global_list_prev;
        MemoryPoolNodeHeader* next_free_header = node->header.global_list_next;

#pragma message("Optimize for small count of free nodes")
        if(this->free_list != nullptr)
        {
            prev_free_header = nullptr;
            next_free_header = nullptr;
        }
        else
        {
            while(prev_free_header != nullptr)
            {
                if(GetBlockSpecialFieldIsFree(prev_free_header->block_special))
                    break;

                prev_free_header = prev_free_header->global_list_prev;
            }

            while(next_free_header != nullptr)
            {
                if(GetBlockSpecialFieldIsFree(next_free_header->block_special))
                    break;

                next_free_header = next_free_header->global_list_next;
            }
        }

        MemoryPoolFreeNode* prev_free = reinterpret_cast<MemoryPoolFreeNode*>(prev_free_header);
        MemoryPoolFreeNode* next_free = reinterpret_cast<MemoryPoolFreeNode*>(next_free_header);

        if((prev_free != nullptr && node->header.global_list_prev == prev_free_header) && (next_free != nullptr && node->header.global_list_next == next_free_header))
        {
            //ablk free curr free ablk -> merge all free

            MemoryPoolNodeHeader* next_ablk = next_free->header.global_list_next;
            MemoryPoolFreeNode* next_list_free = next_free->free_list_next;

            //we will use prev_free as main block
            MemoryPoolFreeNode* main_node = prev_free;
            main_node->header.size += node->header.size + next_free->header.size;
            main_node->free_list_next = next_list_free;
            main_node->header.global_list_next = next_ablk;

            if(next_list_free != nullptr)
                next_list_free->free_list_prev = main_node;

            if(next_ablk != nullptr)
                next_ablk->global_list_prev = &main_node->header;
        }
        else if(prev_free != nullptr && node->header.global_list_prev == prev_free_header)
        {
            //ablk free curr ablk -> merge 2
            MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

            //we will use prev_free as main block
            MemoryPoolFreeNode* main_node = prev_free;
            main_node->header.size += node->header.size;
            main_node->header.global_list_next = next_ablk;

            if(next_ablk != nullptr)
                next_ablk->global_list_prev = &main_node->header;
        }
        else if(next_free != nullptr && node->header.global_list_next == next_free_header)
        {
            //ablk curr free ablk -> merge 2
            MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
            MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

            //we will create new blk over curr and next_free memory

            MemoryPoolFreeNode* main_node = new(node) MemoryPoolFreeNode{
                .header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->header.size + next_free->header.size},
                .free_list_prev = next_free->free_list_prev,
                .free_list_next = next_free->free_list_next};

            if(main_node->free_list_prev == nullptr)
                this->free_list = main_node;

            if(prev_ablk != nullptr)
                prev_ablk->global_list_next = &main_node->header;

            if(next_ablk != nullptr)
                next_ablk->global_list_prev = &main_node->header;
        }
        else
        {
            if(prev_free != nullptr && next_free != nullptr)
            {
                //connect prev, curr and next

                //free, ablk, curr, ablk, free

                MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
                MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

                MemoryPoolFreeNode* main_node =
                    new(node) MemoryPoolFreeNode{.header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->header.size},
                                                 .free_list_prev = prev_free,
                                                 .free_list_next = next_free};

                prev_free->free_list_next = main_node;
                next_free->free_list_prev = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = &main_node->header;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = &main_node->header;
            }
            else if(prev_free != nullptr)
            {
                //connect prev and curr

                //free, ablk, curr, ablk

                MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
                MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

                MemoryPoolFreeNode* main_node =
                    new(node) MemoryPoolFreeNode{.header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->header.size},
                                                 .free_list_prev = prev_free,
                                                 .free_list_next = nullptr};

                prev_free->free_list_next = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = &main_node->header;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = &main_node->header;
            }
            else if(next_free != nullptr)
            {
                //connect curr and next

                //ablk, curr, ablk, free

                MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
                MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

                MemoryPoolFreeNode* main_node =
                    new(node) MemoryPoolFreeNode{.header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->header.size},
                                                 .free_list_prev = nullptr,
                                                 .free_list_next = next_free};

                next_free->free_list_prev = main_node;

                this->free_list = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = &main_node->header;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = &main_node->header;
            }
            else
            {
                //make curr as single free node

                //ablk, curr, ablk
                assert(this->free_list == nullptr);

                MemoryPoolNodeHeader* prev_ablk = node->header.global_list_prev;
                MemoryPoolNodeHeader* next_ablk = node->header.global_list_next;

                MemoryPoolFreeNode* main_node =
                    new(node) MemoryPoolFreeNode{.header = {.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->header.size},
                                                 .free_list_prev = nullptr,
                                                 .free_list_next = nullptr};

                this->free_list = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = &main_node->header;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = &main_node->header;
            }
        }
    }

    DeviceSize MemoryPool::GetSize() const noexcept
    {
        return this->memory.GetSize();
    }
};