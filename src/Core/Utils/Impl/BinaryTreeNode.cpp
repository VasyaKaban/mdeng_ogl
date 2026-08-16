#include "BinaryTreeNode.h"
#include "../Utility.hpp"

namespace Core
{
    namespace Detail
    {
        BinaryTreeNodeBase::BinaryTreeNodeBase() noexcept
            : parent(nullptr),
              left(nullptr),
              right(nullptr),
              height(0)
        {}

        BinaryTreeNodeBase::BinaryTreeNodeBase(BinaryTreeNodeBase* parent, BinaryTreeNodeBase* left, BinaryTreeNodeBase* right, UInt8 height) noexcept
            : parent(parent),
              left(left),
              right(right),
              height(height)
        {}

        enum class RotationType
        {
            None,
            SmallLeft,
            BigLeft,
            SmallRight,
            BigRight
        };

        static UInt8 GetNodeHeight(BinaryTreeNodeBase* node) noexcept
        {
            if(node == nullptr)
                return 0;

            return node->height;
        }

        static RotationType SelectRotationType(BinaryTreeNodeBase* node) noexcept
        {
            UInt8 left_height = GetNodeHeight(node->left);
            UInt8 right_height = GetNodeHeight(node->right);

            if(left_height > right_height) //choose right or none
            {
                if(left_height - right_height < 2)
                    return RotationType::None;

                BinaryTreeNodeBase* a = node;
                BinaryTreeNodeBase* R = a->right;
                BinaryTreeNodeBase* b = a->left;
                BinaryTreeNodeBase* L = b->left;
                BinaryTreeNodeBase* C = b->right;

                if(GetNodeHeight(C) <= GetNodeHeight(L))
                    return RotationType::SmallRight;
                else
                    return RotationType::BigRight;
            }
            else if(right_height > left_height) //choose left or none
            {
                if(right_height - left_height < 2)
                    return RotationType::None;

                BinaryTreeNodeBase* a = node;
                BinaryTreeNodeBase* L = a->left;
                BinaryTreeNodeBase* b = a->right;
                BinaryTreeNodeBase* C = b->left;
                BinaryTreeNodeBase* R = b->right;

                if(GetNodeHeight(C) <= GetNodeHeight(R))
                    return RotationType::SmallLeft;
                else
                    return RotationType::BigLeft;
            }
            else
                return RotationType::None;
        }

        static Void UpdateNodeHeight(BinaryTreeNodeBase* node) noexcept
        {
            UInt8 left_height = 0;
            UInt8 right_height = 0;

            if(node->left != nullptr)
                left_height = node->left->height;

            if(node->right != nullptr)
                right_height = node->right->height;

            node->height = Max(left_height, right_height) + 1;
        }

        //return node that is parent of new root based on node
        static BinaryTreeNodeBase* RotateNone(BinaryTreeNodeBase* node) noexcept
        {
            UpdateNodeHeight(node);

            return node->parent;
        }

        static BinaryTreeNodeBase* RotateSmallLeft(BinaryTreeNodeBase* node) noexcept
        {
            BinaryTreeNodeBase* a = node;
            BinaryTreeNodeBase* b = a->right;
            BinaryTreeNodeBase* L = a->left;
            BinaryTreeNodeBase* C = b->left;
            BinaryTreeNodeBase* R = b->right;

            BinaryTreeNodeBase* next_node = a->parent;

            a->right = C;
            if(C != nullptr)
                C->parent = a;

            b->left = a;
            a->parent = b;

            b->parent = next_node;

            UpdateNodeHeight(a);
            UpdateNodeHeight(b);

            return next_node;
        }

        static BinaryTreeNodeBase* RotateBigLeft(BinaryTreeNodeBase* node) noexcept
        {
            BinaryTreeNodeBase* a = node;
            BinaryTreeNodeBase* L = a->left;
            BinaryTreeNodeBase* b = a->right;
            BinaryTreeNodeBase* c = b->left;
            BinaryTreeNodeBase* R = b->right;
            BinaryTreeNodeBase* M = c->left;
            BinaryTreeNodeBase* N = c->right;

            BinaryTreeNodeBase* next_node = a->parent;

            c->left = a;
            a->parent = c;

            c->right = b;
            b->parent = c;

            a->right = M;
            if(M != nullptr)
                M->parent = a;

            b->left = N;
            if(N != nullptr)
                N->parent = b;

            c->parent = next_node;

            UpdateNodeHeight(a);
            UpdateNodeHeight(b);
            UpdateNodeHeight(c);

            return next_node;
        }

        static BinaryTreeNodeBase* RotateSmallRight(BinaryTreeNodeBase* node) noexcept
        {
            BinaryTreeNodeBase* a = node;
            BinaryTreeNodeBase* b = a->left;
            BinaryTreeNodeBase* R = a->right;
            BinaryTreeNodeBase* L = b->left;
            BinaryTreeNodeBase* C = b->right;

            BinaryTreeNodeBase* next_node = a->parent;

            a->left = C;
            if(C != nullptr)
                C->parent = a;

            b->right = a;
            a->parent = b;

            b->parent = next_node;

            UpdateNodeHeight(a);
            UpdateNodeHeight(b);

            return next_node;
        }

        static BinaryTreeNodeBase* RotateBigRight(BinaryTreeNodeBase* node) noexcept
        {
            BinaryTreeNodeBase* a = node;
            BinaryTreeNodeBase* b = a->left;
            BinaryTreeNodeBase* R = a->right;
            BinaryTreeNodeBase* L = b->left;
            BinaryTreeNodeBase* c = b->right;
            BinaryTreeNodeBase* M = c->left;
            BinaryTreeNodeBase* N = c->right;

            BinaryTreeNodeBase* next_node = a->parent;

            c->left = b;
            b->parent = c;

            c->right = a;
            a->parent = c;

            b->right = M;
            if(M != nullptr)
                M->parent = b;

            a->left = N;
            if(N != nullptr)
                N->parent = a;

            c->parent = next_node;

            UpdateNodeHeight(b);
            UpdateNodeHeight(a);
            UpdateNodeHeight(c);

            return next_node;
        }

