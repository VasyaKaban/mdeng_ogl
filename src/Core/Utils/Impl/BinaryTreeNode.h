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

    template<typename K, typename V>
    class BinaryTree;

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

            static BinaryTreeNodeBase SelfLinkedRoot(BinaryTreeNodeBase* root) noexcept;
        };

        CORE_API BinaryTreeNodeBase*& GetRootBeginIteratorNode(BinaryTreeNodeBase* root) noexcept;
        CORE_API BinaryTreeNodeBase* const& GetRootBeginIteratorNode(const BinaryTreeNodeBase* root) noexcept;
        CORE_API Void Balance(BinaryTreeNodeBase* node, BinaryTreeNodeBase* root) noexcept;
        CORE_API Void ApplyNodeInsert(BinaryTreeNodeBase* node, BinaryTreeNodeBase* root) noexcept;
        CORE_API Void DetachNode(BinaryTreeNodeBase* node, BinaryTreeNodeBase* root) noexcept;

        //we use const node but we can const_cast to drop constness
        CORE_API const BinaryTreeNodeBase* InOrderNodeForwardTraversal(const BinaryTreeNodeBase* node) noexcept;
        CORE_API const BinaryTreeNodeBase* InOrderNodeReverseTraversal(const BinaryTreeNodeBase* node) noexcept;

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
            template<typename OK, typename OV>
            friend class ::Core::BinaryTree;

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
                this->node = const_cast<NodeType>(InOrderNodeForwardTraversal(this->node));

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
                this->node = const_cast<NodeType>(InOrderNodeReverseTraversal(this->node));

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