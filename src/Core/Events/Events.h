#pragma once

#include <map>
#include <cassert>
#include <memory>
#include "Core/Utils/NonCreatable.hpp"
#include "Core/Utils/ClassID.hpp"
#include "Core/API.h"

namespace Core
{
    template<typename E>
    concept Event = true; //std::is_base_of_v<Core::ClassID<E>, E>;

    enum class EventHandlerResult
    {
        None,
        Disable,
        Erase
    };

    enum class EventHandlerState
    {
        Enabled,
        Disabled
    };

    template<typename F, typename E>
    concept EventHandlerFunc = Event<E> && requires(F&& func, const E& event) {
        { std::forward<F>(func)(event) } -> std::same_as<EventHandlerResult>;
    };

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

    template<Event E, EventHandlerFunc<E> F>
    EventHandlerResult event_handler_caller_wrapper(void* memory, const void* event)
    {
        return (*reinterpret_cast<F*>(memory))(*static_cast<const E*>(event));
    }

    using EventHandlerCallerWrapperType = EventHandlerResult (*)(void* memory, const void* event);

    template<Event E, EventHandlerFunc<E> F>
    void event_handler_deleter_wrapper(void* memory) noexcept
    {
        delete reinterpret_cast<F*>(memory);
    }

    using EventHandlerDeleterWrapperType = void (*)(void* memory) noexcept;

    class EventHandler;

    class CORE_API EventHandlerRef
    {
    public:
        EventHandlerRef() noexcept;
        EventHandlerRef(EventHandler* _handler) noexcept;

        ~EventHandlerRef();

        EventHandlerRef(const EventHandlerRef& ref) noexcept;
        EventHandlerRef(EventHandlerRef&& ref) noexcept;
        EventHandlerRef& operator=(const EventHandlerRef& ref) noexcept;
        EventHandlerRef& operator=(EventHandlerRef&& ref) noexcept;

        void Enable() noexcept;
        void Disable() noexcept;
        EventHandlerState GetState() const noexcept;

        void DropRef();
        void Disconnect();
    private:
        EventHandler* handler;
    };

    template<typename N>
    requires std::same_as<N, EmitterNode> || std::same_as<N, ListenerNode>
    struct NodeList : N
    {
        //prev = nullptr
        N* last;

        static NodeList CreateEmpty() noexcept
        {
            return NodeList{N{.prev = nullptr, .next = nullptr}, nullptr};
        }

        bool IsEmpty() const noexcept
        {
            return this->N::next == nullptr;
        }

        void Push(N* node) noexcept
        {
            if(IsEmpty())
            {
                this->N::next = node;
                this->last = node;

                node->next = this;
                node->prev = this;
            }
            else
            {
                auto prev_last = last;

                last = node;
                prev_last->next = node;

                node->next = this;
                node->prev = prev_last;
            }
        }

        static bool Erase(N* node) noexcept
        {
            if(node->prev == nullptr)
                return false;

            bool is_first = (node->prev->prev == nullptr);
            bool is_last = (node->next->prev == nullptr);

            if(is_first && is_last)
            {
                auto list = static_cast<NodeList*>(node->prev);

                list->next = nullptr;
                list->last = nullptr;
            }
            else if(is_first)
            {
                auto list = static_cast<NodeList*>(node->prev);

                list->next = node->next;
                node->next->prev = list;
            }
            else if(is_last)
            {
                auto list = static_cast<NodeList*>(node->next);

                node->prev->next = list;
            }
            else //if(!is_first && !is_last)
            {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }

            node->prev = nullptr; //mark as erased

            return true;
        }
    };

    using EmitterList = NodeList<EmitterNode>;
    using ListenerList = NodeList<ListenerNode>;

    class CORE_API EventListener : Core::NonCopyable
    {
        friend class EventEmitter;
    public:
        EventListener() noexcept;
        ~EventListener();

        EventListener(EventListener&&) = default;
        EventListener& operator=(EventListener&&) = default;
    private:
        std::unique_ptr<ListenerList> list;
    };

    class CORE_API EventEmitter : Core::NonCopyable
    {
    public:
        EventEmitter() = default;
        EventEmitter(std::initializer_list<Core::ClassIDBase::ClassIDType> reserved);

        ~EventEmitter();

        EventEmitter(EventEmitter&& emitter) noexcept;
        EventEmitter& operator=(EventEmitter&& emitter) noexcept;

        template<Event E>
        void Emit(const E& event)
        {
            EmitRaw(Core::ClassID<E>::ID, &event);
        }

        template<Event E, EventHandlerFunc<E> F>
        requires std::constructible_from<std::remove_cvref_t<F>, F>
        EventHandlerRef Connect(F&& func, EventListener* listener, EventHandlerState state)
        {
            EventHandlerCallerWrapperType caller = event_handler_caller_wrapper<E, F>;
            EventHandlerDeleterWrapperType deleter = event_handler_deleter_wrapper<E, F>;
            void* handler_memory = new F(std::forward<F>(func));

            return ConnectImpl(Core::ClassID<E>::ID,
                               caller,
                               deleter,
                               handler_memory,
                               listener,
                               state);
        }
        void EmitRaw(ClassIDBase::ClassIDType id, const void* event);
    private:
        EventHandlerRef ConnectImpl(ClassIDBase::ClassIDType id,
                                    EventHandlerCallerWrapperType caller,
                                    EventHandlerDeleterWrapperType deleter,
                                    void* handler_memory,
                                    EventListener* listener,
                                    EventHandlerState state);
    private:
        std::map<Core::ClassIDBase::ClassIDType, EmitterList> mapping;
    };

    template<Event... Events>
    class ReservedEventEmitter : public EventEmitter
    {
    public:
        ReservedEventEmitter()
            : EventEmitter({Core::ClassID<Events>::ID...})
        {}

        ~ReservedEventEmitter() = default;
        ReservedEventEmitter(ReservedEventEmitter&&) = default;
        ReservedEventEmitter& operator=(ReservedEventEmitter&&) = default;
    };
};