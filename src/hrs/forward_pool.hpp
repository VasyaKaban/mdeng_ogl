#pragma once

#include <vector>
#include <list>
#include <cassert>
#include "non_creatable.hpp"

namespace hrs
{
    template<typename T>
    class forward_pool : hrs::non_copyable
    {
        struct block
        {
            using container_t = std::vector<T>;
            using entry_t = container_t::iterator;

            container_t data;
            entry_t next_free_entry; //end -> full;
            entry_t next_in_use_entry; //end -> empty

            block(std::size_t size)
                : data(size),
                  next_free_entry(data.begin()),
                  next_in_use_entry(data.end())
            {}

            ~block() = default;
            block(block&&) = default;
            block& operator=(block&&) = default;

            T* pick() noexcept
            {
                return &*next_in_use_entry;
            }

            void pop() noexcept
            {
                next_in_use_entry++;
            }

            bool has_items() const noexcept
            {
                return !(next_in_use_entry == next_free_entry || next_free_entry == data.end());
            }

            T& push() noexcept
            {
                auto it = next_free_entry;
                next_free_entry++;
                if(next_in_use_entry == data.end())
                    next_in_use_entry = it;

                return *it;
            }

            std::size_t get_available_space() const noexcept
            {
                if(next_in_use_entry == data.end()) //empty
                    return data.size();
                else
                    return next_free_entry - next_in_use_entry;
            }

            void reset() noexcept
            {
                next_free_entry = data.begin();
                next_in_use_entry = data.end();
            }
        };
    public:
        forward_pool(std::size_t _block_size, std::size_t blocks_reserve)
            : block_size(_block_size)
        {
            if(blocks_reserve != 0)
            {
                for(std::size_t i = 0; i < blocks_reserve; i++)
                    free_blocks.push_back(block(_block_size));
            }
        }

        ~forward_pool() = default;

        forward_pool(forward_pool&& pool) noexcept
            : acquired_blocks(std::move(pool.acquired_blocks)),
              free_blocks(std::move(pool.free_blocks)),
              block_size(pool.block_size)
        {}

        forward_pool& operator=(forward_pool&& pool) noexcept
        {
            acquired_blocks = std::move(pool.acquired_blocks);
            free_blocks = std::move(pool.free_blocks);
            block_size = pool.block_size;

            return *this;
        }

        T* pick() noexcept
        {
            auto current_in_use_block = acquired_blocks.begin();
            if(current_in_use_block == acquired_blocks.end())
                return nullptr;

            return current_in_use_block->pick();
        }

        void pop() noexcept
        {
            if(pick() == nullptr)
                return;

            auto current_in_use_block = acquired_blocks.begin();
            current_in_use_block->pop();
            if(!current_in_use_block->has_items()) //adjust
                free_blocks.splice(free_blocks.end(), acquired_blocks, current_in_use_block);
        }

        T& acquire()
        {
            if(acquired_blocks.empty())
            {
                if(free_blocks.empty())
                    free_blocks.push_back(block(block_size));

                auto it = free_blocks.begin();
                it->reset();
                acquired_blocks.splice(acquired_blocks.end(), free_blocks, it);
            }

            auto to_insert_block = std::prev(acquired_blocks.end());
            return to_insert_block->push();
        }

        void reserve_next(std::size_t size)
        {
            if(size == 0)
                return;

            if(!acquired_blocks.empty())
            {
                std::size_t in_avail = acquired_blocks.back().get_available_space();
                if(in_avail >= size)
                    return;

                size -= in_avail;
            }

            while(!free_blocks.empty())
            {
                auto it = free_blocks.begin();
                it->reset();
                acquired_blocks.splice(acquired_blocks.end(), free_blocks, it);

                if(block_size >= size)
                    return;

                size -= block_size;
            }

            while(true)
            {
                acquired_blocks.push_back(block(block_size));

                if(block_size >= size)
                    return;

                size -= block_size;
            }
        }
    private:
        using container_t = std::list<block>;

        container_t acquired_blocks;
        container_t free_blocks;

        std::size_t block_size;
    };
};