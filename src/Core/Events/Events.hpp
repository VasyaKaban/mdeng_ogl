#pragma once

#include <map>
#include <memory>
#include <cassert>
#include "hrs/non_creatable.hpp"
#include "../Utils/ClassID.hpp"

namespace Core
{
    template<typename T>
    concept Event = std::is_base_of_v<ClassID<T>, T>;

    struct EmitterNode
    {
        EmitterNode* prev;
        EmitterNode* next;
    };

    struct ListenerNode
    {
        ListenerNode* prev;
        ListenerNode* next;
    };

    enum class EventHandlerAction
    {
        None,
        Erase,
        Disable
    };

    class EventHandlerNode : EmitterNode, ListenerNode, hrs::non_copyable, hrs::non_movable
    {
    public:
        template<Event E, typename F>
        requires std::is_invocable_r_v<void, F, const E&>
        EventHandlerNode(F&& _func, bool _enabled)
            : EmitterNode{.prev = nullptr, .next = nullptr},
              ListenerNode{.prev = nullptr, .next = nullptr},
              enabled(_enabled),
              func_caller(function_caller_wrapper<F, E>),
              func(::operator new[](sizeof(F), std::align_val_t(alignof(F))),
                   function_destructor_wrapper<F>)
        {}

        ~EventHandlerNode() = default;

        EventHandlerAction operator()(const void* event);

        void Disable() noexcept;
        void Enable() noexcept;
        bool IsEnabled() const noexcept;

        void DecoupleEmitterList() noexcept;
        void DecoupleListenerList() noexcept;
    private:
        template<typename F>
        static void function_destructor_wrapper(std::byte* func) noexcept
        {
            reinterpret_cast<F*>(func)->~F();
            ::operator delete[](func, std::align_val_t(alignof(F)));
        }

        template<typename F, typename E>
        static EventHandlerAction function_caller_wrapper(std::byte* func, const void* event)
        {
            return (*reinterpret_cast<F*>(func))(*static_cast<const E*>(event));
        }
    private:
        bool enabled;
        EventHandlerAction (*func_caller)(std::byte* func, const void* event);
        std::unique_ptr<std::byte[], void (*)(std::byte*) noexcept> func;
    };

    template<typename N>
    requires std::same_as<N, EmitterNode> || std::same_as<N, ListenerNode>
    class NodeList : N, hrs::non_copyable
    {
    public:
        NodeList() noexcept
            : N{.prev = nullptr, .next = nullptr},
              last(nullptr)
        {}

        ~NodeList()
        {
            if(this->N::next == nullptr)
                return;

            auto node = this->N::next;
            while(node != this)
            {
                EventHandlerNode* handler = static_cast<EventHandlerNode*>(node);
                if constexpr(std::same_as<N, EmitterNode>)
                    handler->DecoupleEmitterList();
                else
                    handler->DecoupleListenerList();

                node = node->next;
                delete handler;
            }
        }

        NodeList(NodeList&& list) noexcept
            : N{.prev = std::exchange(list.prev, nullptr),
                .next = std::exchange(list.next, nullptr)},
              last(std::exchange(list.last, nullptr))
        {}

        NodeList& operator=(NodeList&& list) noexcept
        {
            this->~NodeList();

            this->N::prev = std::exchange(list.prev, nullptr);
            this->N::next = std::exchange(list.next, nullptr);
            last = std::exchange(list.last, nullptr);

            return *this;
        }

        void Push(EventHandlerNode* handler) noexcept
        {
            if(last == nullptr) //empty
            {
                this->N::next = handler;
                last = handler;

                handler->N::prev = this;
                handler->N::next = this;
            }
            else
            {
                auto prev_last = last;

                prev_last->next = handler;
                last = handler;

                handler->N::prev = prev_last;
                handler->N::next = this;
            }
        }
    public:
        N* last;
    };

    using EmitterNodeList = NodeList<EmitterNode>;
    using ListenerNodeList = NodeList<ListenerNode>;

    class EventHandlerRef
    {
    public:
        EventHandlerRef(EventHandlerNode* _node) noexcept;
        ~EventHandlerRef() = default;
        EventHandlerRef(const EventHandlerRef&) = default;
        EventHandlerRef(EventHandlerRef&&) = default;
        EventHandlerRef& operator=(const EventHandlerRef&) = default;
        EventHandlerRef& operator=(EventHandlerRef&&) = default;

        void Disable() noexcept;
        void Enable() noexcept;
        bool IsEnabled() const noexcept;
        void Disconnect() noexcept;
    private:
        EventHandlerNode* node;
    };

    class Listener : hrs::non_copyable
    {
    public:
        Listener() = default;
        ~Listener() = default;
        Listener(Listener&&) = default;
        Listener& operator=(Listener&&) = default;
    private:
        ListenerNodeList list;
    };

    class Emitter : hrs::non_copyable
    {
    public:
        Emitter() = default;
        ~Emitter() = default;
        Emitter(Emitter&&) = default;
        Emitter& operator=(Emitter&&) = default;

        template<Event E>
        void Emit(const E& event)
        {
            auto it = mapping.find(E::template ClassID<E>::ID);
            if(it == mapping.end())
                return;

            EmitterNode* node = it->next;
            if(node == nullptr)
                return;

            while(node != &(*it))
            {
                EventHandlerNode* handler = static_cast<EventHandlerNode*>(node);
                if(!handler->IsEnabled())
                {
                    node = node->next;
                    continue;
                }

                EventHandlerAction action = (*handler)(&event);
                switch(action)
                {
                    case EventHandlerAction::None:
                        node = node->next;
                        break;
                    case EventHandlerAction::Disable:
                        handler->Disable();
                        node = node->next;
                        break;
                    case EventHandlerAction::Erase:
                    {
                        auto next = node->next;
                        handler->DecoupleEmitterList();
                        handler->DecoupleListenerList();
                        delete handler;
                        node = next;
                    }
                    break;
                }
            }
        }

        template<Event E, typename F>
        requires std::is_invocable_r_v<void, F, const E&>
        EventHandlerRef Connect(Listener* listener, F&& func, bool enabled)
        {
            auto [it, _] = mapping.insert({E::template ClassID<E>::ID, EmitterNodeList{}});

            auto handler = new EventHandlerNode(std::forward<F>(func), enabled);
            it->Push(handler);
            if(listener)
                listener->list.Push(handler);
        }
    private:
        std::map<ClassIDBase::Type, EmitterNodeList> mapping;
    };
};