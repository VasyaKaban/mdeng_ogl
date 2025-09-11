#pragma once

#include <list>
#include <map>

namespace Events
{
    template<typename E>
    class EventEmitter;

    template<typename E>
    class EventListener
    {
        friend class EventEmitter<E>;
        using ListenerHandle = EventEmitter<E>::ListenerHandle;
    public:
        virtual ~EventListener()
        {
            for(auto& [obj, handle]: handles_mapping)
                obj->notify_erase(handle);
        }

        void Connect(EventEmitter<E>* obj)
        {
            auto [it, inserted] = handles_mapping.insert_or_assign(obj, ListenerHandle{});
            if(!inserted)
                obj->notify_erase(it->second);

            it->second = obj->connect(this);
        }

        void Disconnect(EventEmitter<E>* obj)
        {
            auto it = handles_mapping.find(obj);
            if(it == handles_mapping.end())
                return;

            obj->notify_erase(it->second);

            handles_mapping.erase(it);
        }

        virtual void Handle(E& event)
        {}
    private:
        void notify_erase(EventEmitter<E>* obj) noexcept
        {
            handles_mapping.erase(obj);
        }
    private:
        std::map<EventEmitter<E>*, ListenerHandle> handles_mapping;
    };

    template<typename E>
    class EventEmitter
    {
        friend class EventListener<E>;
        using Container = std::list<EventListener<E>*>;
        using ListenerHandle = std::list<EventListener<E>*>::iterator;
    public:
        ~EventEmitter()
        {
            for(auto& listener: listeners)
                listener->notify_erase(this);
        }

        void Emit(E& event)
        {
            for(auto& listener: listeners)
                listener->Handle(event);
        }
    private:
        void notify_erase(ListenerHandle handle) noexcept
        {
            listeners.erase(handle);
        }

        ListenerHandle connect(EventListener<E>* listener)
        {
            listeners.push_back(listener);
            return std::prev(listeners.end());
        }
    private:
        Container listeners;
    };

    template<typename E>
    void Connect(EventListener<E>& listener, EventEmitter<E>& emitter)
    {
        listener.EventListener<E>::Connect(&emitter);
    }

    template<typename E>
    void Disconnect(EventListener<E>& listener, EventEmitter<E>& emitter)
    {
        listener.EventListener<E>::Disconnect(&emitter);
    }

    /*struct Event
    {
        std::time_t time;
    };

    struct Event2
    {
        int i;
    };

    class Listener : public EventListener<const Event>, public EventListener<Event2>
    {
    public:
        virtual void Handle(const Event& event) override
        {
            std::cerr << "Time is: " << event.time << std::endl;
        }

        virtual void Handle(Event2& event) override
        {
            std::cerr << "I is: " << event.i << std::endl;
        }
    };

    class Emitter : public EventEmitter<const Event>
    {
    public:
        void Foo()
        {
            Emit(Event{.time = 123});
        }
    };

    void foo()
    {
        Emitter emitter;
        Listener listener;

        Connect<const Event>(listener, emitter);

        emitter.Foo();

        Disconnect<const Event>(listener, emitter);

        emitter.Foo();
    }*/

    /*
mapping:
    emitter* -> handle


    EMITTER: list<Listener>

    LISTENER: map<listener, handle>
*/
};