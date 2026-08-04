#pragma once

#include "Impl/BinaryTreeNode.h"
#include "Memory.h"
#include "Utility.hpp"

namespace Core
{
    template<typename K, typename V>
    struct BinaryTreeInsertResult
    {
        Detail::BinaryTreeIterator<K, V> it;
        Bool inserted;
    };

    //for policy(unique or multiple) use separate functions
    template<typename K, typename V>
    class BinaryTree
    {
        static_assert(SameAs<K, DropConstVolatileReference<K>> && SameAs<V, DropConstVolatileReference<V>> && LessComparable<const K, const K> && EqualComparable<const K, const K> &&
                      GreaterComparable<const K, const K>);
    public:
        using Iterator = Detail::BinaryTreeIterator<K, V>;
        using ConstIterator = Detail::BinaryTreeIterator<K, const V>;

        using Node = Detail::BinaryTreeNode<K, V>;
        using ConstNode = Detail::BinaryTreeNode<K, const V>;

        BinaryTree(Allocator allocator = GetGlobalAllocator())
            : base(Detail::BinaryTreeNodeBase::SelfLinkedRoot(&this->base)),
              size(0),
              allocator(allocator)
        {}

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
            : base(Exchange(tree.base, Node::SelfLinkedRoot(&tree.base))),
              size(Exchange(tree.size, 0)),
              allocator(tree.allocator)
        {}

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

