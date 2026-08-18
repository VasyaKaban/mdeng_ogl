#pragma once

#include "Impl/ChainNode.h"
#include "Utility.hpp"
#include "Memory.h"
#include "Binary.hpp"
#include "Hash.hpp"

namespace Core
{
    template<typename K, typename V>
    struct HashChainNodeKeyValueHashTuple
    {
        const K key;
        V value;
        const DeviceSize hash;
    };

    struct HashChainMemoryRequirements
    {
        MemoryRequirements slots_requirements;
        MemoryRequirements objects_requirements;
        MemoryRequirements linear_requirements;
    };

    //if hash chain does not have slots then all insert command will fail and return end iterator
    template<typename K, typename V>
    class HashChain
    {
        static_assert(SameAs<K, DropConstVolatileReference<K>> && SameAs<V, DropConstVolatileReference<V>> && EqualComparable<const K&, const K&> && Hashable<K>);

        struct HashChainSlot
        {
            Detail::ChainNodeBase* first;
        };

        struct FindInsertParentNodeResult
        {
            Detail::ChainNodeBase* node;
            Bool update_slot;
            Bool success; //use it when using Unique methods. true -> inserted, false -> exist
        };
    public:
        using Iterator = Detail::ChainIterator<HashChainNodeKeyValueHashTuple<K, V>>;
        using ConstIterator = Detail::ChainIterator<const HashChainNodeKeyValueHashTuple<K, V>>;

        using Node = Detail::ChainNode<HashChainNodeKeyValueHashTuple<K, V>>;
        using ConstNode = Detail::ChainNode<const HashChainNodeKeyValueHashTuple<K, V>>;

        HashChain(Allocator allocator = GetGlobalAllocator())
            : base(),
              slots(nullptr),
              slot_count(0),
              size(0),
              allocator(allocator)
        {
            this->base.InitBaseNode(true);
        }

        HashChain(DeviceSize slot_count, Allocator allocator = GetGlobalAllocator())
            : HashChain(allocator)
        {
            if(slot_count != 0)
            {
                this->slots = reinterpret_cast<HashChainSlot*>(allocator.Allocate(MemoryRequirements{.alignment = alignof(HashChainSlot), .size = sizeof(HashChainSlot) * slot_count}));
                for(DeviceSize i = 0; i < slot_count; i++)
                    new(this->slots + i) HashChainSlot{.first = nullptr};
            }

            this->slot_count = slot_count;
        }

        ~HashChain()
        {
            Clear();

            for(DeviceSize i = 0; i < slot_count; i++)
                this->slots[i].~HashChainSlot();

            this->allocator.Deallocate(this->slots);
        }

        HashChain(const HashChain& chain)
        requires CopyConstructible<K> && CopyConstructible<V>
            : HashChain(chain.slot_count, chain.allocator)
        {
            for(const auto& [k, v, _]: chain)
                InsertMultiple(k, v);
        }

        HashChain(HashChain&& chain) noexcept
            : base(chain.base),
              slots(Exchange(chain.slots, nullptr)),
              slot_count(Exchange(chain.slot_count, 0)),
              size(Exchange(chain.size, 0)),
              allocator(chain.allocator)
        {
            this->base.InitBaseNode(this->size == 0);
            chain.base.InitBaseNode(true);
        }

        HashChain& operator=(const HashChain& tree)
        requires CopyConstructible<K> && CopyConstructible<V>
        {
            Clear();

            this->allocator = tree.allocator;

            for(const auto& [k, v, _]: tree)
                InsertMultiple(k, v);
        }

        HashChain& operator=(HashChain&& chain) noexcept
        {
            this->~HashChain();

            this->base = chain.base;
            this->slots = Exchange(chain.slots, nullptr);
            this->slot_count = Exchange(chain.slot_count, 0);
            this->size = Exchange(chain.size, 0);
            this->allocator = chain.allocator;

            this->base.InitBaseNode(this->size == 0);
            chain.base.InitBaseNode(true);

            return *this;
        }

        Bool IsSlotless() const noexcept
        {
            return this->slot_count == 0;
        }

        Bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        DeviceSize GetSlotCount() const noexcept
        {
            return this->slot_count;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename OK>
        requires EqualComparable<const K, OK> && Hashable<DropConstVolatileReference<OK>>
        Iterator Find(OK&& key) noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            DeviceSize hash = Hash(Forward(key));
            DeviceSize slot = hash % slot_count;

            if(this->slots[slot].first == nullptr)
                return GetSentinel();

            Iterator it = GetSlotIterator(slot);
            while(it != GetSentinel())
            {
                Node* node = static_cast<Node*>(it.GetNode());
                if(node->value.hash % slot_count != slot) //out of bounds
                {
                    it = GetSentinel();
                    break;
                }

                if(node->value.key == Forward(key))
                    break;
            }

            return it;
        }

        template<typename OK>
        requires EqualComparable<const K, OK> && Hashable<DropConstVolatileReference<OK>>
        ConstIterator Find(OK&& key) const noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            DeviceSize hash = Hash(Forward(key));
            DeviceSize slot = hash % slot_count;

            if(this->slots[slot].first == nullptr)
                return GetSentinel();

            ConstIterator it = GetSlotIterator(slot);
            while(it != GetSentinel())
            {
                ConstNode* node = static_cast<ConstNode*>(it.GetNode());
                if(node->value.hash % slot_count != slot) //out of bounds
                {
                    it = GetSentinel();
                    break;
                }

                if(node->value.key == Forward(key))
                    break;
            }

            return it;
        }

        Void Erase(ConstIterator it) noexcept
        {
            Detail::ChainNodeBase* node = const_cast<Detail::ChainNodeBase*>(it.GetNode());

            DetachChangeSlotFirstIterator(Iterator(node));
            node->Detach(&this->base);

            this->size--;

            static_cast<Node*>(node)->~Node();
            this->allocator.Deallocate(node);
        }

        Void Erase(ConstIterator it, ConstIterator sent) noexcept
        {
            for(; it != sent; it++)
                Erase(it);
        }

        template<typename OK, typename OV>
        requires Constructible<K, OK> && Constructible<V, OV> && EqualComparable<const K, OK> && Hashable<DropConstVolatileReference<OK>>
        InsertResult<Iterator> InsertUnique(OK&& key, OV&& value)
        {
            if(IsSlotless())
                return {.it = GetSentinel(), .inserted = false};

            DeviceSize hash = Hash(Forward(key));
            DeviceSize slot = hash % this->slot_count;

            auto [parent, update_slot, success] = FindInsertParentNode(Forward(key), hash, slot, true);
            if(!success)
                return {.it = Iterator(parent), .inserted = false};

            Node* node = AllocateAndInitNode(Forward(key), Forward(value), hash);

            if(update_slot)
                this->slots[slot].first = node;

            node->Insert(parent, &this->base);

            this->size++;

            return {.it = Iterator(node), .inserted = true};
        }

        template<typename OK, typename OV>
        requires Constructible<K, OK> && Constructible<V, OV> && Hashable<DropConstVolatileReference<OK>>
        Iterator InsertMultiple(OK&& key, OV&& value)
        {
            if(IsSlotless())
                return GetSentinel();

            DeviceSize hash = Hash(Forward(key));
            DeviceSize slot = hash % this->slot_count;

            auto [parent, update_slot, success] = FindInsertParentNode(Forward(key), hash, slot, false);

            Node* node = AllocateAndInitNode(Forward(key), Forward(value), hash);

            if(update_slot)
                this->slots[slot].first = node;

            node->Insert(parent, &this->base);

            this->size++;

            return Iterator(node);
        }

        Void Clear() noexcept
        {
            if(IsEmpty())
                return;

            Erase(GetIterator(), GetSentinel());
        }

        Node* Detach(ConstIterator it) noexcept
        {
            Detail::ChainNodeBase* node = const_cast<Detail::ChainNodeBase*>(it.GetNode());

            DetachChangeSlotFirstIterator(Iterator(node));
            node->Detach(&this->base);

            this->size--;

            return static_cast<Node*>(node);
        }

        InsertResult<Iterator> AttachUnique(Node* node) noexcept
        {
            if(IsSlotless())
                return {.it = GetSentinel(), .inserted = false};

            DeviceSize hash = node->value.hash;
            DeviceSize slot = hash % this->slot_count;

            auto [parent, update_slot, success] = FindInsertParentNode(Forward(node->value.key), hash, slot, true);
            if(!success)
                return {.it = Iterator(parent), .inserted = false};

            node->prev = nullptr;
            node->next = nullptr;

            if(update_slot)
                this->slots[slot].first = node;

            node->Insert(parent, &this->base);

            this->size++;

            return {.it = Iterator(node), .inserted = true};
        }

