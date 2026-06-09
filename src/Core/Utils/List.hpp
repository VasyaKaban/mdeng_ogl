#pragma once

#include <concepts>
#include <cassert>
#include <utility>
#include <ranges>
#include "Memory.h"
#include "Instantiation.hpp"

namespace Core
{
    template<typename T>
    requires std::same_as<T, std::remove_cvref_t<T>>
    class List;

    namespace Detail
    {
        template<typename T>
        struct ListNode;

        struct ListNodeBase
        {
            ListNodeBase* prev;
            ListNodeBase* next;

            ListNodeBase(ListNodeBase* prev = nullptr, ListNodeBase* next = nullptr) noexcept
                : prev(prev),
                  next(next)
            {}

            static ListNodeBase SelfLinked(ListNodeBase* node) noexcept
            {
                return ListNodeBase(node, node);
            }

            template<typename T>
            ListNode<T>* AsListNode() noexcept
            {
                return static_cast<ListNode<T>*>(this);
            }
        };

        template<typename T>
        struct ListNode : ListNodeBase
        {
            T value;

            template<typename U>
            requires std::constructible_from<T, U>
            ListNode(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
                : ListNodeBase(),
                  value(std::forward<U>(value))
            {}

            ~ListNode() = default;

            ListNode(const ListNode&) = delete;
            ListNode(ListNode&&) = delete;
            ListNode& operator=(const ListNode&) = delete;
            ListNode& operator=(ListNode&&) = delete;
        };

        template<typename U>
        class ListIterator
        {
            template<typename T>
            requires std::same_as<T, std::remove_cvref_t<T>>
            friend class ::Core::List;

            using ListValueType = std::remove_cvref_t<U>;

            ListIterator(Detail::ListNodeBase* node) noexcept
                : node(node)
            {}
        public:
            ListIterator() noexcept
                : node(nullptr)
            {}

            ~ListIterator() = default;
            ListIterator(const ListIterator&) = default;
            ListIterator(ListIterator&&) = default;
            ListIterator& operator=(const ListIterator&) = default;
            ListIterator& operator=(ListIterator&&) = default;

            ListIterator operator++(int) noexcept
            {
                ListIterator ret(this->node);

                this->node = this->node->next;

                return ret;
            }

            ListIterator& operator++() noexcept
            {
                this->node = this->node->next;

                return *this;
            }

            ListIterator operator--(int) noexcept
            {
                ListIterator ret(this->node);

                this->node = this->node->prev;

                return ret;
            }

            ListIterator& operator--() noexcept
            {
                this->node = this->node->prev;

                return *this;
            }

            U& operator*() const noexcept
            {
                return node->AsListNode<ListValueType>()->value;
            }

            U* operator->() const noexcept
            {
                return std::addressof(node->AsListNode<ListValueType>()->value);
            }

            bool operator==(const ListIterator& it) const noexcept
            {
                return this->node == it.node;
            }
        private:
            Detail::ListNodeBase* node;
        };
    };

    template<typename T>
    requires std::same_as<T, std::remove_cvref_t<T>>
    class List
    {
        using Node = Detail::ListNode<T>;
        constexpr static MemoryRequirements LIST_NODE_MEMORY_REQUIREMENTS{.alignment =
                                                                              alignof(Node),
                                                                          .size = sizeof(Node)};
    public:
        using Iterator = Detail::ListIterator<T>;
        using ConstIterator = Detail::ListIterator<const T>;

