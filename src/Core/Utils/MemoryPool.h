#pragma once

#include "../API.h"
#include "Binary.hpp"
#include "Types.hpp"
#include "Memory.h"
#include "Span.hpp"
#include "Utility.hpp"

namespace Core
{
    /*
    void* ptr = Allocate(...)
    MemoryPoolAllocatedNode* node = (MemoryPoolAllocatedNode*)(ptr - *(DeviceSize*)(ptr - sizeof(DeviceSize)))

                              +-------------------------------------+
                              |                                     |
                              |                                     |
###################           |        ###################          |         ###################                    ###################                    ###################
#                 #           |        #                 #          |         #                 #                    #                 #                    #                 #
#      free       #<----------+        #      nfree      #          +-------->#      free       #<------------------>#      free       #<------------------>#      free       #
#                 #                    #                 #                    #                 #                    #                 #                    #                 #
##############@####                    ####@#########@####                    ####@#########@####                    ####@#########@####                    ####@#########@####
              ^                            ^         ^                            ^         ^                            ^         ^                            ^         ^                            
              |                            |         |                            |         |                            |         |                            |         |                            
               \                          /           \                          /           \                          /           \                          /           \                          
                \                        /             \                        /             \                        /             \                        /             \                        
                 \______           _____/               \______           _____/               \______           _____/               \______           _____/               \______         
                        |         |                            |         |                            |         |                            |         |                            |         
                        v         v                            v         v                            v         v                            v         v                            v        
                    ####@#########@####                    ####@#########@####                    ####@#########@####                    ####@#########@####                    ####@##############
                    #                 #                    #                 #                    #                 #                    #                 #                    #                 #
                    #      nfree      #                    #      nfree      #                    #      nfree      #                    #      nfree      #                    #      nfree      #
                    #                 #                    #                 #                    #                 #                    #                 #                    #                 #
                    ###################                    ###################                    ###################                    ###################                    ###################
    */

    constexpr static DeviceSize PoolNodeIsFreeFlagMask = 1ull << 63; //100000...0000
    constexpr static DeviceSize PoolNodeDataOffsetsMask = ~PoolNodeIsFreeFlagMask; //011111...11111

    struct MemoryPoolNodeHeader
    {
        DeviceSize block_special; //special field for 'is_free' flag and 'data_offset' bits
        MemoryPoolNodeHeader* global_list_prev; //prev node in the global list
        MemoryPoolNodeHeader* global_list_next; //next node in the global list
        DeviceSize size; //size of the whole node(with headers)
    };

    struct MemoryPoolFreeNode
    {
        MemoryPoolNodeHeader header; //is_free = true
        MemoryPoolFreeNode* free_list_prev; //prev free node in the list
        MemoryPoolFreeNode* free_list_next; //next free node in the list
    };

    struct MemoryPoolAllocatedNode
    {
        MemoryPoolNodeHeader header; //is_free = false, data_offset=...
        //...padding
        //AllocatedNodeDataPrefixHeader prefix;
        //...data with possible unused padding in tail
    };

    struct MemoryPoolAllocatedNodeDataPrefixHeader
    {
        DeviceSize header_offset; //how many bytes we should subtract from data pointer to get address of it's header
    };

    static_assert(StandardLayout<MemoryPoolFreeNode> && StandardLayout<MemoryPoolAllocatedNode>); //check that 'header' field has zero offset

    static_assert(sizeof(MemoryPoolFreeNode) >= (sizeof(MemoryPoolAllocatedNode) + sizeof(MemoryPoolAllocatedNodeDataPrefixHeader)));

    //We do not implement Allocator1 interface due to the different purposes.
    //If you need to use MemoryPool as Allocator then create wrapper over this class
    class CORE_API MemoryPool
    {
    public:
        //specify minimal alignment and size power of the allocatable buffer
        constexpr static DeviceSize BaseHeaderGranularity = GetPowerOf2(Max(sizeof(MemoryPoolFreeNode), sizeof(MemoryPoolAllocatedNode) + sizeof(MemoryPoolAllocatedNodeDataPrefixHeader)));

        MemoryPool(Allocator allocator, DeviceSize size);
        ~MemoryPool();
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&& pool) noexcept;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool& operator=(MemoryPool&& pool) noexcept;

        Void* Allocate(const MemoryRequirements& req) noexcept;
        Void Deallocate(Void* ptr) noexcept;

        DeviceSize GetSize() const noexcept;
    private:
        MemoryPoolFreeNode* free_list;
        Allocator allocator;
        Span<UInt8> memory;
    };
};