        Iterator AttachMultiple(Node* node) noexcept
        {
            if(IsSlotless())
                return GetSentinel();

            DeviceSize hash = node->value.hash;
            DeviceSize slot = hash % this->slot_count;

            auto [parent, update_slot, success] = FindInsertParentNode(Forward(node->value.key), hash, slot, false);

            node->prev = nullptr;
            node->next = nullptr;

            if(update_slot)
                this->slots[slot].first = node;

            node->Insert(parent, &this->base);

            this->size++;

            return Iterator(node);
        }

        Iterator GetSlotIterator(DeviceSize slot) noexcept
        {
            if(this->slots[slot].first == nullptr)
                return GetSentinel();

            return Iterator(this->slots[slot].first);
        }

        ConstIterator GetSlotIterator(DeviceSize slot) const noexcept
        {
            if(this->slots[slot].first == nullptr)
                return GetSentinel();

            return ConstIterator(this->slots[slot].first);
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

        static HashChainMemoryRequirements GetMemoryRequirements(DeviceSize slot_count, DeviceSize object_count) noexcept
        {
            HashChainMemoryRequirements req;
            req.slots_requirements = MemoryRequirements{.alignment = alignof(HashChainSlot), .size = sizeof(HashChainSlot) * slot_count};
            req.objects_requirements = MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node) * object_count};
            req.linear_requirements = MemoryRequirements{.alignment = Max(alignof(HashChainSlot), alignof(Node)), .size = req.slots_requirements.size + req.objects_requirements.size};
            Align(req.linear_requirements.size, req.linear_requirements.alignment);

            return req;
        }
    private:
        template<typename OK, typename OV>
        Node* AllocateAndInitNode(OK&& key, OV&& value, DeviceSize hash)
        {
            Node* node = reinterpret_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node(Detail::ChainNodeBase(), HashChainNodeKeyValueHashTuple<K, V>(Forward(key), Forward(value), hash));
            }
            catch(...)
            {
                allocator.Deallocate(node);
                throw;
            }

            return node;
        }

        Void DetachChangeSlotFirstIterator(Iterator node_it) noexcept
        {
            Node* node = static_cast<Node*>(node_it.GetNode());
            DeviceSize slot = node->value.hash % slot_count;

            auto slot_iter = GetSlotIterator(slot);
            if(slot_iter == node_it) //go to the next
            {
                auto next_slot_iter = slot_iter++;
                if(next_slot_iter == GetSentinel()) //set nullptr
                {
                    this->slots[slot].first = nullptr;
                }
                else //update or set nullptr
                {
                    Node* next_iter_node = static_cast<Node*>(next_slot_iter.GetNode());
                    if(next_iter_node->value.hash % slot_count == slot) //use as first
                    {
                        this->slots[slot].first = next_iter_node;
                    }
                    else //next iter is from other slot -> set nullptr
                    {
                        this->slots[slot].first = nullptr;
                    }
                }
            }
        }

        template<typename OK>
        FindInsertParentNodeResult FindInsertParentNode(OK&& key, DeviceSize hash, DeviceSize slot, Bool is_unqiue_policy)
        {
            auto slot_it = GetSlotIterator(slot);

            Detail::ChainNodeBase* parent = nullptr;
            Bool update_slot = false;

            if(slot_it == GetSentinel()) //empty
            {
                parent = &this->base;
                update_slot = true;
            }
            else if(!is_unqiue_policy) //in non-unqiue policy insert as first
            {
                parent = slot_it.GetNode();
                update_slot = true;
            }
            else
            {
                parent = slot_it.GetNode();

                ++slot_it;
                for(; slot_it != GetSentinel(); slot_it++)
                {
                    Node* node = static_cast<Node*>(slot_it.GetNode());

                    if(node->value.hash % this->slot_count != slot)
                        break;

                    if(node->value.hash == hash && node->value.key == Forward(key))
                        return FindInsertParentNodeResult{.node = slot_it.GetNode(), .update_slot = false, .success = false};
                }
            }

            return FindInsertParentNodeResult{.node = parent, .update_slot = update_slot, .success = true};
        }
    private:
        Detail::ChainNodeBase base;
        HashChainSlot* slots;
        DeviceSize slot_count;
        DeviceSize size;
        Allocator allocator;
    };

    //std compat
    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, HashChain>
    auto begin(T&& chain) noexcept
    {
        return Forward(chain).GetIterator();
    }

    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, HashChain>
    auto end(T&& chain) noexcept
    {
        return Forward(chain).GetSentinel();
    }
};