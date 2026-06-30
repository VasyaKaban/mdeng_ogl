#pragma once

#include "Memory.h"
#include "RangeTraits.hpp"
#include <cassert>

namespace Core
{
    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, T>
    class Sequence
    {
    public:
        using Iterator = T*;
        using ConstIterator = const T*;

        Sequence(Allocator allocator = GetGlobalAllocator()) noexcept
            : memory(nullptr),
              size(0),
              capacity(0),
              allocator(allocator)
        {}

        Sequence(DeviceSize reserve, Allocator allocator = GetGlobalAllocator()) noexcept
            : Sequence(allocator)
        {
            Reserve(reserve);
        }

        ~Sequence()
        {
            if(this->memory)
            {
                DestroyObjects();

                this->allocator.Deallocate(this->memory);
            }
        }

        Sequence(const Sequence& seq)
        {
            Sequence tmp_seq(seq.size, seq.allocator);

            for(DeviceSize i = 0; i < seq.size; i++)
                tmp_seq.Push(seq[i]);

            *this = Move(tmp_seq);
        }

        Sequence(Sequence&& seq) noexcept
            : memory(Exchange(seq.memory, nullptr)),
              size(Exchange(seq.size, 0)),
              capacity(Exchange(seq.capacity, 0)),
              allocator(seq.allocator)
        {}

        Sequence& operator=(const Sequence& seq)
        {
            Sequence tmp_seq(seq.size, seq.allocator);

            for(DeviceSize i = 0; i < seq.size; i++)
                tmp_seq.Push(seq[i]);

            *this = Move(tmp_seq);

            return *this;
        }

        Sequence& operator=(Sequence&& seq) noexcept
        {
            this->~Sequence();

            this->memory = Exchange(seq.memory, nullptr);
            this->size = Exchange(seq.size, 0);
            this->capacity = Exchange(seq.capacity, 0);
            this->allocator = seq.allocator;

            return *this;
        }

        template<SizedRange R>
        Sequence(R&& values, Allocator allocator = GetGlobalAllocator())
        {
            Sequence tmp_seq(size(Forward(values)), allocator);

            for(auto&& value: Forward(values))
                tmp_seq.Push(Forward(value));

            *this = Move(tmp_seq);
        }