        List(Allocator allocator = GetGlobalAllocator()) noexcept
            : base(Detail::ListNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {}

        ~List()
        {
            Clear();
        }

        List(const List& list)
            : base(Detail::ListNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(list.allocator)
        {
            try
            {
                for(auto it = list.Begin(); it != list.End(); it++)
                    PushToEnd(*it);
            }
            catch(...)
            {
                Clear();
                throw;
            }
        }

        List(List&& list) noexcept
            : base(std::exchange(list.base, Detail::ListNodeBase::SelfLinked(&list.base))),
              size(std::exchange(list.size, 0)),
              allocator(list.allocator)
        {
            UpdateBase();
        }

        List& operator=(const List& list)
        {
            this->~Clear();

            try
            {
                for(auto it = list.Begin(); it != list.End(); it++)
                    PushToEnd(*it);
            }
            catch(...)
            {
                Clear();
                throw;
            }

            return *this;
        }

        List& operator=(List&& list) noexcept
        {
            this->~Clear();

            this->base = std::exchange(list.base, Detail::ListNodeBase::SelfLinked(&list.base));
            this->size = std::exchange(list.size, 0);
            this->allocator = list.allocator;

            UpdateBase();

            return *this;
        }

        template<std::ranges::range R>
        List(R&& values, Allocator allocator = GetGlobalAllocator())
            : base(Detail::ListNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {
            try
            {
                for(auto&& value: std::forward<R>(values))
                    PushToEnd(std::forward<decltype(value)>(value));
            }
            catch(...)
            {
                Clear();
                throw;
            }
        }

        size_t GetSize() const noexcept
        {
            return this->size;
        }

        bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void Insert(ConstIterator before_it,
                    U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            AllocateAndInsertNode(std::forward<U>(value), before_it.node);
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void PushToBegin(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            AllocateAndInsertNode(std::forward<U>(value), &this->base);
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void PushToEnd(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            AllocateAndInsertNode(std::forward<U>(value), this->base.prev);
        }

        void
        Splice(ConstIterator before_it, List& list, ConstIterator begin, ConstIterator end) noexcept
        {
            Detail::ListNodeBase* before_node = before_it.node;

            for(auto it = begin; it != end; it++)
            {
                Node* it_node = it.node;
                list.EraseNode(it_node);
                InsertNode(before_node, it_node);

                before_node = before_node->next;
            }
        }

        void Clear() noexcept
        {
            Node* node = this->base.next->AsListNode<T>();
            for(size_t i = 0; i < this->size; i++)
            {
                Detail::ListNodeBase* next = node->next;
                node->~Node();
                this->allocator.Deallocate(node);

                node = next->AsListNode<T>();
            }

            this->size = 0;
            this->base = Detail::ListNodeBase::SelfLinked(&this->base);
        }

        void Erase(ConstIterator it) noexcept
        {
            EraseAndFreeNode(it.node);
        }

        void EraseFirst() noexcept
        {
            EraseAndFreeNode(this->base.next);
        }

        void EraseLast() noexcept
        {
            EraseAndFreeNode(this->base.prev);
        }

        T& GetFirst() noexcept
        {
            return this->base.next->AsListNode<T>()->value;
        }

        const T& GetFirst() const noexcept
        {
            return this->base.next->AsListNode<T>()->value;
        }

        T& GetLast() noexcept
        {
            return this->base.prev->AsListNode<T>()->value;
        }

        const T& GetLast() const noexcept
        {
            return this->base.prev->AsListNode<T>()->value;
        }

        Iterator Begin() noexcept
        {
            return Iterator(this->base.next);
        }

        Iterator End() noexcept
        {
            return Iterator(&this->base);
        }

        ConstIterator Begin() const noexcept
        {
            return ConstIterator(this->base.next);
        }

        ConstIterator End() const noexcept
        {
            return ConstIterator(this->base);
        }
    public:
        void
        UpdateBase() noexcept //use on move when we must change first and last references to the base
        {
            if(this->size == 0)
            {
                this->base = Detail::ListNodeBase::SelfLinked(&this->base);
            }
            else
            {
                this->base.next->prev = &this->base;
                this->base.prev->next = &this->base;
            }
        }

        void InsertNode(Detail::ListNodeBase* prev_node,
                        Node* node) noexcept //insert node right after prev_node
        {
            if(this->size == 0)
            {
                assert(prev_node == &this->base);

                this->base.next = node;
                this->base.prev = node;

                node->next = &this->base;
                node->prev = &this->base;
            }
            else
            {
                Detail::ListNodeBase* next_node = prev_node->next;

                prev_node->next = node;
                next_node->prev = node;

                node->next = next_node;
                node->prev = prev_node;
            }

            this->size++;
        }

        template<typename U>
        void AllocateAndInsertNode(
            U&& value,
            Detail::ListNodeBase* prev_node) //allocate node and insert right after prev_node
        {
            Node* node =
                static_cast<Node*>(this->allocator.Allocate(LIST_NODE_MEMORY_REQUIREMENTS));
            if(!node)
                CORE_THROW_EXCEPTION_MOCK("Bad alloc")

            try
            {
                new(node) Node(std::forward<U>(value));
            }
            catch(...)
            {
                this->allocator.Deallocate(node);
            }

            InsertNode(prev_node, node);
        }

        void EraseNode(Node* node) noexcept //erase node without dealloc
        {
            Node* prev_node = node->prev;
            Node* next_node = node->next;

            if(this->size == 0)
            {
                assert(prev_node == next_node && prev_node == &this->base);

                this->base = Detail::ListNodeBase::SelfLinked(&this->base);
            }
            else
            {
                prev_node->next = next_node;
                next_node->prev = prev_node;
            }

            this->size--;
        }

        void EraseAndFreeNode(Node* node) noexcept //erase node with dealloc
        {
            EraseNode(node);

            node->~Node();
            this->allocator.Deallocate(node);
        }
    private:
        Detail::ListNodeBase base;
        size_t size;
        Allocator allocator;
    };

    template<std::ranges::range R>
    List(R&& values)
        -> List<std::remove_cvref_t<std::ranges::range_value_t<std::remove_cvref_t<R>>>>;

    //std compat
    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, List>
    auto begin(T&& list) noexcept
    {
        return std::forward<T>(list).Begin();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, List>
    auto end(T&& list) noexcept
    {
        return std::forward<T>(list).End();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, List>
    auto size(T&& list) noexcept
    {
        return std::forward<T>(list).GetSize();
    }
};

//std compat
namespace std
{
    template<typename T>
    struct iterator_traits<Core::Detail::ListIterator<T>>
    {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;
    };
};