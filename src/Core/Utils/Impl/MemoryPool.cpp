#include "../MemoryPool.h"
#include "../Binary.hpp"

#include <cassert>

namespace Core
{
    constexpr static DeviceSize PoolNodeIsFreeFlagMask = 1ull << 63; //100000...0000
    constexpr static DeviceSize PoolNodeDataOffsetsMask = ~PoolNodeIsFreeFlagMask; //011111...11111

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

    struct PoolNodeBase
    {
        DeviceSize block_special;
        PoolNodeBase* global_list_prev; //prev node in the global list
        PoolNodeBase* global_list_next; //next node in the global list
        DeviceSize size; //size of the whole node(with headers)
    };

    struct MemoryPool::FreeNode : PoolNodeBase
    {
        //is_free = true
        FreeNode* free_list_prev; //prev free node in the list
        FreeNode* free_list_next; //next free node in the list
    };

    struct MemoryPool::AllocatedNode : PoolNodeBase
    {
        //is_free = false, data_offset=...
        //...padding
        //AllocatedNodeDataPrefixHeader prefix;
        //...data with possible unused padding in tail
    };

    struct AllocatedNodeDataPrefixHeader
    {
        DeviceSize header_offset; //how many bytes we should subtract from data pointer to get address of it's header
    };

    static_assert(MemoryPool::MemoryRequirementsMinAlignment <= MemoryPool::MemoryRequirementsMinSize);
    static_assert(sizeof(MemoryPool::FreeNode) <= MemoryPool::MemoryRequirementsMinAlignment);
    static_assert((sizeof(MemoryPool::AllocatedNode) + sizeof(AllocatedNodeDataPrefixHeader)) <= MemoryPool::MemoryRequirementsMinAlignment);
    static_assert(sizeof(MemoryPool::FreeNode) >= (sizeof(MemoryPool::AllocatedNode) + sizeof(AllocatedNodeDataPrefixHeader)));

    MemoryPool::MemoryPool(Allocator allocator, DeviceSize size)
        : free_list(nullptr),
          allocator(allocator)
    {
        if(MemoryPool::MemoryRequirementsMinSize > size)
            size = MemoryPool::MemoryRequirementsMinSize;
        else
        {
            if(!Align(size, MemoryPool::MemoryRequirementsMinSize)) //go to the lower bound
            {
                size = (size / MemoryPool::MemoryRequirementsMinSize) * MemoryPool::MemoryRequirementsMinSize;
            }
        }

        Void* ptr = this->allocator.Allocate(MemoryRequirements{.alignment = MemoryPool::MemoryRequirementsMinAlignment, .size = size});

        this->memory = Span(reinterpret_cast<UInt8*>(ptr), size);

#pragma message("SET UP FREE_LIST")
        new(this->memory.GetData()) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = nullptr, .global_list_next = nullptr, .size = size},
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
          memory(pool.memory)
    {}

    MemoryPool& MemoryPool::operator=(MemoryPool&& pool) noexcept
    {
        this->~MemoryPool();

        this->free_list = Exchange(pool.free_list, nullptr);
        this->allocator = pool.allocator;
        this->memory = pool.memory;

        return *this;
    }

    Void* MemoryPool::Allocate(const MemoryRequirements& req) noexcept
    {
#pragma message("WE SHOULD ALLOCATE BLOCKS WITH MIN SIZE MemoryPool::MemoryRequirementsMinSize because we mujst then deallocate data and create free node(FreeNode.size > AllocatedNode.size)!")
#pragma message("TODO")

        if(req.size == 0)
            return nullptr;

#pragma message("CHANGE ALIGNMENT CORRECTLY!")
        MemoryRequirements new_req = req;
        if(new_req.alignment < 8)
            new_req.alignment = 8;

        FreeNode* node = this->free_list;
        while(node != nullptr)
        {
#pragma message("SIZE OF FREE NODE MUST BE BIFGGER THAT ALLOCATED NODE + PREFIX!")

            //header, padding, prefix, data, padding
            UInt8* base_address = reinterpret_cast<UInt8*>(node);
            UInt8* data_address = base_address + sizeof(AllocatedNode) + sizeof(AllocatedNodeDataPrefixHeader);

            if(!AlignAddress(data_address, node->size - (data_address - base_address), new_req.alignment))
                node = node->free_list_next;
            else
            {
                DeviceSize remain_size = (base_address + node->size) - data_address;
                if(remain_size < new_req.size)
                {
                    node = node->free_list_next;
                }
                else
                {
                    FreeNode* prev_free = node->free_list_prev;
                    FreeNode* next_free = node->free_list_next;

                    PoolNodeBase* prev_ablk = node->global_list_prev;
                    PoolNodeBase* next_ablk = node->global_list_next;

                    //pad after data address to some multiple(8 or 64???)
                    //calculate after data of aligned address size
                    //if it a small node(< 64???) -> then allocate the whole node
                    //if it a big node(> 64???) -> split and reconnect
                }
            }
        }

        return nullptr;

        /*
        req.alignemnt = max(MinPoolAlign, req.alignemnt)

        for free_blk : free_list:
            if free_blk.size < size:
                continue;

            addr = FindAddress(free_blk, size, alignment)
            if OutOfBounds(addr):
                continue;

            if NotEnoughSpaceForHeaderAndPrefix(addr):
                AlignToTheNextMultiple(addr, alignment)
                if OutOfBounds(addr):
                    continue;

            ptrs = GetPointers(free_blk)
            PlaceData(addr)
            PlaceHeaderAndPrefix(addr, ptr)
            if EnoughSpaceForFreeBlock(addr):
                EmitFreeBlock(ptrs)

            ...
        */
    }

