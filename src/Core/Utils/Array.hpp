#pragma once

#include <ranges>
#include <cassert>
#include "Memory.h"
#include "Instantiation.hpp"

namespace Core
{
    template<typename T>
    class Array
    {
        static_assert(std::same_as<std::remove_cvref_t<T>, T>);
    public:
        using Iterator = T*;
        using ConstIterator = const T*;

        Array(Allocator allocator = GetGlobalAllocator()) noexcept
            : memory(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {}

        Array(size_t reserve, Allocator allocator = GetGlobalAllocator()) noexcept
            : Array(allocator)
        {
            if(reserve != 0)
            {
                this->memory = AllocateMemory(reserve);
                this->capacity = reserve;
            }
        }

        ~Array()
        {
            DestroyObjectsAndDeallocateMemory(this->memory, this->size, this->allocator);
        }

        Array(const Array& array)
        requires std::copy_constructible<T>
            : Array(array.allocator)
        {
            T* new_memory = AllocateMemory(array.size);

            size_t index = 0;
            try
            {
                for(; index < array.size; index++)
                    new(new_memory + index) T(array.memory[index]);
            }
            catch(...)
            {
                DestroyObjectsAndDeallocateMemory(new_memory, index, this->allocator);

                throw;
            }

            this->memory = new_memory;
            this->size = array.size;
            this->capacity = this->size;
        }

        Array(Array&& array) noexcept
            : memory(std::exchange(array.memory, nullptr)),
              size(std::exchange(array.size, 0)),
              capacity(std::exchange(array.capacity, 0)),
              allocator(array.allocator)
        {}

        Array& operator=(const Array& array)
        {
            this->Clear();

            T* new_memory = AllocateMemory(array.size);

            size_t index = 0;
            try
            {
                for(; index < array.size; index++)
                    new(new_memory + index) T(array.memory[index]);
            }
            catch(...)
            {
                DestroyObjectsAndDeallocateMemory(new_memory, index, this->allocator);

                throw;
            }

            this->memory = new_memory;
            this->size = array.size;
            this->capacity = this->size;

            return *this;
        }

        Array& operator=(Array&& array) noexcept
        {
            this->Clear();

            memory = std::exchange(array.memory, nullptr);
            size = std::exchange(array.size, 0);
            capacity = std::exchange(array.capacity, 0);
            allocator = array.allocator;

            return *this;
        }

        template<std::ranges::sized_range R>
        requires std::constructible_from<T, std::ranges::range_value_t<std::remove_cvref_t<R>>>
        Array(R&& values, Allocator allocator = GetGlobalAllocator())
            : Array(allocator)
        {
            size_t values_size = std::ranges::size(std::forward<R>(values));
            if(values_size != 0)
            {
                T* new_memory = AllocateMemory(values_size);

                size_t index = 0;
                try
                {
                    for(auto&& value: std::forward<R>(values))
                    {
                        new(new_memory + index) T(std::forward<decltype(value)>(value));
                        index++;
                    }
                }
                catch(...)
                {
                    DestroyObjectsAndDeallocateMemory(new_memory, index, this->allocator);
                    throw;
                }

                this->memory = new_memory;
                this->size = values_size;
                this->capacity = this->size;
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //insert back -> push back
        //insert -> move + possible alloc
        //if enough capacity -> move or copy and insert new object
        //otherwise try to grow and perform the same op from above
        //else allocate new memory, copy old objects and insert new and free previous memory

        template<typename U>
        requires std::constructible_from<T, U>
        void Insert(ConstIterator before_it, U&& value);
        ////////////////////////////////////////////////////////////////////////

        template<typename U>
        requires std::constructible_from<T, U> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
        void PushToEnd(U&& value)
        {
            if(this->capacity > this->size) //push back if there is enough capacity
            {
                new(this->memory + this->size) T(std::forward<U>(value));
            }
            else if(!this->memory) //if there is no memory then allocate and create object
            {
                this->memory = AllocateMemory(1);
                try
                {
                    new(this->memory) T(std::forward<U>(value));
                    this->capacity++;
                }
                catch(...)
                {
                    this->allocator.Deallocate(this->memory);
                    this->memory = nullptr;
                    throw;
                }
            }
            else
            {
                if(this->allocator.Grow(this->memory,
                                        sizeof(T) * (this->capacity + 1))) //firstly try to grow memory
                {
                    this->capacity++;
                    new(this->memory) T(std::forward<U>(value));
                }
                else //otherwise allocate new memory, copy or move objects from old memory into new one and deallocate old memory
                {
                    //allocate new memory
                    size_t new_capacity = this->capacity * 2;
                    T* new_memory = AllocateMemory(new_capacity);

                    //forward new object to the 'end' of the new memory
                    try
                    {
                        new(new_memory + this->size) T(std::forward<U>(value));
                    }
                    catch(...)
                    {
                        this->allocator.Deallocate(new_memory);
                        throw;
                    }

                    //copy or move objects from old memory
                    for(size_t i = 0; i < this->size; i++)
                    {
                        if constexpr(std::is_nothrow_move_constructible_v<T>)
                            new(new_memory + i) T(std::move(this->memory[i]));
                        else
                            new(new_memory + i) T(this->memory[i]);
                    }

                    //we should destroy previous objects from old memory before deallocation
                    DestroyObjectsAndDeallocateMemory(this->memory, this->size, this->allocator);

                    this->memory = new_memory;
                    this->capacity = new_capacity;
                }
            }

            this->size++;
        }

        void Erase(ConstIterator it) noexcept
        requires std::is_nothrow_move_assignable_v<T> || std::is_nothrow_copy_assignable_v<T>
        {
            assert(this->size != 0);
            assert(it >= this->memory && it < (this->memory + this->size));

            size_t start_index = (it - this->memory) + 1;
            for(size_t i = start_index; i < this->size; i++)
            {
                T& prev = *(this->memory + (i - 1));
                T& current = *(this->memory + i);

                if constexpr(std::is_nothrow_move_assignable_v<T>)
                    prev = std::move(current);
                else
                    prev = current;
            }

            (this->memory + (this->size - 1))->~T(); //erase last

            this->size--;
        }

        void EraseLast() noexcept
        {
            assert(this->size != 0);

            (this->memory + (this->size - 1))->~T();

            this->size--;
        }

        void Clear() noexcept
        {
            DestroyObjects(this->memory, this->size);

            this->size = 0;
        }

        void Reset() noexcept
        {
            if(this->memory)
            {
                DestroyObjectsAndDeallocateMemory(this->memory, this->size, allocator);

                this->memory = nullptr;
                this->size = 0;
                this->capacity = 0;
            }
        }

        bool FlushUnusedReserve() noexcept
        {
            bool res = false;
            if(!this->memory)
                res = true;
            else if(this->size == 0 && this->capacity != 0)
            {
                allocator.Deallocate(memory);

                this->memory = nullptr;
                this->capacity = 0;

                res = true;
            }
            else if(this->capacity > this->size)
            {
                res = allocator.Trim(this->memory, this->size * sizeof(T));
                if(res)
                    this->capacity = this->size;
            }

            return res;
        }

        bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        size_t GetSize() const noexcept
        {
            return this->size;
        }

        size_t GetCapacity() const noexcept
        {
            return this->capacity;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        T& GetFirst() noexcept
        {
            return *(this->memory);
        }

        const T& GetFirst() const noexcept
        {
            return *(this->memory);
        }

        T& GetLast() noexcept
        {
            return *(this->memory + (this->size - 1));
        }

        const T& GetLast() const noexcept
        {
            return *(this->memory + (this->size - 1));
        }

        Iterator Begin() noexcept
        {
            return this->memory;
        }

        ConstIterator Begin() const noexcept
        {
            return this->memory;
        }

        Iterator End() noexcept
        {
            return this->memory + this->size;
        }

        ConstIterator End() const noexcept
        {
            return this->memory + this->size;
        }
    private:
        T* AllocateMemory(size_t size)
        {
            return static_cast<T*>(allocator.Allocate(MemoryRequirements{.alignment = alignof(T), .size = size}));
        }

        static void DestroyObjects(T* memory, size_t size) noexcept
        {
            for(size_t i = 0; i < size; i++)
                (memory + i)->~T();
        }

        static void DestroyObjectsAndDeallocateMemory(T* memory, size_t size, Allocator& allocator) noexcept
        {
            DestroyObjects(memory, size);

            allocator.Deallocate(memory);
        }
    private:
        T* memory;
        size_t size;
        size_t capacity;
        Allocator allocator;
    };

    template<std::ranges::sized_range R>
    Array(R&& values) -> Array<std::remove_cvref_t<std::ranges::range_value_t<std::remove_cvref_t<R>>>>;

    //std compat
    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Array>
    auto begin(T&& array) noexcept
    {
        return std::forward<T>(array).Begin();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Array>
    auto end(T&& array) noexcept
    {
        return std::forward<T>(array).End();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Array>
    auto size(T&& array) noexcept
    {
        return std::forward<T>(array).GetSize();
    }
};