            this->base = Exchange(tree.base, Node::SelfLinkedRoot(&tree.base));
            this->size = Exchange(tree.size, 0);
            this->allocator = tree.allocator;

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

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename OK>
        requires EqualComparable<const K, OK> && LessComparable<const K, OK>
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
        requires EqualComparable<const K, OK> && LessComparable<const K, OK>
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
            Detail::BinaryTreeNodeBase* node = const_cast<Detail::BinaryTreeNodeBase*>(it.node);
            Detail::DetachNode(node, &base);

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
        requires Constructible<K, OK> && Constructible<V, OV> && EqualComparable<const K, OK> && LessComparable<const K, OK>
        BinaryTreeInsertResult<K, V> InsertUnique(OK&& key, OV&& value)
        {
            Node* node = nullptr;

            if(IsEmpty())
            {
                //allocate and create node
                node = AllocateAndInitNode(Forward(key), Forward(value));

                this->base.left = node;
                node->parent = &this->base;
            }
            else
            {
                Detail::BinaryTreeNodeBase* cmp_node = this->base.left;
                while(true)
                {
                    const K& cmp_node_key = static_cast<Node*>(cmp_node)->key;

                    if(cmp_node_key == Forward(key))
                        return {.it = Iterator(cmp_node), .inserted = false};
                    else if(cmp_node_key < Forward(key))
                    {
                        if(cmp_node->right != nullptr)
                            cmp_node = cmp_node->right;
                        else
                        {
                            node = AllocateAndInitNode(Forward(key), Forward(value));

                            cmp_node->right = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                    else
                    {
                        if(cmp_node->left != nullptr)
                            cmp_node = cmp_node->left;
                        else
                        {
                            node = AllocateAndInitNode(Forward(key), Forward(value));

                            cmp_node->left = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                }
            }

            Detail::ApplyNodeInsert(node, &this->base);

            return {.it = Iterator(node), .inserted = true};
        }

        template<typename OK, typename OV>
        requires Constructible<K, OK> && Constructible<V, OV> && LessComparable<const K, OK>
        Iterator InsertMultiple(OK&& key, OV&& value)
        {
            Node* node = nullptr;

            if(IsEmpty())
            {
                //allocate and create node
                node = AllocateAndInitNode(Forward(key), Forward(value));

                this->base.left = node;
                node->parent = &this->base;
            }
            else
            {
                Detail::BinaryTreeNodeBase* cmp_node = this->base.left;
                while(true)
                {
                    const K& cmp_node_key = static_cast<Node*>(cmp_node)->key;

                    if(cmp_node_key < Forward(key))
                    {
                        if(cmp_node->right != nullptr)
                            cmp_node = cmp_node->right;
                        else
                        {
                            node = AllocateAndInitNode(Forward(key), Forward(value));

                            cmp_node->right = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                    else
                    {
                        if(cmp_node->left != nullptr)
                            cmp_node = cmp_node->left;
                        else
                        {
                            node = AllocateAndInitNode(Forward(key), Forward(value));

                            cmp_node->left = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                }
            }

            Detail::ApplyNodeInsert(node, &this->base);

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
            Detail::BinaryTreeNodeBase* node = const_cast<Detail::BinaryTreeNodeBase*>(it.node);
            Detail::DetachNode(node, &this->base);

            this->size--;

            return static_cast<Node*>(node);
        }

        BinaryTreeInsertResult<K, V> AttachUnique(Node* node) noexcept
        {
            node->left = nullptr;
            node->right = nullptr;
            node->height = 1;

            if(IsEmpty())
            {
                this->base.left = node;
                node->parent = &this->base;
            }
            else
            {
                Detail::BinaryTreeNodeBase* cmp_node = this->base.left;
                while(true)
                {
                    const K& cmp_node_key = static_cast<Node*>(cmp_node)->key;

                    if(cmp_node_key == node->key)
                        return {.it = Iterator(cmp_node), .inserted = false};
                    else if(cmp_node_key < node->key)
                    {
                        if(cmp_node->right != nullptr)
                            cmp_node = cmp_node->right;
                        else
                        {
                            cmp_node->right = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                    else
                    {
                        if(cmp_node->left != nullptr)
                            cmp_node = cmp_node->left;
                        else
                        {
                            cmp_node->left = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                }
            }

            Detail::ApplyNodeInsert(node, &this->base);

            return {.it = Iterator(node), .inserted = true};
        }

        Iterator AttachMultiple(Node* node) noexcept
        {
            node->left = nullptr;
            node->right = nullptr;
            node->height = 1;

            if(IsEmpty())
            {
                this->base.left = node;
                node->parent = &this->base;
            }
            else
            {
                Detail::BinaryTreeNodeBase* cmp_node = this->base.left;
                while(true)
                {
                    const K& cmp_node_key = static_cast<Node*>(cmp_node)->key;
                    if(cmp_node_key < node->key)
                    {
                        if(cmp_node->right != nullptr)
                            cmp_node = cmp_node->right;
                        else
                        {
                            cmp_node->right = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                    else
                    {
                        if(cmp_node->left != nullptr)
                            cmp_node = cmp_node->left;
                        else
                        {
                            cmp_node->left = node;
                            node->parent = cmp_node;
                            break;
                        }
                    }
                }
            }

            Detail::ApplyNodeInsert(node, &this->base);

            return Iterator(node);
        }

        Iterator GetIterator() noexcept
        {
            return Iterator(Detail::GetRootBeginIteratorNode(&this->base));
        }

        ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(Detail::GetRootBeginIteratorNode(&this->base));
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
        Void UpdateBase() noexcept
        {
            if(this->size == 0)
            {
                this->base = Detail::BinaryTreeNodeBase::SelfLinkedRoot(&this->base);
            }
            else
            {
                this->base.left->parent = &this->base;
                Detail::GetRootBeginIteratorNode(&this->base)->parent = &this->base;
            }
        }

        template<typename OK, typename OV>
        Node* AllocateAndInitNode(OK&& key, OV&& value)
        {
            Node* node = reinterpret_cast<Node*>(this->allocator.Allocate(MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node)}));

            try
            {
                new(node) Node{Detail::BinaryTreeNodeBase(), Detail::BinaryTreeNodeKeyValuePair<K, V>(Forward(key), Forward(value))};
            }
            catch(...)
            {
                allocator.Deallocate(node);
                throw;
            }

            return node;
        }
    private:
        Detail::BinaryTreeNodeBase base; //end iterator + base.right == begin
        DeviceSize size;
        Allocator allocator;
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