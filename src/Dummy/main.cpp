#include <map>
#include <type_traits>
#include <cassert>
#include <memory>
#include "hrs/non_creatable.hpp"
#include <iostream>
#include "Core/Utils/ClassID.hpp"

namespace Test
{
    template<typename E>
    concept Event = std::is_base_of_v<Core::ClassID<E>, E>;

    enum class EventHandlerResult
    {
        None,
        Disable,
        Erase
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

    struct EmitterList : public EmitterNode
    {
        //prev = nullptr
        EmitterNode* last;
    };

    struct ListenerList : public ListenerNode
    {
        //prev = nullptr
        ListenerNode* last;
    };

    template<Event E, EventHandlerFunc<E> F>
    EventHandlerResult event_handler_caller_wrapper(std::byte* memory, const void* event)
    {
        return (*reinterpret_cast<F*>(memory))(*static_cast<const E*>(event));
    }

    template<Event E, EventHandlerFunc<E> F>
    void event_handler_deleter_wrapper(std::byte* memory) noexcept
    {
        delete reinterpret_cast<F*>(memory);
    }

    class ListenerList;
    class EmitterList;
    class EventHandlerRef;

    class EventHandler : public EmitterNode,
                         public ListenerNode,
                         hrs::non_copyable,
                         hrs::non_movable
    {
    public:
        template<Event E, EventHandlerFunc<E> F>
        EventHandler(const E&,
                     F&& func,
                     bool _enabled,
                     EmitterList* emitter_list,
                     ListenerList* listener_list)
            : handler_memory(reinterpret_cast<std::byte*>(new F(std::forward<F>(func)))),
              caller(event_handler_caller_wrapper<E, F>),
              deleter(event_handler_deleter_wrapper<E, F>),
              enabled(_enabled),
              counter(1) //for emitter
        {
            if(emitter_list->next == nullptr)
            {
                emitter_list->next = this;
                emitter_list->last = this;

                this->EmitterNode::next = emitter_list;
                this->EmitterNode::prev = emitter_list;
            }
            else
            {
                auto last = emitter_list->last;

                emitter_list->last = this;
                last->next = this;

                this->EmitterNode::next = emitter_list;
                this->EmitterNode::prev = last;
            }

            if(listener_list)
            {
                counter++;
                if(listener_list->next == nullptr)
                {
                    listener_list->next = this;
                    listener_list->last = this;

                    this->ListenerNode::next = listener_list;
                    this->ListenerNode::prev = listener_list;
                }
                else
                {
                    auto last = listener_list->last;

                    listener_list->last = this;
                    last->next = this;

                    this->ListenerNode::next = listener_list;
                    this->ListenerNode::prev = last;
                }
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
            enabled = true;
        }

        void Disable() noexcept
        {
            enabled = false;
        }

        bool IsEnabled() const noexcept
        {
            return enabled;
        }

        void DropListener() noexcept
        {
            if(this->ListenerNode::prev == nullptr)
                return;

            assert(counter > 0);
            counter--;

            bool is_first = (this->ListenerNode::prev->prev == nullptr);
            bool is_last = (this->ListenerNode::next->prev == nullptr);

            if(is_first && is_last)
            {
                auto list = static_cast<ListenerList*>(this->ListenerNode::prev);

                list->next = nullptr;
                list->last = nullptr;
            }
            else if(is_first)
            {
                auto list = static_cast<ListenerList*>(this->ListenerNode::prev);

                list->next = this->ListenerNode::next;
                this->ListenerNode::next->prev = list;
            }
            else if(is_last)
            {
                auto list = static_cast<ListenerList*>(this->ListenerNode::next);

                this->ListenerNode::prev->next = list;
            }
            else //if(!is_first && !is_last)
            {
                this->ListenerNode::prev->next = this->ListenerNode::next;
                this->ListenerNode::next->prev = this->ListenerNode::prev;
            }
        }

        void DropEmitter() noexcept
        {
            assert(counter > 0);
            counter--;

            bool is_first = (this->EmitterNode::prev->prev == nullptr);
            bool is_last = (this->EmitterNode::next->prev == nullptr);

            if(is_first && is_last)
            {
                auto list = static_cast<EmitterList*>(this->EmitterNode::prev);

                list->next = nullptr;
                list->last = nullptr;
            }
            else if(is_first)
            {
                auto list = static_cast<EmitterList*>(this->EmitterNode::prev);

                list->next = this->EmitterNode::next;
                this->EmitterNode::next->prev = list;
            }
            else if(is_last)
            {
                auto list = static_cast<EmitterList*>(this->EmitterNode::next);

                this->EmitterNode::prev->next = list;
            }
            else //if(!is_first && !is_last)
            {
                this->EmitterNode::prev->next = this->EmitterNode::next;
                this->EmitterNode::next->prev = this->EmitterNode::prev;
            }
        }

        void GetRef() noexcept
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
            assert(counter > 0);

            counter--;
            if(counter == 0)
                delete this;
        }
    private:
        std::byte* handler_memory;
        EventHandlerResult (*caller)(std::byte* memory, const void* event);
        void (*deleter)(std::byte* memory) noexcept;
        bool enabled;

        std::uint64_t counter;
    };

    class EventHandlerRef
    {
    public:
        EventHandlerRef(EventHandler* _handler) noexcept
            : handler(_handler)
        {
            if(handler)
                handler->GetRef();
        }

        ~EventHandlerRef()
        {
            if(handler)
                handler->RefDisconnect();
        }

        EventHandlerRef(const EventHandlerRef& ref) noexcept
            : handler(ref.handler)
        {
            if(handler)
                handler->GetRef();
        }

        EventHandlerRef(EventHandlerRef&& ref) noexcept
            : handler(std::exchange(ref.handler, nullptr))
        {}

