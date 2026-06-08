#pragma once

#include <ranges>
#include <concepts>
#include "Memory.h"

#include "Instantiation.hpp"
#include <type_traits>

namespace Core
{
    template<typename T>
    requires(!std::is_reference_v<T>)
    class Vector
    {
        using InnerValue = std::remove_cv_t<T>;
    public:
        class Iterator
        {
            friend class Vector;

            Iterator(InnerValue* ptr) noexcept
                : ptr(ptr)
            {}
        public:
            ~Iterator() = default;
            Iterator(const Iterator&) = default;
            Iterator(Iterator&&) = default;
            Iterator& operator=(const Iterator&) = default;
            Iterator& operator=(Iterator&&) = default;

            T& operator*() const noexcept
            {
                return *this->ptr;
            }

            T* operator->() const noexcept
            {
                return this->ptr;
            }

            Iterator operator++(int) noexcept
            {
                Iterator ret_it(this->ptr);
                this->ptr++;

                return ret_it;
            }

            Iterator& operator++() noexcept
            {
                this->ptr++;

                return *this;
            }

            bool operator==(const Iterator& it) const noexcept
            {
                return it.ptr == this->ptr;
            }
        private:
            InnerValue* ptr;
        };

        Vector(Allocator* allocator = GetGlobalAllocator()) noexcept
            : memory(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {}

        ~Vector()
        {
            if(this->memory != nullptr)
                Vector::ClearAndFreeMemory(this->allocator, this->memory, this->size);
        }

        Vector(const Vector& vec)
        requires std::is_copy_constructible_v<InnerValue>
            : Vector()
        {
            this->memory = Vector::AllocateAndPopulateFromCurrent(vec.allocator,
                                                                  vec.memory,
                                                                  vec.size,
                                                                  vec.size,
                                                                  false);
            this->size = vec.size;
            this->capacity = this->size;
            this->allocator = vec.allocator;
        }

        Vector(Vector&& vec) noexcept
            : memory(std::exchange(vec.memory, nullptr)),
              size(vec.size),
              capacity(vec.capacity),
              allocator(vec.allocator)
        {}

        Vector& operator=(const Vector& vec)
        {
            InnerValue* new_memory = Vector::AllocateAndPopulateFromCurrent(vec.allocator,
                                                                            vec.memory,
                                                                            vec.size,
                                                                            vec.size,
                                                                            false);

            this->~Vector();

            this->memory = new_memory;
            this->size = vec.size;
            this->capacity = this->size;
            this->allocator = vec.allocator;

            return *this;
        }

        Vector& operator=(Vector&& vec)
        {
            this->~Vector();

            this->memory = std::exchange(vec.memory, nullptr);
            this->size = vec.size;
            this->capacity = vec.capacity;
            this->allocator = vec.allocator;

            return *this;
        }

        template<typename R>
        requires std::ranges::sized_range<R> &&
                 std::constructible_from<InnerValue, std::ranges::range_value_t<R>>
        Vector(R&& values, Allocator* allocator = GetGlobalAllocator())
        {
            size_t values_size = std::ranges::size(std::forward<R>(values));

            this->memory = Vector::AllocateMemory(allocator, values_size);

            size_t index = 0;
            try
            {
                for(auto&& value: values)
                {
                    if constexpr(std::is_lvalue_reference_v<R>)
                    {
                        new(this->memory + index) InnerValue(value);
                    }
                    else
                    {
                        if constexpr(std::is_nothrow_constructible_v<InnerValue,
                                                                     std::ranges::range_value_t<R>>)
                        {
                            new(this->memory + index) InnerValue(value);
                        }
                        else
                        {
                            new(this->memory + index) InnerValue(value);
                        }
                    }

                    index++;
                }
            }
            catch(...)
            {
                Vector::ClearAndFreeMemory(allocator, this->memory, index);

                throw;
            }

            this->size = values_size;
            this->capacity = this->size;
            this->allocator = allocator;
        }

        Vector(size_t size, const T& value = {})
        requires std::copy_constructible<InnerValue>
        {
            this->allocator = GetGlobalAllocator();

            this->memory =
                AllocateAndPopulateFromCurrent(this->allocator, nullptr, 0, size, false, value);

            this->size = size;
            this->capacity = this->size;
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

        void Reserve(size_t capacity)
        requires std::copy_constructible<T> || std::move_constructible<T>
        {
            if(this->capacity >= capacity)
                return;
            else
            {
                InnerValue* new_memory = AllocateAndPopulateFromCurrent(this->allocator,
                                                                        this->memory,
                                                                        this->size,
                                                                        capacity,
                                                                        true);

                Vector::ClearAndFreeMemory(this->allocator, this->memory, this->size);

                this->memory = new_memory;
                this->capacity = capacity;
            }
        }

        Allocator* GetAllocator() const noexcept
        {
            return this->allocator;
        }

        T* GetData() const noexcept
        {
            return this->memory;
        }

        void Resize(size_t size, const T& value = {})
        requires std::copy_constructible<InnerValue>
        {
            if(this->size >= size)
                return;
            else if(this->capacity >= size)
            {
                Vector::FillFromDefault(this->memory, this->size, size - this->size, value);
            }
            else
            {
                constexpr static size_t MAX_CAPACITY = std::numeric_limits<size_t>::max();

                size_t new_capacity = size;
                if(MAX_CAPACITY - new_capacity >= this->capacity) //adjust as future reserve
                    new_capacity += this->capacity;

                InnerValue* new_memory = AllocateAndPopulateFromCurrent(this->allocator,
                                                                        this->memory,
                                                                        this->size,
                                                                        new_capacity,
                                                                        true,
                                                                        value);

                Vector::ClearAndFreeMemory(this->allocator, this->memory, this->size);

                this->memory = new_memory;
                this->capacity = new_capacity;
            }

            this->size = size;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        template<std::constructible_from<T> U>
        void Insert(Iterator before_it, U&& value);

        template<typename R>
        requires std::ranges::range<R> && std::constructible_from<T, std::ranges::range_value_t<R>>
        void InsertRange(Iterator before_it, R&& values);
        ////////////////////////////////////////////////////////////////////////////////////////////

        template<std::constructible_from<T> U>
        void PushBack(U&& value)
        {
            Insert(End(), std::forward<U>(value));
        }

        template<std::constructible_from<T> U>
        void PushFront(U&& value)
        {
            Insert(std::prev(Begin()), std::forward<U>(value));
        }

        void Erase(Iterator it)
        {
            EraseRange(it, 0);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        void EraseRange(Iterator it, size_t size);
        ////////////////////////////////////////////////////////////////////////////////////////////

        void PopBack(Iterator it)
        {
            return Erase(std::prev(End()));
        }
        void PopFront(Iterator it)
        {
            return Erase(Begin());
        }

        T& operator[](size_t index) const noexcept
        {
            return *(this->memory + index);
        }

        T& GetFirst() const noexcept
        {
            return *(this->memory);
        }

        T& GetLast() const noexcept
        {
            return *(this->memory + (this->size - 1));
        }

        void Clear() noexcept
        {
            if(!IsEmpty())
            {
                Vector::ClearMemory(this->memory, 0, this->size);
                this->size = 0;
            }
        }

        void Release() noexcept
        {
            this->memory = nullptr;
            this->size = 0;
            this->capacity = 0;
        }

        Iterator Begin() const noexcept
        {
            return Iterator(this->memory);
        }

        Iterator End() const noexcept
        {
            return Iterator(this->memory + this->size);
        }
    private:
        static InnerValue* AllocateMemory(Allocator* allocator, size_t size)
        {
            InnerValue* memory = static_cast<InnerValue*>(allocator->Allocate(
                MemoryRequirements{.alignment = alignof(T), .size = sizeof(T) * size}));
            if(!memory)
                CORE_THROW_EXCEPTION_MOCK("BAD_ALLOC")

            return memory;
        }

        static void ClearMemory(InnerValue* memory, size_t offset, size_t size) noexcept
        {
            for(size_t i = 0; i < size; i++)
                (memory + offset + i)->~InnerValue();
        }

        template<typename U>
        static void
        FillFromDefault(InnerValue* memory, size_t offset, size_t size, U&& default_value)
        {
            size_t filled_size = 0;
            try
            {
                for(; filled_size < size; filled_size++)
                    new(memory + offset + filled_size) InnerValue(std::forward<U>(default_value));
            }
            catch(...)
            {
                Vector::ClearMemory(memory, offset, filled_size);
                throw;
            }
        }

        template<typename U>
        static InnerValue* AllocateAndPopulateFromCurrent(Allocator* allocator,
                                                          InnerValue* old_memory,
                                                          size_t old_size,
                                                          size_t new_size,
                                                          bool allow_move,
                                                          U&& default_value)
        {
            //allocate new memory
            InnerValue* new_memory = Vector::AllocateMemory(allocator, new_size);

            size_t default_index = old_size;
            size_t copy_index = 0;
            try
            {
                //populate default
                Vector::FillFromDefault(new_memory,
                                        default_index,
                                        new_size - old_size,
                                        std::forward<U>(default_value));
                default_index = new_size;

                //populate copies from old memory
                for(; copy_index < old_size; copy_index++)
                {
                    if(allow_move)
                    {
                        if constexpr(std::is_nothrow_move_constructible_v<T>)
                            new(new_memory + copy_index)
                                InnerValue(std::move(*(old_memory + copy_index)));
                        else
                            new(new_memory + copy_index) InnerValue(*(old_memory + copy_index));
                    }
                    else
                    {
                        new(new_memory + copy_index) InnerValue(*(old_memory + copy_index));
                    }
                }
            }
            catch(...)
            {
                //delete copied from old memory objects
                Vector::ClearMemory(new_memory, 0, copy_index);

                //delete default objects
                Vector::ClearMemory(new_memory, old_size, default_index - old_size);

                throw;
            }

            return new_memory;
        }

        static InnerValue* AllocateAndPopulateFromCurrent(Allocator* allocator,
                                                          InnerValue* old_memory,
                                                          size_t old_size,
                                                          size_t new_size,
                                                          bool allow_move)
        {
            //allocate new memory
            InnerValue* new_memory = Vector::AllocateMemory(allocator, new_size);

            size_t copy_index = 0;
            try
            {
                //populate copies from old memory
                for(; copy_index < old_size; copy_index++)
                {
                    if(allow_move)
                    {
                        if constexpr(std::is_nothrow_move_constructible_v<T>)
                            new(new_memory + copy_index)
                                InnerValue(std::move(*(old_memory + copy_index)));
                        else
                            new(new_memory + copy_index) InnerValue(*(old_memory + copy_index));
                    }
                    else
                    {
                        new(new_memory + copy_index) InnerValue(*(old_memory + copy_index));
                    }
                }
            }
            catch(...)
            {
                //delete copied from old memory objects
                Vector::ClearMemory(new_memory, 0, copy_index);

                throw;
            }

            return new_memory;
        }

        static void
        ClearAndFreeMemory(Allocator* allocator, InnerValue* memory, size_t size) noexcept
        {
            Vector::ClearMemory(memory, 0, size);

            allocator->Deallocate(memory);
        }
    private:
        InnerValue* memory;
        size_t size;
        size_t capacity;
        Allocator* allocator;
    };

    //std compat
    /*template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Vector>
    auto begin(T&& vec) noexcept
    {
        return std::forward<T>(vec).Begin();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Vector>
    auto end(T&& vec) noexcept
    {
        return std::forward<T>(vec).End();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Vector>
    auto size(T&& vec) noexcept
    {
        return std::forward<T>(vec).GetSize();
    }*/

    template<typename T>
    auto begin(const Vector<T>& vec) noexcept
    {
        return vec.Begin();
    }

    template<typename T>
    auto end(const Vector<T>& vec) noexcept
    {
        return vec.End();
    }

    template<typename T>
    auto size(const Vector<T>& vec) noexcept
    {
        return vec.size();
    }
};