        //on enough capacity -> move right and place
        //on grow -> move right and place
        //om alloc -> copy left, place, copy right
        template<typename... Args>
        requires NoexceptConstructible<T, Args...> && (NoexceptMoveConstructible<T> || NoexceptCopyConstructible<T>)
        Void Insert(ConstIterator before_it, Args&&... args)
        {
            assert(before_it >= this->memory);
            assert(before_it <= this->memory + this->size);

            if(this->capacity != this->size) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(Forward(args)...);
                this->size++;
            }
            else if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * (this->size + 1))) //move right and place
            {
                MoveRight(const_cast<T*>(before_it) + 1, GetSentinel() - (before_it + 1), 1);
                new(const_cast<T*>(before_it) + 1) T(Forward(args)...);
                this->size++;
            }
            else //copy left, place, copy right
            {
                Sequence<T> tmp_seq(this->size + 1, this->allocator);
                for(auto it = GetIterator(); it <= before_it; it++) //copy left
                {
                    if constexpr(NoexceptMoveConstructible<T>)
                        tmp_seq.Push(Move(*it));
                    else
                        tmp_seq.Push(*it);
                }

                tmp_seq.Push(Forward(args)...); //place

                for(auto it = before_it + 1; it != GetSentinel(); it++) //copy right
                {
                    if constexpr(NoexceptMoveConstructible<T>)
                        tmp_seq.Push(Move(*it));
                    else
                        tmp_seq.Push(*it);
                }

                *this = Move(tmp_seq);
            }
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void Push(Args&&... args)
        {
            if(this->capacity == this->size)
                Reserve(this->size + 1);

            new(this->memory + this->size) T(Forward(args)...);

            this->size++;
        }

        Void Erase(ConstIterator it) noexcept
        {
            Erase(it, it + 1);
        }

        Void Erase(ConstIterator begin, ConstIterator end) noexcept
        {
            assert(end > begin);
            assert(begin >= this->memory);
            assert(begin < this->memory + this->size);
            assert(end <= this->memory + this->size);

            DeviceSize steps = end - begin;

            //destroy objects to destroy
            for(DeviceSize i = begin - this->memory; i != end - this->memory; i++)
            {
                (this->memory + i)->~T();
            }

            //move remain objects to the left side
            if(end != GetSentinel()) //erase last
                MoveLeft(const_cast<T*>(end), GetSentinel() - end, steps);

            this->size -= steps;
        }

        Void EraseLast() noexcept
        {
            assert(!IsEmpty());

            (this->memory + this->size - 1)->~T();

            this->size--;
        }

        Void Reserve(DeviceSize reserve)
        requires NoexceptMoveConstructible<T> || NoexceptCopyConstructible<T>
        {
            if(this->capacity >= reserve)
                return;

            if(this->memory != nullptr && this->allocator.Grow(this->memory, sizeof(T) * reserve)) //try grow
            {
                this->capacity = reserve;
            }
            else //allocate new buffer
            {
                T* new_memory = reinterpret_cast<T*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));

                for(DeviceSize i = 0; i < this->size; i++)
                {
                    if constexpr(NoexceptMoveConstructible<T>)
                    {
                        new(new_memory + i) T(Move(this->memory[i]));
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

        Void Clear() noexcept
        {
            if(this->memory)
            {
                DestroyObjects();

                this->size = 0;
            }
        }

        Bool IsEmpty() const noexcept
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

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        DeviceSize GetCapacity() const noexcept
        {
            return this->capacity;
        }

        T& GetFirst() noexcept
        {
            assert(this->size != 0);

            return this->memory[0];
        }

        const T& GetFirst() const noexcept
        {
            assert(this->size != 0);

            return this->memory[0];
        }

        T& GetLast() noexcept
        {
            assert(this->size != 0);

            return this->memory[this->size - 1];
        }

        const T& GetLast() const noexcept
        {
            assert(this->size != 0);

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

        T& operator[](DeviceSize index) noexcept
        {
            assert(index < size);

            return this->memory[index];
        }

        const T& operator[](DeviceSize index) const noexcept
        {
            assert(index < size);

            return this->memory[index];
        }

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(T), .size = sizeof(T) * reserve};
        }
    private:
        static Void MoveRight(T* data, DeviceSize size, DeviceSize steps) noexcept
        requires NoexceptMoveConstructible<T> || NoexceptCopyConstructible<T>
        {
            if(size == 0)
                return;

            T* ptr = data + size - 1;
            for(DeviceSize i = 0; i < size; i++)
            {
                if constexpr(NoexceptMoveConstructible<T>)
                {
                    new(ptr + steps) T(Move(*ptr));
                }
                else
                {
                    new(ptr + steps) T(*ptr);
                }

                ptr->~T();

                ptr--;
            }
        }

        static Void MoveLeft(T* data, DeviceSize size, DeviceSize steps) noexcept
        requires NoexceptMoveConstructible<T> || NoexceptCopyConstructible<T>
        {
            if(size == 0)
                return;

            T* ptr = data;
            for(DeviceSize i = 0; i < size; i++)
            {
                if constexpr(NoexceptMoveConstructible<T>)
                {
                    new(ptr - steps) T(Move(*ptr));
                }
                else
                {
                    new(ptr - steps) T(*ptr);
                }

                ptr->~T();

                ptr++;
            }
        }

        Void DestroyObjects() noexcept
        {
            for(DeviceSize i = 0; i < this->size; i++)
                (this->memory + i)->~T();
        }
    private:
        T* memory;
        DeviceSize size;
        DeviceSize capacity;
        Allocator allocator;
    };

    template<SizedRange R>
    Sequence(R&& values) -> Sequence<DropConstVolatileReference<RangeDereferenceType<R>>>;

    //std compat
    template<typename T>
    requires TypeInstantiation<T, Sequence>
    auto begin(T&& seq) noexcept
    {
        return Forward(seq).GetIterator();
    }

    template<typename T>
    requires TypeInstantiation<T, Sequence>
    auto end(T&& seq) noexcept
    {
        return Forward(seq).GetSentinel();
    }

    template<typename T>
    requires TypeInstantiation<T, Sequence>
    auto size(T&& seq) noexcept
    {
        return Forward(seq).GetSize();
    }
};