#pragma once

#include <concepts>
#include <list>
#include <optional>
#include <stdexcept>

namespace Core
{
#error Split begin and after iterators in free block! In for loop -> after always be the next of before!!!
    template<std::unsigned_integral I>
    struct BlockRequirement
    {
        I alignment;
        I size;
    };

    template<std::unsigned_integral I>
    struct Block
    {
        I offset;
        I size;
    };

    template<std::unsigned_integral I>
    I RoundUpToAlignment(I size, I alignment) noexcept
    {
        I rem = size % alignment;
        if(rem == 0)
            return size;

        return size + (alignment - rem);
    }

    template<std::unsigned_integral I>
    using FreeBlockListContainer = std::list<Block<I>>;

    template<typename S, typename I>
    concept FreeBlockListNodeStorage = requires(S& storage,
                                                FreeBlockListContainer<I>& list,
                                                FreeBlockListContainer<I>::iterator it) {
        { storage.Release(list, it) } noexcept;
        { storage.Release(list) } noexcept;
        { storage.Acquire() } -> std::same_as<typename FreeBlockListContainer<I>::iterator>;

        { storage.GetList() } noexcept -> std::same_as<FreeBlockListContainer<I>&>;
    };

    template<std::unsigned_integral I>
    class DefaultFreeBlockListNodeStorage
    {
    public:
        using Container = FreeBlockListContainer<I>;

        DefaultFreeBlockListNodeStorage() = default;
        ~DefaultFreeBlockListNodeStorage() = default;
        DefaultFreeBlockListNodeStorage(const DefaultFreeBlockListNodeStorage&) = default;
        DefaultFreeBlockListNodeStorage(DefaultFreeBlockListNodeStorage&&) = default;
        DefaultFreeBlockListNodeStorage&
        operator=(const DefaultFreeBlockListNodeStorage&) = default;
        DefaultFreeBlockListNodeStorage& operator=(DefaultFreeBlockListNodeStorage&&) = default;

        void Release(Container& list, Container::iterator it) noexcept
        {
            list.erase(it);
        }

        void Release(Container& list) noexcept
        {
            list.clear();
        }

        typename FreeBlockListContainer<I>::iterator Acquire()
        {
            data.push_back({});
            return data.begin();
        }

        Container& GetList() noexcept
        {
            return data;
        }
    private:
        Container data;
    };

    /*
    Resource alignment = COMMON_ALIGNMENT(mem_req.alignment, granularity)
    images -> OK
    buffers -> let's assume that mem_req.alignment also introduce alignment for max aligned data type in GLSL. or use outer offset...
    */
    template<std::unsigned_integral I, FreeBlockListNodeStorage<I> S>
    class FreeBlockList
    {
    public:
        using Block = Block<I>;
        using BlockRequirement = BlockRequirement<I>;
        using Container = FreeBlockListContainer<I>;

        FreeBlockList(/*I _offset, */ I _size, S&& _storage = {})
            : //offset(_offset),
              size(_size),
              storage(std::move(_storage))
        {
            data.splice(data.begin(), storage.GetList(), storage.Acquire());

            data.back() = Block{.offset = /*offset*/ 0, .size = _size};
        }

        ~FreeBlockList()
        {
            storage.Release(data);
        }

        FreeBlockList(const FreeBlockList&) = default;
        FreeBlockList(FreeBlockList&&) = default;
        FreeBlockList& operator=(const FreeBlockList&) = default;
        FreeBlockList& operator=(FreeBlockList&&) = default;

