#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"
#include "../../API.h"

namespace Core
{
    /*
          E|Root|
            / \
          L/   \R
          v     \
        |10|     \
        / \       \
      L/   \R      \
      v     v       \
    B|5|   |15|      \
    ^                /
    |_______________/

    */

    namespace Detail
    {
        struct CORE_API BinaryTreeNodeBase
        {
            BinaryTreeNodeBase* parent;
            BinaryTreeNodeBase* left;
            BinaryTreeNodeBase* right;
            UInt8 height;

            BinaryTreeNodeBase() noexcept;
            BinaryTreeNodeBase(BinaryTreeNodeBase* parent, BinaryTreeNodeBase* left, BinaryTreeNodeBase* right, UInt8 height) noexcept;

            BinaryTreeNodeBase*& GetRootBeginIteratorNode() noexcept;
            BinaryTreeNodeBase* const& GetRootBeginIteratorNode() const noexcept;

            Void Balance(BinaryTreeNodeBase* root) noexcept;
            Void Insert(BinaryTreeNodeBase* parent, BinaryTreeNodeBase*& parent_branch, BinaryTreeNodeBase* root) noexcept; //call it when you insert you node after all pointers linking
            Void Detach(BinaryTreeNodeBase* root) noexcept; //call it when you want to detach node

            Void InitRootNode(Bool is_empty_tree) noexcept;

            //we use const node but we can const_cast to drop constness
            const BinaryTreeNodeBase* InOrderNodeForwardTraversal() const noexcept;
            const BinaryTreeNodeBase* InOrderNodeReverseTraversal() const noexcept;
        };

        template<typename K, typename V>
        struct BinaryTreeNodeKeyValuePair
        {
            const K key;
            V value;
        };

        template<typename K, typename V>
        struct BinaryTreeNode : BinaryTreeNodeBase, BinaryTreeNodeKeyValuePair<K, V>
        {};

        template<typename K, typename V> //K is always const, but V may be const and non-const
        class BinaryTreeIterator
        {
            constexpr static Bool IsConstIterator = Const<V>;
            using NodeType = Conditional<IsConstIterator, const BinaryTreeNodeBase*, BinaryTreeNodeBase*>;
        public:
            explicit BinaryTreeIterator(BinaryTreeNodeBase* node) noexcept
                : node(node)
            {}

            explicit BinaryTreeIterator(const BinaryTreeNodeBase* node) noexcept
            requires IsConstIterator
                : node(node)
            {}

            BinaryTreeIterator() = default;
            ~BinaryTreeIterator() = default;
            BinaryTreeIterator(const BinaryTreeIterator&) = default;
            BinaryTreeIterator(BinaryTreeIterator&&) = default;
            BinaryTreeIterator& operator=(const BinaryTreeIterator&) = default;
            BinaryTreeIterator& operator=(BinaryTreeIterator&&) = default;

            BinaryTreeIterator operator++(int) noexcept
            {
                BinaryTreeIterator out(*this);

                ++(*this);

                return out;
            }

            BinaryTreeIterator& operator++() noexcept
            {
                this->node = const_cast<NodeType>(this->node->InOrderNodeForwardTraversal());

                return *this;
            }

            BinaryTreeIterator operator--(int) noexcept
            {
                BinaryTreeIterator out(*this);

                --(*this);

                return out;
            }

            BinaryTreeIterator& operator--() noexcept
            {
                this->node = const_cast<NodeType>(this->node->InOrderNodeReverseTraversal());

                return *this;
            }

            BinaryTreeNodeKeyValuePair<K, V>& operator*() const noexcept
            {
                return *static_cast<BinaryTreeNode<K, V>*>(this->node)->pair;
            }

            BinaryTreeNodeKeyValuePair<K, V>* operator->() const noexcept
            {
                return static_cast<BinaryTreeNode<K, V>*>(this->node)->pair;
            }

            template<typename OK, typename OV>
            Bool operator==(const BinaryTreeIterator<OK, OV>& it) const noexcept
            {
                return this->node == it.node;
            }

            NodeType GetNode() const noexcept
            {
                return this->node;
            }

            operator BinaryTreeIterator<K, const V>() const noexcept
            requires(!IsConstIterator)
            {
                return BinaryTreeIterator<K, const V>(this->node);
            }
        private:
            NodeType node;
        };
    };
};