#pragma once

#include "Memory.h"
#include "RangeTraits.hpp"
#include "Utility.hpp"
#include "Impl/ChainNode.h"

namespace Core
{
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

        template<typename U, DeviceSize N>
        requires Constructible<T, const U&>
        Chain(const U (&init_list)[N], Allocator allocator = GetGlobalAllocator())
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {
            try
            {
                for(const auto& value: init_list)
                    PushToEnd(value);
            }
            catch(...)
            {
                Clear();
                throw;
            }
        }

        template<typename U, DeviceSize N>
        requires Constructible<T, U&&>
        Chain(U (&&init_list)[N], Allocator allocator = GetGlobalAllocator())
            : base(Detail::ChainNodeBase::SelfLinked(&this->base)),
              size(0),
              allocator(allocator)
        {
            try
            {
                for(auto&& value: Forward(init_list))
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
            AllocateAndInsertNode(before_it.node, Forward(args)...);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToBegin(Args&&... args)
        {
            AllocateAndInsertNode(&this->base, Forward(args)...);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToEnd(Args&&... args)
        {
            AllocateAndInsertNode(this->base.prev, Forward(args)...);
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
        template<typename OT>
        Void AllocateAndInitNode(OT&& value) //allocate node and insert right after prev_node
        {
            Node* node = static_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node{Detail::ChainNodeBase(), Forward(value)};
            }
            catch(...)
            {
                this->allocator.Deallocate(node);
                throw;
            }

            return node;
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