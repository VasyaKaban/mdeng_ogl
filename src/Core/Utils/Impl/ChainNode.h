#pragma once

#include "../Types.hpp"
#include "../Traits.hpp"
#include "../../API.h"

namespace Core
{
    /*
        E|Root| <-> B|1| <-> 2 <-> 3 <---+
        ^                                |
        |                                |
        +--------------------------------+ 
    */

    namespace Detail
    {
        struct CORE_API ChainNodeBase
        {
            ChainNodeBase* prev;
            ChainNodeBase* next;

            ChainNodeBase() noexcept;
            ChainNodeBase(ChainNodeBase* prev, ChainNodeBase* next) noexcept;

            ChainNodeBase*& GetBaseBeginIteratorNode() noexcept;
            ChainNodeBase* const& GetBaseBeginIteratorNode() const noexcept;

            Void Insert(ChainNodeBase* prev_node, ChainNodeBase* base) noexcept; //call it when you insert you node after all pointers linking
            Void Detach(ChainNodeBase* base) noexcept; //call it when you want to detach node

            Void InitBaseNode(Bool is_empty_chain) noexcept;
        };

        template<typename T>
        struct ChainNode : ChainNodeBase
        {
            T value;
        };

        template<typename T>
        class ChainIterator
        {
            constexpr static Bool IsConstIterator = Const<T>;
            using NodeType = Conditional<IsConstIterator, const Detail::ChainNodeBase*, Detail::ChainNodeBase*>;
        public:
            explicit ChainIterator(ChainNodeBase* node) noexcept
                : node(node)
            {}

            explicit ChainIterator(const ChainNodeBase* node) noexcept
            requires IsConstIterator
                : node(node)
            {}

            ChainIterator() = default;
            ~ChainIterator() = default;
            ChainIterator(const ChainIterator&) = default;
            ChainIterator(ChainIterator&&) = default;
            ChainIterator& operator=(const ChainIterator&) = default;
            ChainIterator& operator=(ChainIterator&&) = default;

            ChainIterator operator++(int) noexcept
            {
                ChainIterator out(*this);

                ++(*this);

                return out;
            }

            ChainIterator& operator++() noexcept
            {
                this->node = this->node->next;

                return *this;
            }

            ChainIterator operator--(int) noexcept
            {
                ChainIterator out(*this);

                --(*this);

                return out;
            }

            ChainIterator& operator--() noexcept
            {
                this->node = this->node->prev;

                return *this;
            }

            T& operator*() const noexcept
            {
                return *static_cast<ChainNode<T>*>(this->node)->value;
            }

            T* operator->() const noexcept
            {
                return *static_cast<ChainNode<T>*>(this->node)->value;
            }

            template<typename OT>
            Bool operator==(const ChainIterator<OT>& it) const noexcept
            {
                return this->node == it.node;
            }

            NodeType GetNode() const noexcept
            {
                return this->node;
            }

            operator ChainIterator<const T>() const noexcept
            requires(!IsConstIterator)
            {
                return ChainIterator<const T>(this->node);
            }
        private:
            NodeType node;
        };
    };
};