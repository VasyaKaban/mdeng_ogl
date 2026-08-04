#pragma once

#include "Impl/BinaryTreeNode.h"
#include "Memory.h"

namespace Core
{
    template<typename K, typename V>
    struct BinaryTreeInsertResult
    {
        Detail::BinaryTreeIterator<K, V> it;
        Bool inserted;
    };

    //for policy(unique or multiple) use separate functions
    template<typename K, typename V, typename C>
    class BinaryTree
    {
        static_assert(SameAs<K, DropConstVolatileReference<K>> && SameAs<V, DropConstVolatileReference<K>> && NoexceptInvocable<C, const K&, const K&> &&
                      CompareResultLessComparable<InvokeResult<C, const K&, const K&>> && CompareResultEqualComparable<InvokeResult<C, const K&, const K&>> &&
                      CompareResultGreaterComparable<InvokeResult<C, const K&, const K&>>);
    public:
        using Iterator = Detail::BinaryTreeIterator<K, V>;
        using ConstIterator = Detail::BinaryTreeIterator<K, const V>;

        using Node = Detail::BinaryTreeNode<K, V>;
        using ConstNode = Detail::BinaryTreeNode<K, const V>;

        ////////////////////////////////////////////////////////////////////////////
        BinaryTree(Allocator allocator = GetGlobalAllocator()) noexcept(NoexceptDefaultConstructible<C>)
        requires DefaultConstructible<C>;

        BinaryTree(const C& comparator, Allocator allocator = GetGlobalAllocator()) noexcept(NoexceptCopyConstructible<C>)
        requires CopyConstructible<C>;

        BinaryTree(C&& comparator, Allocator allocator = GetGlobalAllocator()) noexcept(NoexceptMoveConstructible<C>)
        requires MoveConstructible<C>;

        ~BinaryTree();

        BinaryTree(const BinaryTree& tree)
        requires CopyConstructible<C> && CopyConstructible<K> && CopyConstructible<V>;

        BinaryTree(BinaryTree&& tree) noexcept(NoexceptMoveConstructible<C>)
        requires MoveConstructible<C>;

        BinaryTree& operator=(const BinaryTree& tree)
        requires CopyAssignable<C> && CopyConstructible<K> && CopyConstructible<V>;

        BinaryTree& operator=(BinaryTree&& tree) noexcept(NoexceptMoveAssignable<C>)
        requires MoveAssignable<C>;
        ////////////////////////////////////////////////////////////////////////////

        Bool IsEmpty() const noexcept
        {
            return this->size == 0;
        }

        DeviceSize GetSize() const noexcept
        {
            return this->size;
        }

        C& GetComparator() noexcept
        {
            return this->comparator;
        }

        const C& GetComparator() const noexcept
        {
            return this->comparator;
        }

        Allocator GetAllocator() const noexcept
        {
            return this->allocator;
        }

        template<typename OK>
        requires CompareResultEqualComparable<InvokeResult<C, const K&, OK>> && CompareResultLessComparable<InvokeResult<C, const K&, OK>>
        Iterator Find(OK&& key) noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            Node* node = this->base.left;
            while(node != nullptr)
            {
                auto cmp_res = comparator(reinterpret_cast<Node*>(node)->pair.key, Forward(key));
                if(cmp_res == 0)
                    return Iterator(node);
                else if(cmp_res < 0)
                    node = node->right;
                else
                    node = node->left;
            }

            return GetSentinel();
        }

        template<typename OK>
        requires CompareResultEqualComparable<InvokeResult<C, const K&, OK>> && CompareResultLessComparable<InvokeResult<C, const K&, OK>>
        ConstIterator Find(OK&& key) const noexcept
        {
            if(IsEmpty())
                return GetSentinel();

            Node* node = this->base.left;
            while(node != nullptr)
            {
                auto cmp_res = comparator(reinterpret_cast<ConstNode*>(node)->pair.key, Forward(key));
                if(cmp_res == 0)
                    return ConstIterator(node);
                else if(cmp_res < 0)
                    node = node->right;
                else
                    node = node->left;
            }

            return GetSentinel();
        }

        Void Erase(ConstIterator it) noexcept
        {
            Node* node = const_cast<Node*>(it.node);
            Detail::DetachNode(node, &base);

            this->size--;

            node->~Node();
            this->allocator.Deallocate(node);
        }

        Void Erase(ConstIterator it, ConstIterator sent) noexcept
        {
            for(; it != sent; it++)
                Erase(it);
        }

        ////////////////////////////////////////////////////////////////////////////
        template<typename OK, typename OV>
        BinaryTreeInsertResult<K, V> InsertUnique(OK&& key, OV&& value);

        template<typename OK, typename OV>
        BinaryTreeInsertResult<K, V> InsertMultiple(OK&& key, OV&& value);
        ////////////////////////////////////////////////////////////////////////////

        Void Clear() noexcept
        {
            if(IsEmpty())
                return;

            Erase(GetIterator(), GetSentinel());
        }

        ////////////////////////////////////////////////////////////////////////////
        Node* Detach(ConstIterator it) noexcept;

        Iterator Attach(Node* node) noexcept;
        ////////////////////////////////////////////////////////////////////////////

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
            return Iterator(this->base);
        }

        ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->base);
        }

        static MemoryRequirements GetMemoryRequirements(DeviceSize reserve) noexcept
        {
            return MemoryRequirements{.alignment = alignof(Node), .size = sizeof(Node) * reserve};
        }
    private:
        Detail::BinaryTreeNodeBase base; //end iterator + base.right == begin
        DeviceSize size;
        C comparator;
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

#error DO NOT FORGET THAT base HOLDS ITERATORS SO WE NEED TO RELINK base ON MOVE!!!
};