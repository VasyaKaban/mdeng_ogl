#pragma once

#include <list>
#include <map>
#include <functional>
#include <cassert>
#include "hrs/non_creatable.hpp"

namespace Events
{
    template<typename E>
    class EventEmitter;

    template<typename E>
    class EventListener;

    enum class HandlerAction
    {
        None,
        Erase,
        Disable
    };

    template<typename E>
    using EventHandler = std::function<HandlerAction(const E&)>;

    template<typename E>
    class EventListenerWrapper : hrs::non_copyable, hrs::non_move_assignable
    {
    public:
        EventListenerWrapper(EventListener<E>* _listener, EventHandler<E>&& _handler) noexcept
            : listener(_listener),
              handler(std::move(_handler)),
              enabled(true)
        {}

        ~EventListenerWrapper() = default;

        EventListenerWrapper(EventListenerWrapper&& w) noexcept
            : listener(std::exchange(w.listener, nullptr)),
              handler(std::move(w.handler)),
              enabled(w.enabled)
        {}

        HandlerAction operator()(const E& event)
        {
            if(enabled)
                return handler(event);

            return HandlerAction::None;
        }

        void NotifyErase(EventEmitter<E>* emitter)
        {
            if(listener)
                listener->notify_erase(emitter);
        }

        void Enable() noexcept
        {
            enabled = true;
        }

        void Disable() noexcept
        {
            enabled = false;
        }
    private:
        EventListener<E>* listener;
        EventHandler<E> handler;
        bool enabled;
    };

    template<typename E>
    class EventListener : hrs::non_copyable, hrs::non_movable
    {
        friend class EventEmitter<E>;
        friend class EventListenerWrapper<E>;

        using ListenerHandle = EventEmitter<E>::ListenerHandle;
    public:
        EventListener() = default;

        ~EventListener()
        {
            for(auto& [obj, handle]: handles_mapping)
                obj->notify_erase(handle);
        }
        void Connect(EventEmitter<E>* obj, EventHandler<E>&& _handler)
        {
            auto [it, inserted] = handles_mapping.insert(std::pair{obj, ListenerHandle{}});
            if(!inserted)
                obj->notify_erase(it->second);

            it->second = obj->connect(EventListenerWrapper<E>(this, std::move(_handler)));
        }

        void Disconnect(EventEmitter<E>* obj)
        {
            auto it = handles_mapping.find(obj);
            if(it == handles_mapping.end())
                return;

            obj->notify_erase(it->second);

            handles_mapping.erase(it);
        }

        void Enable(EventEmitter<E*> obj)
        {
            auto it = handles_mapping.find(obj);
            if(it == handles_mapping.end())
                return;

            it->second->Enable();
        }

        void Disable(EventEmitter<E*> obj)
        {
            auto it = handles_mapping.find(obj);
            if(it == handles_mapping.end())
                return;

            it->second->Disable();
        }
    private:
        void notify_erase(EventEmitter<E>* obj) noexcept
        {
            handles_mapping.erase(obj);
        }
    private:
        std::map<EventEmitter<E>*, ListenerHandle> handles_mapping;
    };

    template<typename E>
    class EventEmitter : hrs::non_copyable, hrs::non_movable
    {
        friend class EventListener<E>;
        using Container = std::list<EventListenerWrapper<E>>;

        template<typename EV>
        friend void Emit(EventEmitter<EV>* emitter, const EV& event);
    public:
        using ListenerHandle = Container::iterator;

        EventEmitter() = default;

        ~EventEmitter()
        {
            for(auto& listener: listeners)
                listener.NotifyErase(this);
        }

        ListenerHandle NoListenerConnect(EventHandler<E>&& _handler)
        {
            return connect(EventListenerWrapper<E>(nullptr, std::move(_handler)));
        }

        void NoListenerDisconnect(ListenerHandle handle)
        {
            listeners.erase(handle);
        }

