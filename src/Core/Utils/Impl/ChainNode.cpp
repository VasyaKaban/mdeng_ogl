#include "ChainNode.h"

namespace Core
{
    namespace Detail
    {
        ChainNodeBase::ChainNodeBase() noexcept
            : prev(nullptr),
              next(nullptr)
        {}

        ChainNodeBase::ChainNodeBase(ChainNodeBase* prev, ChainNodeBase* next) noexcept
            : prev(prev),
              next(next)
        {}

        ChainNodeBase*& ChainNodeBase::GetBaseBeginIteratorNode() noexcept
        {
            return this->next;
        }

        ChainNodeBase* const& ChainNodeBase::GetBaseBeginIteratorNode() const noexcept
        {
            return this->next;
        }

        Void ChainNodeBase::Insert(ChainNodeBase* prev_node, ChainNodeBase* base) noexcept
        {
            ChainNodeBase* next_node = prev_node->next;

            prev_node->next = this;
            next_node->prev = this;

            this->prev = prev_node;
            this->next = next_node;
        }

        Void ChainNodeBase::Detach(ChainNodeBase* base) noexcept
        {
            ChainNodeBase* prev = this->prev;
            ChainNodeBase* next = this->next;

            prev->next = next;
            next->prev = prev;
        }

        static Void SetSelfLinkedNode(ChainNodeBase* base) noexcept
        {
            *base = ChainNodeBase(base, base);
        }

        Void ChainNodeBase::InitBaseNode(Bool is_empty_chain) noexcept
        {
            if(is_empty_chain)
                SetSelfLinkedNode(this);
            else
            {
                this->next->prev = this;
                this->prev->next = this;
            }
        }
    };
};