    Void MemoryPool::Deallocate(Void* ptr) noexcept
    {
        AllocatedNodeDataPrefixHeader* prefix = reinterpret_cast<AllocatedNodeDataPrefixHeader*>(static_cast<UInt8*>(ptr) - sizeof(AllocatedNodeDataPrefixHeader));
        AllocatedNode* node = reinterpret_cast<AllocatedNode*>(static_cast<UInt8*>(ptr) - prefix->header_offset);

        assert(!GetBlockSpecialFieldIsFree(node->block_special));

        PoolNodeBase* prev_free = node->global_list_prev;
        PoolNodeBase* next_free = node->global_list_next;

#pragma message("Optimize for small count of free nodes")
        if(this->free_list != nullptr)
        {
            prev_free = nullptr;
            next_free = nullptr;
        }
        else
        {
            while(prev_free != nullptr)
            {
                if(GetBlockSpecialFieldIsFree(prev_free->block_special))
                    break;

                prev_free = prev_free->global_list_prev;
            }

            while(next_free != nullptr)
            {
                if(GetBlockSpecialFieldIsFree(next_free->block_special))
                    break;

                next_free = next_free->global_list_next;
            }
        }

        if((prev_free != nullptr && node->global_list_prev == prev_free) && (next_free != nullptr && node->global_list_next == next_free))
        {
            //ablk free curr free ablk -> merge all free

            PoolNodeBase* next_ablk = prev_free->global_list_next;

            //we will use prev_free as main block
            FreeNode* main_node = static_cast<FreeNode*>(prev_free);
            main_node->size += node->size + static_cast<FreeNode*>(next_free)->size;
            main_node->free_list_next = static_cast<FreeNode*>(next_free)->free_list_next;
            main_node->global_list_next = next_ablk;
            if(next_ablk != nullptr)
                next_ablk->global_list_prev = main_node;
        }
        else if(prev_free != nullptr && node->global_list_prev == prev_free)
        {
            //ablk free curr ablk -> merge 2
            PoolNodeBase* next_blk = node->global_list_next;

            //we will use prev_free as main block
            FreeNode* main_node = static_cast<FreeNode*>(prev_free);
            main_node->size += node->size;
            main_node->global_list_next = next_blk;
            if(next_blk != nullptr)
                next_blk->global_list_prev = main_node;
        }
        else if(next_free != nullptr && node->global_list_next == next_free)
        {
            //ablk curr free ablk -> merge 2
            PoolNodeBase* prev_ablk = node->global_list_prev;
            PoolNodeBase* next_ablk = node->global_list_next;

            //we will create new blk over curr and next_free memory
            FreeNode* next_free_node = static_cast<FreeNode*>(next_free);

            FreeNode* main_node =
                new(node) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->size + next_free->size},
                                   .free_list_prev = next_free_node->free_list_prev,
                                   .free_list_next = next_free_node->free_list_next};

            if(main_node->free_list_prev == nullptr)
                this->free_list = main_node;

            if(prev_ablk != nullptr)
                prev_ablk->global_list_next = main_node;

            if(next_ablk != nullptr)
                next_ablk->global_list_prev = main_node;
        }
        else
        {
            if(prev_free != nullptr && next_free != nullptr)
            {
                //connect prev, curr and next

                //free, ablk, curr, ablk, free

                PoolNodeBase* prev_ablk = node->global_list_prev;
                PoolNodeBase* next_ablk = node->global_list_next;

                FreeNode* prev_free_node = static_cast<FreeNode*>(prev_free);
                FreeNode* next_free_node = static_cast<FreeNode*>(next_free);

                FreeNode* main_node = new(node) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->size},
                                                         .free_list_prev = prev_free_node,
                                                         .free_list_next = next_free_node};

                prev_free_node->free_list_next = main_node;
                next_free_node->free_list_prev = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = main_node;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = main_node;
            }
            else if(prev_free != nullptr)
            {
                //connect prev and curr

                //free, ablk, curr, ablk

                PoolNodeBase* prev_ablk = node->global_list_prev;
                PoolNodeBase* next_ablk = node->global_list_next;

                FreeNode* prev_free_node = static_cast<FreeNode*>(prev_free);

                FreeNode* main_node = new(node) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->size},
                                                         .free_list_prev = prev_free_node,
                                                         .free_list_next = nullptr};

                prev_free_node->free_list_next = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = main_node;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = main_node;
            }
            else if(next_free != nullptr)
            {
                //connect curr and next

                //ablk, curr, ablk, free

                PoolNodeBase* prev_ablk = node->global_list_prev;
                PoolNodeBase* next_ablk = node->global_list_next;

                FreeNode* next_free_node = static_cast<FreeNode*>(next_free);

                FreeNode* main_node = new(node) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->size},
                                                         .free_list_prev = nullptr,
                                                         .free_list_next = next_free_node};

                next_free_node->free_list_prev = main_node;

                this->free_list = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = main_node;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = main_node;
            }
            else
            {
                //make curr as single free node

                //ablk, curr, ablk
                assert(this->free_list == nullptr);

                PoolNodeBase* prev_ablk = node->global_list_prev;
                PoolNodeBase* next_ablk = node->global_list_next;

                FreeNode* main_node = new(node) FreeNode{PoolNodeBase{.block_special = SetBlockSpecialField(0, true), .global_list_prev = prev_ablk, .global_list_next = next_ablk, .size = node->size},
                                                         .free_list_prev = nullptr,
                                                         .free_list_next = nullptr};

                this->free_list = main_node;

                if(prev_ablk != nullptr)
                    prev_ablk->global_list_next = main_node;

                if(next_ablk != nullptr)
                    next_ablk->global_list_prev = main_node;
            }
        }
    }

    DeviceSize MemoryPool::GetSize() const noexcept
    {
        return this->memory.GetSize();
    }
};