#pragma once

#include "Impl/BinaryTreeNode.h"
#include "Memory.h"
#include "Utility.hpp"

namespace Core
{
    //for policy(unique or multiple) use separate functions
    template<typename K, typename V>
    class BinaryTree
    {
        static_assert(SameAs<K, DropConstVolatileReference<K>> && SameAs<V, DropConstVolatileReference<V>> && LessComparable<const K&, const K&> && EqualComparable<const K&, const K&> &&
                      GreaterComparable<const K&, const K&>);

        struct FindInsertParentNodeResult
        {
            Detail::BinaryTreeNodeBase* node;
            Bool is_left_branch;
            Bool success; //use it when using Unique methods. true -> inserted, false -> exist
        };
    public:
        using Iterator = Detail::BinaryTreeIterator<K, V>;
        using ConstIterator = Detail::BinaryTreeIterator<K, const V>;

        using Node = Detail::BinaryTreeNode<K, V>;
        using ConstNode = Detail::BinaryTreeNode<K, const V>;

        BinaryTree(SharedPointer<Allocator> allocator = GetGlobalAllocator())
            : base(),
              size(0),
              allocator(allocator)
        {
            this->base.InitRootNode(true);
        }

        ~BinaryTree()
        {
            Clear();
        }

        BinaryTree(const BinaryTree& tree)
        requires CopyConstructible<K> && CopyConstructible<V>
            : BinaryTree(tree.allocator)
        {
            for(const auto& [k, v]: tree)
                InsertMultiple(k, v);
        }

        BinaryTree(BinaryTree&& tree) noexcept
            : base(tree.base),
              size(Exchange(tree.size, 0)),
              allocator(tree.allocator)
        {
            this->base.InitRootNode(this->size == 0);
            tree.base.InitRootNode(true);
        }

        BinaryTree& operator=(const BinaryTree& tree)
        requires CopyConstructible<K> && CopyConstructible<V>
        {
            Clear();

            this->allocator = tree.allocator;

            for(const auto& [k, v]: tree)
                InsertMultiple(k, v);
        }

        BinaryTree& operator=(BinaryTree&& tree) noexcept
        {
            Clear();

            this->base = tree.base;
            this->size = Exchange(tree.size, 0);
            this->allocator = tree.allocator;

            this->base.InitRootNode(this->size == 0);
            tree.base.InitRootNode(true);

            return *this;
        }

        Bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        SharedPointer<Allocator> GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename OK>
        requires EqualComparable<const K&, OK> && LessComparable<const K&, OK>
        Iterator Find(OK&& key) noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            Detail::BinaryTreeNodeBase* node = this->base.left;
            while(node != nullptr)
            {
                const K& cmp_node_key = static_cast<Node*>(node)->key;

                if(cmp_node_key == Forward(key))
                    return Iterator(node);
                else if(cmp_node_key < Forward(key))
                    node = node->right;
                else
                    node = node->left;
            }

            return GetSentinel();
        }

        template<typename OK>
        requires EqualComparable<const K&, OK> && LessComparable<const K&, OK>
        ConstIterator Find(OK&& key) const noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            Detail::BinaryTreeNodeBase* node = this->base.left;
            while(node != nullptr)
            {
                const K& cmp_node_key = static_cast<ConstNode*>(node)->key;

                if(cmp_node_key == Forward(key))
                    return ConstIterator(node);
                else if(cmp_node_key < Forward(key))
                    node = node->right;
                else
                    node = node->left;
            }