        //#error MAYBE FIRSTLY SERACH FOR NON-TWO SPLIT BLOCK AND THEN IF THERE ARE NO BLOCKS FOR NON-TWO SPLIT USE TWO-SPLIT BLOCK??? -> best two-split match where the remaining sum of two-splitted blocks is minimal
        //1. search for ideal blocks that will occupy the whole block without corrected offset
        //2. search for blocks that will occupy part of of block but without corrected offset
        //3. search for blocks that will ocuupy block till the end but with corrected offset
        //4. search for two-split blocks
        std::optional<Block> Acquire(const BlockRequirement& req)
        {
            if(!CanFit(req))
                return std::nullopt;

            //data for fragmentation avoidance
            enum class FragmentationAvoidanceBlockType
            {
                None, //initial
                NotCorrectedBlockPart, //remaining tail
                CorrectedBlockPartTail, //remaining head
                TwoSplit //remaining head and tail
            };

            FragmentationAvoidanceBlockType avoidance_type = FragmentationAvoidanceBlockType::None;
            decltype(data.begin()) best_it = data.end(); //avoidance best free block
            I best_remaining_sum = 0; //sum of remaining blocks sizes
            std::optional<Block> out_blk;
            Block best_first_blk; //first remaining block
            Block best_second_blk; //second remaining block

            for(auto it = data.begin(); it != data.end(); it++)
            {
                if(it->size < req.size)
                    continue;

                I corrected_offset = RoundUpToAlignment(it->offset, req.alignment);
                I diff = corrected_offset - it->offset;

                if(diff > it->size)
                    continue;

                if(it->size - diff < req.size)
                    continue;

                if(corrected_offset == it->offset) //acquire from begin of block
                {
                    if(req.size == it->size) //release the whole block
                    {
                        storage.Release(data, it);
                        return Block{.offset = corrected_offset, // - offset,
                                     .size = req.size};
                    }
                    else //do not release block
                    {
                        I new_remaining_sum = it->size - req.size;
                        if(avoidance_type == FragmentationAvoidanceBlockType::NotCorrectedBlockPart)
                        {
                            if(!(new_remaining_sum < best_remaining_sum))
                                continue;
                        }

                        avoidance_type = FragmentationAvoidanceBlockType::NotCorrectedBlockPart;
                        best_remaining_sum = new_remaining_sum;
                        best_it = it;
                        best_first_blk =
                            Block{.offset = corrected_offset + req.size, .size = new_remaining_sum};
                        out_blk = Block{.offset = corrected_offset, // - offset,
                                        .size = req.size};
                    }
                }
                else //acquire from middle of block
                {
                    if(it->size - diff == req.size) //do not release block -> only leave head
                    {
                        I new_remaining_sum = diff;
                        if(avoidance_type ==
                           FragmentationAvoidanceBlockType::CorrectedBlockPartTail)
                            continue;

                        if(avoidance_type == FragmentationAvoidanceBlockType::NotCorrectedBlockPart)
                        {
                            if(!(new_remaining_sum < best_remaining_sum))
                                continue;
                        }

                        avoidance_type = FragmentationAvoidanceBlockType::CorrectedBlockPartTail;
                        best_remaining_sum = new_remaining_sum;
                        best_it = it;
                        best_first_blk = Block{.offset = it->offset, .size = diff};
                        out_blk = Block{.offset = corrected_offset, // - offset,
                                        .size = req.size};
                    }
                    else //split into two blocks with acquisition from middle
                    {
                        I new_remaining_sum =
                            diff + (it->offset + it->size) - (corrected_offset + req.size);

                        if(avoidance_type ==
                               FragmentationAvoidanceBlockType::NotCorrectedBlockPart ||
                           avoidance_type ==
                               FragmentationAvoidanceBlockType::CorrectedBlockPartTail)
                            continue;

                        if(avoidance_type == FragmentationAvoidanceBlockType::TwoSplit)
                        {
                            if(!(new_remaining_sum < best_remaining_sum))
                                continue;
                        }

                        avoidance_type = FragmentationAvoidanceBlockType::TwoSplit;
                        best_remaining_sum = new_remaining_sum;
                        best_it = it;
                        best_first_blk = Block{.offset = it->offset, .size = diff};
                        best_second_blk =
                            Block{.offset = corrected_offset + req.size,
                                  .size = (it->offset + it->size) - (corrected_offset + req.size)};
                        out_blk = Block{.offset = corrected_offset, // - offset,
                                        .size = req.size};
                    }
                }
            }

            /*Block blk = {.offset = corrected_offset, // - offset,
                             .size = req.size};
                if(corrected_offset == it->offset) //acquire from begin of block
                {
                    if(req.size == it->size) //release the whole block
                    {
                        storage.Release(data, it);
                    }
                    else //do not release block
                    {
                        *it = Block{.offset = corrected_offset + req.size,
                                    .size = it->size - req.size};
                    }
                }
                else //acquire from middle of block
                {
                    if(it->size - diff == req.size) //do not release block -> only leave head
                    {
                        *it = Block{.offset = it->offset, .size = diff};
                    }
                    else //split into two blocks with acquisition from middle
                    {
                        auto second_block_it = storage.Acquire();
                        data.splice(std::next(it), storage.GetList(), second_block_it);

                        Block initial_blk = *it;

                        *it = Block{.offset = it->offset, .size = diff};
                        *second_block_it = Block{.offset = corrected_offset + req.size,
                                                 .size = (initial_blk.offset + initial_blk.size) -
                                                         (corrected_offset + req.size)};
                    }
                }

                return blk;*/

            switch(avoidance_type)
            {
                case FragmentationAvoidanceBlockType::NotCorrectedBlockPart:
                    *best_it = best_first_blk;
                    break;
                case FragmentationAvoidanceBlockType::CorrectedBlockPartTail:
                    *best_it = best_first_blk;
                    break;
                case FragmentationAvoidanceBlockType::TwoSplit:
                {
                    auto second_block_it = storage.Acquire();
                    data.splice(std::next(best_it), storage.GetList(), second_block_it);

                    *best_it = best_first_blk;
                    *second_block_it = best_second_blk;
                }
                break;
            }

            return out_blk;
        }

