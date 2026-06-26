#pragma once

#include "Memory.h"
#include "Instantiation.hpp"
#include <ranges>
#include <cassert>

namespace Core
{
    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, T>
    class Array
    {
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
            Reserve(reserve);
        }

        ~Array()
        {
            if(this->memory)
            {
                DestroyObjects();

                this->allocator.Deallocate(this->memory);
            }
        }

        Array(const Array& array)
        {
            Array tmp_array(array.size, array.allocator);

            for(size_t i = 0; i < array.size; i++)
                tmp_array.Push(array[i]);

            *this = std::move(tmp_array);
        }

        Array(Array&& array) noexcept
            : memory(std::exchange(array.memory, nullptr)),
              size(std::exchange(array.size, 0)),
              capacity(std::exchange(array.capacity, 0)),
              allocator(array.allocator)
        {}

        Array& operator=(const Array& array)
        {
            Array tmp_array(array.size, array.allocator);

            for(size_t i = 0; i < array.size; i++)
                tmp_array.Push(array[i]);

            *this = std::move(tmp_array);

            return *this;
        }

        Array& operator=(Array&& array) noexcept
        {
            this->~Array();

            this->memory = std::exchange(array.memory, nullptr);
            this->size = std::exchange(array.size, 0);
            this->capacity = std::exchange(array.capacity, 0);
            this->allocator = array.allocator;

            return *this;
        }

        template<std::ranges::sized_range R>
        Array(R&& values, Allocator allocator = GetGlobalAllocator())
        {
            Array tmp_array(std::ranges::size(std::forward<R>(values)), allocator);

            for(auto&& value: std::forward<R>(values))
                tmp_array.Push(std::forward<decltype(value)>(value));

            *this = std::move(tmp_array);
        }