            return GetSentinel();
        }

        Void Erase(ConstIterator it) noexcept
        {
            Detail::BinaryTreeNodeBase* node = const_cast<Detail::BinaryTreeNodeBase*>(it.GetNode());
            node->Detach(&this->base);

            this->size--;

            static_cast<Node*>(node)->~Node();
            this->allocator->Deallocate(node);
        }

        Void Erase(ConstIterator it, ConstIterator sent) noexcept
        {
            for(; it != sent; it++)
                Erase(it);
        }

        template<typename OK, typename OV>
        requires Constructible<K, OK> && Constructible<V, OV> && EqualComparable<const K&, OK> && LessComparable<const K&, OK>
        InsertResult<Iterator> InsertUnique(OK&& key, OV&& value)
        {
            auto [parent, is_left_branch, success] = FindInsertParentNode(Forward(key), true);
            if(!success)
                return {.it = Iterator(parent), .inserted = false};

            Node* node = AllocateAndInitNode(Forward(key), Forward(value));

            node->Insert(parent, (is_left_branch ? parent->left : parent->right), &this->base);
            this->size++;

            return {.it = Iterator(node), .inserted = true};
        }

        template<typename OK, typename OV>
        requires Constructible<K, OK> && Constructible<V, OV> && LessComparable<const K, OK>
        Iterator InsertMultiple(OK&& key, OV&& value)
        {
            auto [parent, is_left_branch, success] = FindInsertParentNode(Forward(key), false);

            Node* node = AllocateAndInitNode(Forward(key), Forward(value));

            node->Insert(parent, (is_left_branch ? parent->left : parent->right), &this->base);
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
            Detail::BinaryTreeNodeBase* node = const_cast<Detail::BinaryTreeNodeBase*>(it.GetNode());
            node->Detach(&this->base);

            this->size--;

            return static_cast<Node*>(node);
        }

        InsertResult<Iterator> AttachUnique(Node* node) noexcept
        {
            auto [parent, is_left_branch, success] = FindInsertParentNode(node->key, true);
            if(!success)
                return {.it = Iterator(parent), .inserted = false};

            node->left = nullptr;
            node->right = nullptr;
            node->height = 1;

            node->Insert(parent, (is_left_branch ? parent->left : parent->right), &this->base);
            this->size++;

            return {.it = Iterator(node), .inserted = true};
        }

        Iterator AttachMultiple(Node* node) noexcept
        {
            auto [parent, is_left_branch, success] = FindInsertParentNode(node->key, true);

            node->left = nullptr;
            node->right = nullptr;
            node->height = 1;

            node->Insert(parent, (is_left_branch ? parent->left : parent->right), &this->base);
            this->size++;

            return Iterator(node);
        }

        Iterator GetIterator() noexcept
        {
            return Iterator(this->base.GetRootBeginIteratorNode());
        }

        ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(this->base.GetRootBeginIteratorNode());
        }

        Iterator GetSentinel() noexcept
        {
            return Iterator(&this->base);
        }

        ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(&this->base);
        }

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node) * reserve};
        }
    private:
        template<typename OK, typename OV>
        Node* AllocateAndInitNode(OK&& key, OV&& value)
        {
            Node* node = reinterpret_cast<Node*>(this->allocator->Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node(Detail::BinaryTreeNodeBase(), Detail::BinaryTreeNodeKeyValuePair<K, V>(Forward(key), Forward(value)));
            }
            catch(...)
            {
                allocator->Deallocate(node);
                throw;
            }

            return node;
        }

        template<typename OK>
        FindInsertParentNodeResult FindInsertParentNode(OK&& key, Bool is_unqiue_policy)
        {
            Detail::BinaryTreeNodeBase* parent = nullptr;
            Bool is_left_branch = false;

            if(IsEmpty())
            {
                parent = &this->base;
                is_left_branch = true;
            }
            else
            {
                Detail::BinaryTreeNodeBase* cmp_node = this->base.left;
                while(true)
                {
                    const K& cmp_node_key = static_cast<Node*>(cmp_node)->key;

                    if(is_unqiue_policy && cmp_node_key == Forward(key))
                        return FindInsertParentNodeResult{.node = cmp_node_key, .is_left_branch = false, .success = false};
                    else if(cmp_node_key < Forward(key))
                    {
                        if(cmp_node->right != nullptr)
                            cmp_node = cmp_node->right;
                        else
                        {
                            parent = cmp_node;
                            is_left_branch = false;
                        }
                    }
                    else
                    {
                        if(cmp_node->left != nullptr)
                            cmp_node = cmp_node->left;
                        else
                        {
                            parent = cmp_node;
                            is_left_branch = true;
                            break;
                        }
                    }
                }
            }

            return FindInsertParentNodeResult{.node = parent, .is_left_branch = is_left_branch, .success = true};
        }
    private:
        Detail::BinaryTreeNodeBase base; //end iterator + base.right == begin
        DeviceSize size;
        SharedPointer<Allocator> allocator;
    };

    //std compat
    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, BinaryTree>
    auto begin(T&& tree) noexcept
    {
        return Forward(tree).GetIterator();
    }

    template<typename T>
    requires TypeInstantiation<DropConstVolatileReference<T>, BinaryTree>
    auto end(T&& tree) noexcept
    {
        return Forward(tree).GetSentinel();
    }
};