        static Bool IsLeafNode(const BinaryTreeNodeBase* node) noexcept
        {
            return node->left == nullptr && node->right == nullptr;
        }

        static Void RelinkNodes(BinaryTreeNodeBase* node1, BinaryTreeNodeBase* node2) noexcept
        {
            Swap(node1->parent, node2->parent);
            Swap(node1->left, node2->left);
            Swap(node1->right, node2->right);
            Swap(node1->height, node2->height);
        }

        BinaryTreeNodeBase*& BinaryTreeNodeBase::GetRootBeginIteratorNode() noexcept
        {
            return this->right;
        }

        BinaryTreeNodeBase* const& BinaryTreeNodeBase::GetRootBeginIteratorNode() const noexcept
        {
            return this->right;
        }

        Void BinaryTreeNodeBase::Balance(BinaryTreeNodeBase* root) noexcept
        {
            BinaryTreeNodeBase* node = this;

            while(node != root)
            {
                switch(SelectRotationType(node))
                {
                    case RotationType::None:
                        node = RotateNone(node);
                        break;
                    case RotationType::SmallLeft:
                        node = RotateSmallLeft(node);
                        break;
                    case RotationType::BigLeft:
                        node = RotateBigLeft(node);
                        break;
                    case RotationType::SmallRight:
                        node = RotateSmallRight(node);
                        break;
                    case RotationType::BigRight:
                        node = RotateBigRight(node);
                        break;
                }
            }
        }

        Void BinaryTreeNodeBase::Insert(BinaryTreeNodeBase* parent, BinaryTreeNodeBase*& parent_branch, BinaryTreeNodeBase* root) noexcept
        {
            parent_branch = this;
            this->parent = parent;

            BinaryTreeNodeBase*& begin_node = root->GetRootBeginIteratorNode();
            if(this->parent == begin_node) //new begin
                begin_node = this;

            this->Balance(root);
        }

        Void BinaryTreeNodeBase::Detach(BinaryTreeNodeBase* root) noexcept
        {
            if(IsLeafNode(this)) //with possible iterator change
            {
                BinaryTreeNodeBase*& begin_node = root->GetRootBeginIteratorNode();
                if(begin_node == this)
                    begin_node = this->parent;

                this->parent->Balance(root);
            }
            else
            {
                do
                {
                    auto left_height = GetNodeHeight(this->left);
                    auto right_height = GetNodeHeight(this->right);

                    BinaryTreeNodeBase* repl_node = nullptr;
                    if(left_height > right_height)
                    {
                        repl_node = this->left;
                        while(repl_node->right != nullptr)
                            repl_node = repl_node->right;
                    }
                    else
                    {
                        repl_node = this->right;
                        while(repl_node->left != nullptr)
                            repl_node = repl_node->left;
                    }

                    if(IsLeafNode(repl_node))
                    {
                        RelinkNodes(repl_node, this);
                        BinaryTreeNodeBase* new_parent = this->parent;

                        if(new_parent->left == this)
                            new_parent->left = nullptr;
                        else
                            new_parent->right = nullptr;

                        new_parent->Balance(root);
                    }
                    else
                    {
                        //recurse detach
                        RelinkNodes(repl_node, this);
                    }
                }
                while(!IsLeafNode(this));
            }
        }

        static Void SetSelfLinkedNode(BinaryTreeNodeBase* root) noexcept
        {
            *root = BinaryTreeNodeBase(nullptr, nullptr, root, 0);
        }

        Void BinaryTreeNodeBase::InitRootNode(Bool is_empty_tree) noexcept
        {
            if(is_empty_tree == 0)
                SetSelfLinkedNode(this);
            else
            {
                this->left->parent = this;
                this->GetRootBeginIteratorNode()->parent = this;
            }
        }

        const BinaryTreeNodeBase* BinaryTreeNodeBase::InOrderNodeForwardTraversal() const noexcept
        {
            const BinaryTreeNodeBase* node = this;

            if(IsLeafNode(node))
            {
                if(node->parent->left == node)
                    return node->parent;
                else
                {
                    while(node->parent->right == node)
                        node = node->parent;

                    return node->parent;
                }
            }
            else
            {
                if(node->right != nullptr)
                {
                    node = node->right;
                    while(node->left != nullptr)
                        node = node->left;

                    return node;
                }
                else
                {
                    if(node->parent->left == node)
                        return node->parent;
                    else
                    {
                        while(node->parent->right == node)
                            node = node->parent;

                        return node->parent;
                    }
                }
            }
        }

        const BinaryTreeNodeBase* BinaryTreeNodeBase::InOrderNodeReverseTraversal() const noexcept
        {
            const BinaryTreeNodeBase* node = this;

            if(IsLeafNode(node))
            {
                if(node->parent->right == node)
                    return node->parent;
                else
                {
                    while(node->parent->left == node)
                        node = node->parent;

                    return node->parent;
                }
            }
            else
            {
                if(node->left != nullptr)
                {
                    node = node->left;
                    while(node->right != nullptr)
                        node = node->right;

                    return node;
                }
                else
                {
                    if(node->parent->right == node)
                        return node->parent;
                    else
                    {
                        while(node->parent->left == node)
                            node = node->parent;

                        return node->parent;
                    }
                }
            }
        }
    };
};