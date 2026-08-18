#pragma once

#include "Memory.h"
#include "RangeTraits.hpp"
#include "Utility.hpp"
#include "Impl/ChainNode.h"
#include "Ranges.hpp"

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
            : base(),
              size(0),
              allocator(allocator)
        {
            this->base.InitBaseNode(true);
        }

        ~Chain()
        {
            Clear();
        }

        Chain(const Chain& chain)
            : base(),
              size(0),
              allocator(chain.allocator)
        {
            this->base.InitBaseNode(true);

            for(const auto& value: chain)
                PushToEnd(value);
        }

        Chain(Chain&& chain) noexcept
            : base(chain.base),
              size(Exchange(chain.size, 0)),
              allocator(chain.allocator)
        {
            this->base.InitBaseNode(false);
            chain.base.InitBaseNode(true);
        }

        Chain& operator=(const Chain& chain)
        {
            Clear();

            this->allocator = chain.allocator;

            for(const auto& value: chain)
                PushToEnd(value);

            return *this;
        }

        Chain& operator=(Chain&& chain) noexcept
        {
            Clear();

            this->base = chain.base;
            this->size = Exchange(chain.size, 0);
            this->allocator = chain.allocator;

            this->base.InitBaseNode(false);
            chain.base.InitBaseNode(true);

            return *this;
        }

        template<Range R>
        Chain(R&& values, Allocator allocator = GetGlobalAllocator())
            : base(),
              size(0),
              allocator(allocator)
        {
            base.InitBaseNode(true);

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
            : base(),
              size(0),
              allocator(allocator)
        {
            base.InitBaseNode(true);

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
            : base(),
              size(0),
              allocator(allocator)
        {
            base.InitBaseNode(true);

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
            Detail::ChainNodeBase* before_node = const_cast<Detail::ChainNodeBase*>(before_it.GetNode());

            Node* node = AllocateAndInitNode(Forward(args)...);

            node->Insert(before_node, &this->base);

            this->size++;
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToBegin(Args&&... args)
        {
            Insert(GetSentinel(), Forward(args)...);
        }

        template<typename... Args>
        requires Constructible<T, Args...>
        Void PushToEnd(Args&&... args)
        {
            Insert(AdvanceBack(GetSentinel(), 1), Forward(args)...);
        }

        Void Clear() noexcept
        {
            for(auto it = GetIterator(); it != GetSentinel(); it++)
                Erase(it);
        }

        Void Erase(ConstIterator it) noexcept
        {
            Detail::ChainNodeBase* node = const_cast<Detail::ChainNodeBase*>(it.GetNode());

            node->Detach(&this->base);
            static_cast<Node*>(node)->~Node();
            this->allocator.Deallocate(node);

            this->size--;
        }

        Void EraseFirst() noexcept
        {
            Erase(GetIterator());
        }

        Void EraseLast() noexcept
        {
            Erase(AdvanceBack(GetSentinel(), 1));
        }

        T& GetFirst() noexcept
        {
            return *GetIterator();
        }

        const T& GetFirst() const noexcept
        {
            return *GetIterator();
        }

        T& GetLast() noexcept
        {
            return *AdvanceBack(GetSentinel(), 1);
        }

        const T& GetLast() const noexcept
        {
            return *AdvanceBack(GetSentinel(), 1);
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
        template<typename... Args>
        Node* AllocateAndInitNode(Args&&... args) //allocate node and insert right after prev_node
        {
            Node* node = static_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node(Detail::ChainNodeBase(), Forward(args)...);
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