        void Release(const Block& blk)
        {
            //1. Empty list -> insert
            //2. Find before but no after -> merge with before or insert
            //3. Find after but no before -> merge with after or insert
            //4. Find before and after -> merge with one of them or insert

            if(blk.offset + blk.size > size)
                throw std::runtime_error("Block offset is out of list bounds");

            if(data.empty())
            {
                auto it = storage.Acquire();
                data.splice(data.end(), storage.GetList(), it);
                *it = Block{.offset = /*offset + */ blk.offset, .size = blk.size};
            }
            else
            {
                auto before_it = data.end();
                auto after_it = data.end();

                for(auto it = data.begin(); it != data.end(); it++)
                {
                    if(!((blk.offset + blk.size <= it->offset /* - offset*/) ||
                         (it->offset /*- offset*/ + it->size <= blk.offset)))
                        throw std::runtime_error("Block offset is out of list bounds");

                    if(it->offset /*- offset*/ + it->size <= blk.offset)
                        before_it = it;
                    else if(blk.offset + blk.size <= it->offset /*- offset*/)
                    {
                        after_it = it;
                        break;
                    }
                }

                if(before_it != data.end() && after_it == data.end())
                {
                    if(before_it->offset /*- offset*/ + before_it->size == blk.offset) //merge
                    {
                        before_it->size += blk.size;
                    }
                    else //insert
                    {
                        auto second_block_it = storage.Acquire();
                        data.splice(std::next(before_it), storage.GetList(), second_block_it);
                        *second_block_it =
                            Block{.offset = /*offset +*/ blk.offset, .size = blk.size};
                    }
                }
                else if(before_it == data.end() && after_it != data.end())
                {
                    if(blk.offset + blk.size == after_it->offset /*- offset*/) //merge
                    {
                        *after_it = Block{.offset = blk.offset /*+ offset*/,
                                          .size = blk.size + after_it->size};
                    }
                    else //insert
                    {
                        auto second_block_it = storage.Acquire();
                        data.splice(after_it, storage.GetList(), second_block_it);
                        *second_block_it =
                            Block{.offset = /*offset +*/ blk.offset, .size = blk.size};
                    }
                }
                else //before_it != data.end() && after_it != data.end()
                {
                    bool can_merge_with_before =
                        (before_it->offset /*- offset*/ + before_it->size) == blk.offset;

                    bool can_merge_with_after =
                        (blk.offset + blk.size) == (after_it->offset /*- offset*/);

                    if(!can_merge_with_before && !can_merge_with_after) //insert between
                    {
                        auto it = storage.Acquire();
                        data.splice(after_it, storage.GetList(), it);
                        *it = Block{.offset = /*offset +*/ blk.offset, .size = blk.size};
                    }
                    else if(!can_merge_with_after) //merge with before
                    {
                        before_it->size += blk.size;
                    }
                    else if(!can_merge_with_before) //merge with after
                    {
                        *after_it = Block{.offset = blk.offset /*+ offset*/,
                                          .size = blk.size + after_it->size};
                    }
                    else //merge all three blocks
                    {
                        *before_it = Block{.offset = before_it->offset,
                                           .size = before_it->size + blk.size + after_it->size};
                        storage.Release(data, after_it);
                    }
                }
            }
        }

        I GetSize() const noexcept
        {
            return size;
        }

        bool CanFit(const BlockRequirement& req) const noexcept
        {
            /*I corrected_offset = RoundUpToAlignment(offset, req.alignment);
            I diff = corrected_offset - offset;

            if(diff > size)
                return false;

            return (size - diff) >= req.size;*/

            return size >= req.size;
        }

        /*I ToAbsoluteOffset(I value) const noexcept
        {
            return value + offset;
        }*/
    private:
        Container data;
        //I offset;
        I size;
        S storage;
    };
};