        //on enough capacity -> move right and place
        //on grow -> move right and place
        //om alloc -> copy left, place, copy right
        void Insert(ConstIterator before_it, const T& value)
        requires std::copy_constructible<T> && std::is_nothrow_copy_constructible_v<T> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
        {
            assert(before_it >= this->memory);
            assert(before_it <= this->memory + this->size);

            if(this->capacity != this->size) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(value);
                this->size++;
            }
            else if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * (this->size + 1))) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(value);
                this->size++;
            }
            else //copy left, place, copy right
            {
                Array<T> tmp_array(this->size + 1, this->allocator);
                for(auto it = GetIterator(); it <= before_it; it++) //copy left
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                tmp_array.Push(value); //place

                for(auto it = before_it + 1; it != GetSentinel(); it++) //copy right
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                *this = std::move(tmp_array);
            }
        }

        void Insert(ConstIterator before_it, T&& value)
        requires std::move_constructible<T> && std::is_nothrow_move_constructible_v<T> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
        {
            assert(before_it >= this->memory);
            assert(before_it <= this->memory + this->size);

            if(this->capacity != this->size) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(std::move(value));
                this->size++;
            }
            else if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * (this->size + 1))) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(std::move(value));
                this->size++;
            }
            else //copy left, place, copy right
            {
                Array<T> tmp_array(this->size + 1, this->allocator);
                for(auto it = GetIterator(); it <= before_it; it++) //copy left
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                tmp_array.Push(std::move(value)); //place

                for(auto it = before_it + 1; it != GetSentinel(); it++) //copy right
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                *this = std::move(tmp_array);
            }
        }

        template<typename U>
        requires std::constructible_from<T, U> && std::is_nothrow_constructible_v<T, U> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
        void Insert(ConstIterator before_it, U&& value)
        {
            assert(before_it >= this->memory);
            assert(before_it <= this->memory + this->size);

            if(this->capacity != this->size) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(std::forward<U>(value));
                this->size++;
            }
            else if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * (this->size + 1))) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(std::forward<U>(value));
                this->size++;
            }
            else //copy left, place, copy right
            {
                Array<T> tmp_array(this->size + 1, this->allocator);
                for(auto it = GetIterator(); it <= before_it; it++) //copy left
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                tmp_array.Push(std::forward<U>(value)); //place

                for(auto it = before_it + 1; it != GetSentinel(); it++) //copy right
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                        tmp_array.Push(std::move(*it));
                    else
                        tmp_array.Push(*it);
                }

                *this = std::move(tmp_array);
            }
        }

        void Push(const T& value)
        requires std::copy_constructible<T>
        {
            if(this->capacity == this->size)
                Reserve(this->size + 1);

            new(this->memory + this->size) T(value);

            this->size++;
        }

        void Push(T&& value)
        requires std::move_constructible<T>
        {
            if(this->capacity == this->size)
                Reserve(this->size + 1);

            new(this->memory + this->size) T(std::move(value));

            this->size++;
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void Push(U&& value)
        {
            if(this->capacity == this->size)
                Reserve(this->size + 1);

            new(this->memory + this->size) T(std::forward<U>(value));

            this->size++;
        }

        void Erase(ConstIterator it) noexcept
        {
            Erase(it, it + 1);
        }

        void Erase(ConstIterator begin, ConstIterator end) noexcept
        {
            assert(end > begin);
            assert(begin >= this->memory);
            assert(begin < this->memory + this->size);
            assert(end <= this->memory + this->size);

            size_t steps = end - begin;

            if(end != GetSentinel())
                MoveLeft(const_cast<T*>(end), GetSentinel() - end, steps);

            this->size -= steps;
        }

        void EraseLast() noexcept
        {
            assert(!IsEmpty());

            (this->memory + this->size - 1)->~T();

            this->size--;
        }

        void Reserve(size_t reserve)
        {
            static_assert(std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>);

            if(this->capacity >= reserve)
                return;

            if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * reserve)) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                T* new_memory = reinterpret_cast<T*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));

                for(size_t i = 0; i < this->size; i++)
                {
                    if constexpr(std::is_nothrow_move_constructible_v<T>)
                    {
                        new(new_memory + i) T(std::move(this->memory[i]));
                    }
                    else
                    {
                        new(new_memory + i) T(this->memory[i]);
                    }
                }

                if(this->memory)
                {
                    DestroyObjects();
                    this->allocator.Deallocate(this->memory);
                }

                this->memory = new_memory;
                this->capacity = reserve;
            }
        }

        void Clear() noexcept
        {
            DestroyObjects();

            this->size = 0;
        }

        bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        T* GetData() noexcept
        {
            return this->memory;
        }

        const T* GetData() const noexcept
        {
            return this->memory;
        }

        size_t GetSize() const noexcept
        {
            return this->size;
        }

        size_t GetCapacity() const noexcept
        {
            return this->capacity;
        }

        T& GetFirst() noexcept
        {
            return this->memory[0];
        }

        const T& GetFirst() const noexcept
        {
            return this->memory[0];
        }

        T& GetLast() noexcept
        {
            return this->memory[this->size - 1];
        }

        const T& GetLast() const noexcept
        {
            return this->memory[this->size - 1];
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        Iterator GetIterator() noexcept
        {
            return Iterator(this->memory);
        }

        ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(this->memory);
        }

        Iterator GetSentinel() noexcept
        {
            return Iterator(this->memory + this->size);
        }

        ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->memory + this->size);
        }

        T& operator[](size_t index) noexcept
        {
            assert(index < size);

            return this->memory[index];
        }

        const T& operator[](size_t index) const noexcept
        {
            assert(index < size);

            return this->memory[index];
        }

        static MemoryRequirements GetMemoryRequirements(size_t reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(T), .size = sizeof(T) * reserve};
        }
    private:
        static void MoveRight(T* data, size_t size, size_t steps) noexcept
        {
            static_assert(std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>);

            if(size == 0)
                return;

            T* ptr = data + size - 1;
            for(size_t i = 0; i < size; i++)
            {
                if constexpr(std::is_nothrow_move_constructible_v<T>)
                {
                    new(ptr + steps) T(std::move(*ptr));
                }
                else
                {
                    new(ptr + steps) T(*ptr);
                }

                ptr->~T();

                ptr--;
            }
        }

        static void MoveLeft(T* data, size_t size, size_t steps) noexcept
        {
            static_assert(std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>);

            if(size == 0)
                return;

            T* ptr = data;
            for(size_t i = 0; i < size; i++)
            {
                if constexpr(std::is_nothrow_move_constructible_v<T>)
                {
                    new(ptr - steps) T(std::move(*ptr));
                }
                else
                {
                    new(ptr - steps) T(*ptr);
                }

                ptr->~T();

                ptr++;
            }
        }

        void DestroyObjects() noexcept
        {
            if(this->memory == nullptr)
                return;

            for(size_t i = 0; i < this->size; i++)
                (this->memory + i)->~T();
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
    requires TypeInstantiation<T, Array>
    auto begin(T&& str) noexcept
    {
        return std::forward<T>(str).GetIterator();
    }

    template<typename T>
    requires TypeInstantiation<T, Array>
    auto end(T&& str) noexcept
    {
        return std::forward<T>(str).GetSentinel();
    }

    template<typename T>
    requires TypeInstantiation<T, Array>
    auto size(T&& str) noexcept
    {
        return std::forward<T>(str).GetSize();
    }
};