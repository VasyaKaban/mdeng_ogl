#include "Events.h"
#include "Core/Utils/ScopedCall.hpp"

namespace Core
{
    class EventHandler : public EmitterNode,
                         public ListenerNode,
                         Core::NonCopyable,
                         Core::NonMovable
    {
    public:
        EventHandler(EventHandlerCallerWrapperType _caller,
                     EventHandlerDeleterWrapperType _deleter,
                     void* _handler_memory,
                     EmitterList* emitter_list,
                     ListenerList* listener_list,
                     EventHandlerState _state)
            : handler_memory(_handler_memory),
              caller(_caller),
              deleter(_deleter),
              state(_state),
              counter(1) //for emitter
        {
            emitter_list->Push(this);
            if(listener_list)
            {
                counter++;
                listener_list->Push(this);
            }
            else
            {
                this->ListenerNode::next = nullptr;
                this->ListenerNode::prev = nullptr;
            }
        }

        ~EventHandler()
        {
            deleter(handler_memory);
        }

        EventHandlerResult operator()(const void* event)
        {
            return caller(handler_memory, event);
        }

        void Enable() noexcept
        {
            state = EventHandlerState::Enabled;
        }

        void Disable() noexcept
        {
            state = EventHandlerState::Disabled;
        }

        EventHandlerState GetState() const noexcept
        {
            return state;
        }

        void IncRef() noexcept
        {
            counter++;
        }

        void ChainDisconnect()
        {
            DropListener();
            DropEmitter();

            if(counter == 0)
                delete this;
        }

        void RefDisconnect()
        {
            assert((counter > 0) && "Reference is not connected");

            counter--;
            if(counter == 0)
                delete this;
        }
    private:
        void DropListener() noexcept
        {
            bool erased = ListenerList::Erase(this);
            if(erased)
            {
                assert((counter > 0) && "Listener is not connected");
                counter--;
            }
        }

        void DropEmitter() noexcept
        {
            bool erased = EmitterList::Erase(this);
            if(erased)
            {
                assert((counter > 0) && "Emitter is not connected");
                counter--;
            }
        }
    private:
        void* handler_memory;
        EventHandlerResult (*caller)(void* memory, const void* event);
        void (*deleter)(void* memory) noexcept;
        EventHandlerState state;

        std::uint64_t counter;
    };

    EventHandlerRef::EventHandlerRef() noexcept
        : handler(nullptr)
    {}

    EventHandlerRef::EventHandlerRef(EventHandler* _handler) noexcept
        : handler(_handler)
    {
        if(handler)
            handler->IncRef();
    }

    EventHandlerRef::~EventHandlerRef()
    {
        if(handler)
            handler->RefDisconnect();
    }

    EventHandlerRef::EventHandlerRef(const EventHandlerRef& ref) noexcept
        : handler(ref.handler)
    {
        if(handler)
            handler->IncRef();
    }

    EventHandlerRef::EventHandlerRef(EventHandlerRef&& ref) noexcept
        : handler(std::exchange(ref.handler, nullptr))
    {}

    EventHandlerRef& EventHandlerRef::operator=(const EventHandlerRef& ref) noexcept
    {
        this->~EventHandlerRef();

        handler = ref.handler;

        if(handler)
            handler->IncRef();

        return *this;
    }

    EventHandlerRef& EventHandlerRef::operator=(EventHandlerRef&& ref) noexcept
    {
        this->~EventHandlerRef();

        handler = std::exchange(ref.handler, nullptr);

        return *this;
    }

    void EventHandlerRef::Enable() noexcept
    {
        if(handler)
            handler->Enable();
    }

    void EventHandlerRef::Disable() noexcept
    {
        if(handler)
            handler->Disable();
    }

    EventHandlerState EventHandlerRef::GetState() const noexcept
    {
        return (handler ? handler->GetState() : EventHandlerState::Disabled);
    }

    void EventHandlerRef::DropRef()
    {
        if(handler)
        {
            handler->RefDisconnect();
            handler = nullptr;
        }
    }

    void EventHandlerRef::Disconnect()
    {
        if(handler)
        {
            handler->ChainDisconnect();
            handler->RefDisconnect();
            handler = nullptr;
        }
    }

    EventListener::EventListener() noexcept
        : list(std::unique_ptr<ListenerList>(
              new ListenerList(ListenerNode{.prev = nullptr, .next = nullptr}, nullptr)))
    {}

    EventListener::~EventListener()
    {
        if(list)
        {
            while(list->next != nullptr)
            {
                auto handler = static_cast<EventHandler*>(list->next);
                handler->ChainDisconnect();
            }
        }
    }

    EventEmitter::EventEmitter(std::initializer_list<Core::ClassIDBase::ClassIDType> reserved)
    {
        for(Core::ClassIDBase::ClassIDType id: reserved)
        {
            EmitterList insert_list = {EmitterNode{.prev = nullptr, .next = nullptr}, nullptr};
            mapping.insert(std::pair{id, insert_list});
        }
    }

    EventEmitter::~EventEmitter()
    {
        for(const auto& [_, list]: mapping)
        {
            while(list.next != nullptr)
            {
                auto handler = static_cast<EventHandler*>(list.next);
                handler->ChainDisconnect();
            }
        }
    }

    EventEmitter::EventEmitter(EventEmitter&& emitter) noexcept
        : mapping(std::move(emitter.mapping))
    {}

    EventEmitter& EventEmitter::operator=(EventEmitter&& emitter) noexcept
    {
        this->~EventEmitter();

        mapping = std::move(emitter.mapping);

        return *this;
    }

    void EventEmitter::EmitRaw(ClassIDBase::ClassIDType id, const void* event)
    {
        auto it = mapping.find(id);
        if(it == mapping.end())
            return;

        EmitterList& list = it->second;
        auto node = list.next;
        if(node == nullptr)
            return;

        while(node != &list)
        {
            auto handler = static_cast<EventHandler*>(node);
            if(handler->GetState() == EventHandlerState::Disabled)
            {
                node = node->next;
                continue;
            }

            EventHandlerResult res = (*handler)(event);
            if(res == EventHandlerResult::None)
                node = node->next;
            else if(res == EventHandlerResult::Disable)
            {
                handler->Disable();
                node = node->next;
            }
            else // if(res == EventHandlerResult::Erase)
            {
                auto next = node->next;
                handler->ChainDisconnect();

                node = next;
            }
        }
    }

    EventHandlerRef EventEmitter::ConnectImpl(Core::ClassIDBase::ClassIDType id,
                                              EventHandlerCallerWrapperType caller,
                                              EventHandlerDeleterWrapperType deleter,
                                              void* handler_memory,
                                              EventListener* listener,
                                              EventHandlerState state)
    {
        Core::ScopedCall cleanup(
            [handler_memory, deleter]()
            {
                deleter(handler_memory);
            });

        EmitterList insert_list = EmitterList::CreateEmpty();
        auto [it, _] = mapping.insert(std::pair{id, insert_list});

        ListenerList* llist = nullptr;
        if(listener)
        {
            assert((listener->list.get() != nullptr) &&
                   "Listener without valid list. Possible use after move!");
            llist = listener->list.get();
        }

        EventHandler* handler =
            new EventHandler(caller, deleter, handler_memory, &it->second, llist, state);

        cleanup.Drop();

        return EventHandlerRef(handler);
    }

};