        EventHandlerRef& operator=(const EventHandlerRef& ref) noexcept
        {
            this->~EventHandlerRef();

            handler = ref.handler;

            if(handler)
                handler->GetRef();

            return *this;
        }

        EventHandlerRef& operator=(EventHandlerRef&& ref) noexcept
        {
            this->~EventHandlerRef();

            handler = std::exchange(ref.handler, nullptr);

            return *this;
        }

        void Enable() noexcept
        {
            if(handler)
                handler->Enable();
        }

        void Disable() noexcept
        {
            if(handler)
                handler->Disable();
        }

        bool IsEnabled() const noexcept
        {
            return (handler ? handler->IsEnabled() : false);
        }

        void DropRef()
        {
            if(handler)
            {
                handler->RefDisconnect();
                handler = nullptr;
            }
        }

        void Disconnect()
        {
            if(handler)
            {
                handler->RefDisconnect();
                handler->ChainDisconnect();
                handler = nullptr;
            }
        }
    private:
        EventHandler* handler;
    };

    class EventListener : hrs::non_copyable
    {
    public:
        EventListener() noexcept
            : list(std::unique_ptr<ListenerList>(
                  new ListenerList(ListenerNode{.prev = nullptr, .next = nullptr}, nullptr)))
        {}

        ~EventListener()
        {
            while(list->next != nullptr)
            {
                auto handler = static_cast<EventHandler*>(list->next);
                handler->ChainDisconnect();
            }
        }

#pragma message("MOVE PTR BUT LEAVEE NULLPTR IN LISTENER???")
        EventListener(EventListener&& listener) noexcept
        {
            list->prev = std::exchange(listener.list->prev, nullptr);
            list->next = std::exchange(listener.list->next, nullptr);
            list->last = std::exchange(listener.list->last, nullptr);
        }

        EventListener& operator=(EventListener&& listener) noexcept
        {
            this->~EventListener();

            list->prev = std::exchange(listener.list->prev, nullptr);
            list->next = std::exchange(listener.list->next, nullptr);
            list->last = std::exchange(listener.list->last, nullptr);

            return *this;
        }

        ListenerList* GetList() noexcept
        {
            return list.get();
        }
    private:
        std::unique_ptr<ListenerList> list;
    };

    class EventEmitter : hrs::non_copyable
    {
    public:
        EventEmitter() = default;

        EventEmitter(std::initializer_list<Core::ClassIDBase::ClassIDType> reserved)
        {
            for(Core::ClassIDBase::ClassIDType id: reserved)
            {
                EmitterList insert_list = {EmitterNode{.prev = nullptr, .next = nullptr}, nullptr};
                mapping.insert(std::pair{id, insert_list});
            }
        }

        ~EventEmitter()
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

        EventEmitter(EventEmitter&& emitter) noexcept
            : mapping(std::move(emitter.mapping))
        {}

        EventEmitter& operator=(EventEmitter&& emitter) noexcept
        {
            this->~EventEmitter();

            mapping = std::move(emitter.mapping);

            return *this;
        }

        template<Event E>
        void Emit(const E& event)
        {
            auto it = mapping.find(Core::ClassID<E>::ID);
            if(it == mapping.end())
                return;

            EmitterList& list = it->second;
            auto node = list.next;
            if(node == nullptr)
                return;

            while(node != &list)
            {
                auto handler = static_cast<EventHandler*>(node);
                if(!handler->IsEnabled())
                {
                    node = node->next;
                    continue;
                }

                EventHandlerResult res = (*handler)(&event);
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

        template<Event E, EventHandlerFunc<E> F>
        EventHandlerRef Connect(F&& func, EventListener* listener, bool enabled)
        {
            EmitterList insert_list = {EmitterNode{.prev = nullptr, .next = nullptr}, nullptr};
            auto [it, _] = mapping.insert(std::pair{Core::ClassID<E>::ID, insert_list});

            EventHandler* handler = new EventHandler(E{},
                                                     std::forward<F>(func),
                                                     enabled,
                                                     &it->second,
                                                     (listener ? listener->GetList() : nullptr));

            return EventHandlerRef(handler);
        }
    private:
        std::map<Core::ClassIDBase::ClassIDType, EmitterList> mapping;
    };
};

struct EventType : Core::ClassID<EventType>
{
    int data;
};

class CommonEmitter : public Test::EventEmitter
{};

class CommonListener : public Test::EventListener
{};

#include <vector>
#include <format>

int main(int argc, char** argv)
{
    constexpr std::size_t SIZE = 12;

    CommonEmitter emitter;
    CommonListener listener;

    std::size_t num = 0;

    std::vector<Test::EventHandlerRef> refs;
    refs.reserve(SIZE);
    for(std::size_t i = 0; i < SIZE; i++)
    {
        auto ref = emitter.Connect<EventType>(
            [&num, i](const EventType& event) -> Test::EventHandlerResult
            {
                std::cout << std::format("Num: {}; Index: {}; data: {}", num, i, event.data)
                          << std::endl;

                num++;

                if(event.data % 11 == 0)
                    return Test::EventHandlerResult::Erase;

                return Test::EventHandlerResult::None;
            },
            /*&listener,*/ nullptr,
            true);

        refs.push_back(ref);
    }

    num = 0;
    emitter.Emit(EventType{.data = 42});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        if(i % 7 == 0)
            refs[i].Disable();
    }

    num = 0;
    emitter.Emit(EventType{.data = 123});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        if(i % 9 == 0)
            refs[i].Disconnect();
    }

    num = 0;
    emitter.Emit(EventType{.data = 1024});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        refs[i].Disconnect();
    }

    num = 0;
    emitter.Emit(EventType{.data = 0});
}