        void NoListenerEnable(ListenerHandle handle)
        {
            handle->Enable();
        }

        void NoListenerDisable(ListenerHandle handle)
        {
            handle->Disable();
        }
    protected:
        void Emit(const E& event)
        {
            auto it = listeners.begin();
            while(it != listeners.end())
            {
                HandlerAction action = (*it)(event);
                switch(action)
                {
                    case HandlerAction::None:
                        it++;
                        break;
                    case HandlerAction::Erase:
                    {
                        auto to_erase_it = it;
                        it++;
                        to_erase_it->NotifyErase(this);
                        Disconnect<E>(to_erase_it, this);
                    }
                    break;
                    case HandlerAction::Disable:
                        Disable<E>(it, this);
                        it++;
                        break;
                }
            }
        }
    private:
        void notify_erase(ListenerHandle handle) noexcept
        {
            listeners.erase(handle);
        }

        ListenerHandle connect(EventListenerWrapper<E>&& listener)
        {
            listeners.push_back(std::move(listener));
            return std::prev(listeners.end());
        }
    private:
        Container listeners;
    };

    template<typename E, typename L>
    requires std::is_base_of_v<EventListener<E>, L>
    void Connect(L* listener, EventEmitter<E>* emitter, HandlerAction (L::*handler)(const E&))
    {
        assert(listener != nullptr);

        EventHandler<E> func_handler(
            [listener, handler](const E& event)
            {
                return (listener->*handler)(event);
            });

        listener->EventListener<E>::Connect(emitter, std::move(func_handler));
    }

    template<typename E, typename L>
    requires std::is_base_of_v<EventListener<E>, L>
    void
    Connect(L* listener, EventEmitter<E>* emitter, HandlerAction (L::*handler)(const E&) noexcept)
    {
        EventHandler<E> func_handler(
            [listener, handler](const E& event)
            {
                return (listener->*handler)(event);
            });

        listener->EventListener<E>::Connect(emitter, std::move(func_handler));
    }

    template<typename E, typename F>
    requires std::is_invocable_r_v<void, F, const E&>
    void Connect(EventListener<E>* listener, EventEmitter<E>* emitter, F&& handler)
    {
        EventHandler<E> func_handler(
            [_handler = std::forward<F>(handler)](const E& event)
            {
                std::forward<F>(_handler)(event);
            });

        listener->Connect(emitter, std::move(func_handler));
    }

    template<typename E, typename F>
    requires std::is_invocable_r_v<void, F, const E&>
    EventEmitter<E>::ListenerHandle Connect(EventEmitter<E>* emitter, F&& handler)
    {
        EventHandler<E> func_handler(
            [_handler = std::forward<F>(handler)](const E& event)
            {
                return _handler(event);
            });

        return emitter->NoListenerConnect(std::move(func_handler));
    }

    template<typename E>
    void Disconnect(EventListener<E>* listener, EventEmitter<E>* emitter)
    {
        listener->Disconnect(&emitter);
    }

    template<typename E>
    void Disconnect(typename EventEmitter<E>::ListenerHandle handle, EventEmitter<E>* emitter)
    {
        emitter->NoListenerDisconnect(handle);
    }

    template<typename E>
    void Emit(EventEmitter<E>* emitter, const E& event)
    {
        emitter->Emit(event);
    }

    template<typename E>
    void Enable(EventListener<E>* listener, EventEmitter<E>* emitter)
    {
        listener->Enable(emitter);
    }

    template<typename E>
    void Disable(EventListener<E>* listener, EventEmitter<E>* emitter)
    {
        listener->Disable(emitter);
    }

    template<typename E>
    void Enable(typename EventEmitter<E>::ListenerHandle handle, EventEmitter<E>* emitter)
    {
        emitter->NoListenerEnable(handle);
    }

    template<typename E>
    void Disable(typename EventEmitter<E>::ListenerHandle handle, EventEmitter<E>* emitter)
    {
        emitter->NoListenerDisable(handle);
    }
};