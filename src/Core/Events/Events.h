#pragma once

#include <map>
#include <cassert>
#include <memory>
#include "hrs/non_creatable.hpp"
#include "Core/Utils/ClassID.hpp"

namespace Core
{
    template<typename E>
    concept Event = std::is_base_of_v<Core::ClassID<E>, E>;

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

    struct EmitterNode;
    struct ListenerNode;

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

    class EventHandlerRef
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
    struct NodeList;

    using EmitterList = NodeList<EmitterNode>;
    using ListenerList = NodeList<ListenerNode>;

    class EventListener : hrs::non_copyable
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

    class EventEmitter : hrs::non_copyable
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
            EmitImpl(Core::ClassID<E>::ID, &event);
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
    private:
        void EmitImpl(ClassIDBase::ClassIDType id, const void* event);
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