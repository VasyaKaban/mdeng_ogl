#pragma once

#include "Memory.h"

namespace Core
{
    namespace Detail
    {
        template<typename T, typename F>
        requires std::invocable<F, T*>
        void RingForEach(T* data, size_t capacity, size_t size, T* next_pop, T* next_push, F&& func)
        {
            if(next_pop == nullptr) //empty -> no-op
                return;

            if(next_push == nullptr) //full -> iterate from begin to end
            {
                for(size_t i = 0; i < size; i++)
                    std::forward<F>(func)(data + i);
            }
            else
            {
                if(next_push > next_pop) //linear -> iterate from push to pop
                {
                    auto ptr = next_pop;
                    while(ptr != next_push)
                    {
                        std::forward<F>(func)(ptr);
                        ptr++;
                    }
                }
                else //non-linear -> iterate from pop to end and then from begin to push
                {
                    auto ptr = next_pop;
                    while(ptr != (data + capacity))
                    {
                        std::forward<F>(func)(ptr);
                        ptr++;
                    }

                    ptr = data;
                    while(ptr != next_push)
                    {
                        std::forward<F>(func)(ptr);
                        ptr++;
                    }
                }
            }
        }
    };

    template<typename T>
    requires std::same_as<std::remove_cvref_t<T>, T>
    class Ring
    {
    public:
        Ring(Allocator allocator = GetGlobalAllocator())
            : data(nullptr),
              size(0),
              capacity(0),
              next_push(nullptr),
              next_pop(nullptr),
              allocator(allocator)
        {}

        Ring(size_t reserve, Allocator allocator = GetGlobalAllocator())
            : Ring(allocator)
        {
            Reserve(reserve);
        }

        ~Ring()
        {
            DestroyObjects();

            if(this->data)
                this->allocator.Deallocate(this->data);
        }

        Ring(const Ring& ring)
        requires std::is_copy_constructible_v<T>
        {
            Ring tmp_ring(ring.size, ring.allocator);

            RingForEach(ring.data,
                        ring.capacity,
                        ring.size,
                        ring.next_pop,
                        ring.next_push,
                        [&tmp_ring](const T* obj)
                        {
                            tmp_ring.Push(*obj);
                        });

            *this = std::move(tmp_ring);
        }

        Ring(Ring&& ring) noexcept
            : data(std::exchange(ring.data, nullptr)),
              size(std::exchange(ring.size, 0)),
              capacity(std::exchange(ring.capacity, 0)),
              next_push(std::exchange(ring.next_push, nullptr)),
              next_pop(std::exchange(ring.next_pop, nullptr)),
              allocator(ring.allocator)
        {}

        Ring& operator=(const Ring& ring)
        requires std::is_copy_constructible_v<T>
        {
            *this = Ring(this->allocator);

            Ring tmp_ring(ring.size, ring.allocator);

            RingForEach(ring.data,
                        ring.capacity,
                        ring.size,
                        ring.next_pop,
                        ring.next_push,
                        [&tmp_ring](const T* obj)
                        {
                            tmp_ring.Push(*obj);
                        });

            *this = std::move(tmp_ring);

            return *this;
        }

        Ring& operator=(Ring&& ring) noexcept
        {
            this->~Ring();

            this->data = std::exchange(ring.data, nullptr);
            this->size = std::exchange(ring.size, 0);
            this->capacity = std::exchange(ring.capacity, 0);
            this->next_push = std::exchange(ring.next_push, nullptr);
            this->next_pop = std::exchange(ring.next_pop, nullptr);
            this->allocator = ring.allocator;

            return *this;
        }

        T* GetData() noexcept
        {
            return this->data;
        }

        const T* GetData() const noexcept
        {
            return this->data;
        }

        size_t GetSize() const noexcept
        {
            return this->size;
        }

        size_t GetCapacity() const noexcept
        {
            return this->capacity;
        }

        bool CanPop() const noexcept
        {
            return this->next_pop != nullptr;
        }

        bool CanPush() const noexcept
        {
            return this->next_push != nullptr;
        }

        void Push(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
        {
            assert(CanPush());

            new(this->next_push) T(value);

            CorrectPointers(this->next_push, this->next_pop);

            this->size++;
        }

        void Push(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
        {
            assert(CanPush());

            new(this->next_push) T(std::move(value));

            CorrectPointers(this->next_push, this->next_pop);

            this->size++;
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void Push(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            assert(CanPush());

            new(this->next_push) T(std::forward<U>(value));

            CorrectPointers(this->next_push, this->next_pop);

            this->size++;
        }

        T& GetFirst() noexcept
        {
            return *this->next_pop;
        }

        const T& GetFirst() const noexcept
        {
            return *this->next_pop;
        }

        void Pop() noexcept
        {
            assert(CanPop());

            this->next_pop->~T();

            CorrectPointers(this->next_pop, this->next_push);

            this->size--;
        }

        //we can use allocator.Grow() only when pointer are not on different sides of buffer(push and pop do not overlap border)
        //on empty -> try grow
        //on full -> try grow
        //on linear -> try grow
        //otherwise only realloca
        void Reserve(size_t reserve)
        requires std::is_nothrow_move_constructible_v<T> || std::copy_constructible<T>
        {
            if(this->capacity >= reserve)
                return;

            if(this->data && (!CanPop() || !CanPush() || (this->next_push > this->next_pop))) //empty or full or linear -> try grow
            {
                if(this->allocator.Grow(this->data, sizeof(T) * reserve))
                {
                    this->capacity = reserve;
                    return;
                }
            }

            T* new_memory = reinterpret_cast<T*>(this->allocator.Allocate(GetMemoryRequirements(reserve)));
            size_t offset = 0;

            try
            {
                RingForEach(this->data,
                            this->capacity,
                            this->size,
                            this->next_pop,
                            this->next_push,
                            [this, new_memory, &offset](T* obj)
                            {
                                if constexpr(std::is_nothrow_move_constructible_v<T>)
                                    new(new_memory + offset) T(std::move(*obj));
                                else
                                    new(new_memory + offset) T(*obj);

                                offset++;
                            });
            }
            catch(...)
            {
                for(size_t i = 0; i < offset; i++)
                    new_memory[i].~T();

                this->allocator.Deallocate(new_memory);

                throw;
            }

            if(this->data)
            {
                DestroyObjects();
                this->allocator.Deallocate(this->data);
            }

            this->data = new_memory;
            this->capacity = reserve;

            if(offset == 0)
            {
                this->next_push = this->data;
                this->next_pop = nullptr;
            }
            else
            {
                this->next_push = this->data + offset;
                this->next_pop = this->data;
            }
        }

        void Clear() noexcept
        {
            DestroyObjects();

            this->next_push = this->data;
            this->next_pop = nullptr;
        }

        static MemoryRequirements GetMemoryRequirements(size_t reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(T), .size = sizeof(T) * reserve};
        }
    private:
        void DestroyObjects() noexcept
        {
            RingForEach(this->data,
                        this->capacity,
                        this->size,
                        this->next_pop,
                        this->next_push,
                        [](T* obj)
                        {
                            obj->~T();
                        });
        }

        void CorrectPointers(T*& moved_ptr, T*& static_ptr) noexcept
        {
            if(static_ptr == nullptr)
                static_ptr = moved_ptr;

            moved_ptr++;
            if(moved_ptr == this->data + this->capacity)
                moved_ptr = this->data;

            if(moved_ptr == static_ptr) //reset
                moved_ptr = nullptr;
        }
    private:
        T* data;
        size_t size;
        size_t capacity;
        T* next_push;
        T* next_pop;
        Allocator allocator;
    };
};