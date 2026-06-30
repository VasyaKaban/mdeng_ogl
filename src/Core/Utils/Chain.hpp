#pragma once

#include "Memory.h"
#include "RangeTraits.hpp"

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

            template<typename... Args>
            requires Constructible<T, Args...>
            ChainNode(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
                : ChainNodeBase(),
                  value(Forward(args)...)
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

            using ChainValueType = DropConstVolatileReference<U>;
            using Base = Conditional<Const<U>, const Detail::ChainNodeBase, Detail::ChainNodeBase>;

            ChainIterator(Base* node) noexcept
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
                return node->template AsChainNode<ChainValueType>()->value;
            }

            U* operator->() const noexcept
            {
                return GetAddress(node->template AsChainNode<ChainValueType>()->value);
            }

            Bool operator==(const ChainIterator& it) const noexcept
            {
                return this->node == it.node;
            }
        private:
            Base* node;
        };
    };

    template<typename T>
    class Chain
    {
        static_assert(SameAs<T, DropConstVolatileReference<T>>);

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

            *this = Move(tmp_chain);
        }

        Chain(Chain&& chain) noexcept
            : base(Exchange(chain.base, Detail::ChainNodeBase::SelfLinked(&chain.base))),
              size(Exchange(chain.size, 0)),
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

            *this = Move(tmp_chain);

            return *this;
        }

        Chain& operator=(Chain&& chain) noexcept
        {
            this->~Chain();

            this->base = Exchange(chain.base, Detail::ChainNodeBase::SelfLinked(&chain.base));
            this->size = Exchange(chain.size, 0);
            this->allocator = chain.allocator;

            UpdateBase();

            return *this;
        }

        template<Range R>
        Chain(R&& values, Allocator allocator = GetGlobalAllocator())
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {
            try
            {
                for(auto&& value: Forward(values))
                    PushToEnd(Forward(value));
            }
            catch(...)
            {
                Clear();
                throw;
            }
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        Bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void Insert(ConstIterator before_it, Args&&... args)
        {
            AllocateAndInsertNode(Forward(args)..., before_it.node);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToBegin(Args&&... args)
        {
            AllocateAndInsertNode(Forward(args)..., &this->base);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToEnd(Args&&... args)
        {
            AllocateAndInsertNode(Forward(args)..., this->base.prev);
        }

        Void Splice(ConstIterator before_it, Chain& chain, ConstIterator begin, ConstIterator end) noexcept
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

        Void Clear() noexcept
        {
            Node* node = this->base.next->AsChainNode<T>();
            for(DeviceSize i = 0; i < this->size; i++)
            {
                Detail::ChainNodeBase* next = node->next;
                node->~Node();
                this->allocator.Deallocate(node);

                node = next->AsChainNode<T>();
            }

            this->size = 0;
            this->base = Detail::ChainNodeBase::SelfLinked(&this->base);
        }

        Void Erase(ConstIterator it) noexcept
        {
            EraseAndFreeNode(it.node);
        }

        Void EraseFirst() noexcept
        {
            EraseAndFreeNode(this->base.next);
        }

        Void EraseLast() noexcept
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

        Iterator GetIterator() noexcept
        {
            return Iterator(this->base.next);
        }

        ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(this->base.next);
        }

        Iterator GetSentinel() noexcept
        {
            return Iterator(&this->base);
        }

        ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(&this->base);
        }

        static MemoryRequirements GetMemoryRequirements(DeviceSize size) noexcept
        {
            return MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node) * size};
        }
    public:
        Void UpdateBase() noexcept //use on move when we must change first and last references to the base
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

        Void InsertNode(Detail::ChainNodeBase* prev_node,
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

        template<typename... Args>
        Void AllocateAndInsertNode(Args&&... args,
                                   Detail::ChainNodeBase* prev_node) //allocate node and insert right after prev_node
        {
            Node* node = static_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node(Forward(args)...);
            }
            catch(...)
            {
                this->allocator.Deallocate(node);
                throw;
            }

            InsertNode(prev_node, node);
        }

        Void EraseNode(Node* node) noexcept //erase node without dealloc
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

        Void EraseAndFreeNode(Node* node) noexcept //erase node with dealloc
        {
            EraseNode(node);

            node->~Node();
            this->allocator.Deallocate(node);
        }
    private:
        Detail::ChainNodeBase base;
        DeviceSize size;
        Allocator allocator;
    };

    template<Range R>
    Chain(R&& values) -> Chain<DropConstVolatileReference<RangeDereferenceType<R>>>;

    //std compat
    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, Chain>
    auto begin(T&& chain) noexcept
    {
        return Forward(chain).GetIterator();
    }

    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, Chain>
    auto end(T&& chain) noexcept
    {
        return Forward(chain).GetSentinel();
    }

    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, Chain>
    auto size(T&& chain) noexcept
    {
        return Forward(chain).GetSize();
    }
};