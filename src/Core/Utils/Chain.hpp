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
    class Chain;

    namespace Detail
    {
        template<typename T>
        struct ChainNode;

        struct ChainNodeBase
        {
            ChainNodeBase* prev;
            ChainNodeBase* next;

            ChainNodeBase(ChainNodeBase* prev = nullptr, ChainNodeBase* next = nullptr) noexcept
                : prev(prev),
                  next(next)
            {}

            static ChainNodeBase SelfLinked(ChainNodeBase* node) noexcept
            {
                return ChainNodeBase(node, node);
            }

            template<typename T>
            ChainNode<T>* AsChainNode() noexcept
            {
                return static_cast<ChainNode<T>*>(this);
            }
        };

        template<typename T>
        struct ChainNode : ChainNodeBase
        {
            T value;

            template<typename U>
            requires std::constructible_from<T, U>
            ChainNode(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
                : ChainNodeBase(),
                  value(std::forward<U>(value))
            {}

            ~ChainNode() = default;

            ChainNode(const ChainNode&) = delete;
            ChainNode(ChainNode&&) = delete;
            ChainNode& operator=(const ChainNode&) = delete;
            ChainNode& operator=(ChainNode&&) = delete;
        };

        template<typename U>
        class ChainIterator
        {
            template<typename T>
            friend class ::Core::Chain;

            using ChainValueType = std::remove_cvref_t<U>;

            ChainIterator(Detail::ChainNodeBase* node) noexcept
                : node(node)
            {}
        public:
            ChainIterator() noexcept
                : node(nullptr)
            {}

            ~ChainIterator() = default;
            ChainIterator(const ChainIterator&) = default;
            ChainIterator(ChainIterator&&) = default;
            ChainIterator& operator=(const ChainIterator&) = default;
            ChainIterator& operator=(ChainIterator&&) = default;

            ChainIterator operator++(int) noexcept
            {
                ChainIterator ret(this->node);

                ++(*this);

                return ret;
            }

            ChainIterator& operator++() noexcept
            {
                this->node = this->node->next;

                return *this;
            }

            ChainIterator operator--(int) noexcept
            {
                ChainIterator ret(this->node);

                this->node = this->node->prev;

                return ret;
            }

            ChainIterator& operator--() noexcept
            {
                this->node = this->node->prev;

                return *this;
            }

            U& operator*() const noexcept
            {
                return node->AsChainNode<ChainValueType>()->value;
            }

            U* operator->() const noexcept
            {
                return std::addressof(node->AsChainNode<ChainValueType>()->value);
            }

            bool operator==(const ChainIterator& it) const noexcept
            {
                return this->node == it.node;
            }
        private:
            Detail::ChainNodeBase* node;
        };
    };

    template<typename T>
    class Chain
    {
        static_assert(std::same_as<T, std::remove_cvref_t<T>>);

        using Node = Detail::ChainNode<T>;
    public:
        using Iterator = Detail::ChainIterator<T>;
        using ConstIterator = Detail::ChainIterator<const T>;

        Chain(Allocator allocator = GetGlobalAllocator()) noexcept
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {}

        ~Chain()
        {
            Clear();
        }

        Chain(const Chain& chain)
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(chain.allocator)
        {
            Chain tmp_chain(chain.allocator);
            for(const auto& value: chain)
                tmp_chain.PushToEnd(value);

            *this = std::move(tmp_chain);
        }

        Chain(Chain&& chain) noexcept
            : base(std::exchange(chain.base, Detail::ChainNodeBase::SelfLinked(&chain.base))),
              size(std::exchange(chain.size, 0)),
              allocator(chain.allocator)
        {
            UpdateBase();
        }

        Chain& operator=(const Chain& chain)
        {
            *this = Chain(this->allocator);

            Chain tmp_chain(chain.allocator);
            for(const auto& value: chain)
                tmp_chain.PushToEnd(value);

            *this = std::move(tmp_chain);

            return *this;
        }

        Chain& operator=(Chain&& chain) noexcept
        {
            this->~Chain();

            this->base = std::exchange(chain.base, Detail::ChainNodeBase::SelfLinked(&chain.base));
            this->size = std::exchange(chain.size, 0);
            this->allocator = chain.allocator;

            UpdateBase();

            return *this;
        }

        template<std::ranges::range R>
        Chain(R&& values, Allocator allocator = GetGlobalAllocator())
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
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
        void Insert(ConstIterator before_it, U&& value)
        {
            AllocateAndInsertNode(std::forward<U>(value), before_it.node);
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void PushToBegin(U&& value)
        {
            AllocateAndInsertNode(std::forward<U>(value), &this->base);
        }

        template<typename U>
        requires std::constructible_from<T, U>
        void PushToEnd(U&& value)
        {
            AllocateAndInsertNode(std::forward<U>(value), this->base.prev);
        }

        void Splice(ConstIterator before_it, Chain& chain, ConstIterator begin, ConstIterator end) noexcept
        {
            Detail::ChainNodeBase* before_node = before_it.node;

            for(auto it = begin; it != end; it++)
            {
                Node* it_node = it.node;
                chain.EraseNode(it_node);
                InsertNode(before_node, it_node);

                before_node = before_node->next;
            }
        }

        void Clear() noexcept
        {
            Node* node = this->base.next->AsChainNode<T>();
            for(size_t i = 0; i < this->size; i++)
            {
                Detail::ChainNodeBase* next = node->next;
                node->~Node();
                this->allocator.Deallocate(node);

                node = next->AsChainNode<T>();
            }

            this->size = 0;
            this->base = Detail::ChainNodeBase::SelfLinked(&this->base);
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
            return this->base.next->AsChainNode<T>()->value;
        }

        const T& GetFirst() const noexcept
        {
            return this->base.next->AsChainNode<T>()->value;
        }

        T& GetLast() noexcept
        {
            return this->base.prev->AsChainNode<T>()->value;
        }

        const T& GetLast() const noexcept
        {
            return this->base.prev->AsChainNode<T>()->value;
        }

        Iterator Begin() noexcept
        {
            return Iterator(this->base.next);
        }

        ConstIterator Begin() const noexcept
        {
            return ConstIterator(this->base.next);
        }

        Iterator End() noexcept
        {
            return Iterator(&this->base);
        }

        ConstIterator End() const noexcept
        {
            return ConstIterator(this->base);
        }
    public:
        void UpdateBase() noexcept //use on move when we must change first and last references to the base
        {
            if(this->size == 0)
            {
                this->base = Detail::ChainNodeBase::SelfLinked(&this->base);
            }
            else
            {
                this->base.next->prev = &this->base;
                this->base.prev->next = &this->base;
            }
        }

        void InsertNode(Detail::ChainNodeBase* prev_node,
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
                Detail::ChainNodeBase* next_node = prev_node->next;

                prev_node->next = node;
                next_node->prev = node;

                node->next = next_node;
                node->prev = prev_node;
            }

            this->size++;
        }

        template<typename U>
        void AllocateAndInsertNode(U&& value,
                                   Detail::ChainNodeBase* prev_node) //allocate node and insert right after prev_node
        {
            Node* node = static_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node(std::forward<U>(value));
            }
            catch(...)
            {
                this->allocator.Deallocate(node);
                throw;
            }

            InsertNode(prev_node, node);
        }

        void EraseNode(Node* node) noexcept //erase node without dealloc
        {
            assert(this->size != 0);

            Node* prev_node = node->prev;
            Node* next_node = node->next;

            if(this->size == 1)
            {
                assert(prev_node == next_node && prev_node == &this->base);

                this->base = Detail::ChainNodeBase::SelfLinked(&this->base);
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
        Detail::ChainNodeBase base;
        size_t size;
        Allocator allocator;
    };

    template<std::ranges::range R>
    Chain(R&& values) -> Chain<std::remove_cvref_t<std::ranges::range_value_t<std::remove_cvref_t<R>>>>;

    //std compat
    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Chain>
    auto begin(T&& chain) noexcept
    {
        return std::forward<T>(chain).Begin();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Chain>
    auto end(T&& chain) noexcept
    {
        return std::forward<T>(chain).End();
    }

    template<typename T>
    requires TypeInstantiation<std::remove_cvref_t<T>, Chain>
    auto size(T&& chain) noexcept
    {
        return std::forward<T>(chain).GetSize();
    }
};

//std compat
namespace std
{
    template<typename T>
    struct iterator_traits<::Core::Detail::ChainIterator<T>>
    {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;
    };
};
