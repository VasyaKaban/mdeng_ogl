#include <map>
#include <type_traits>
#include <cassert>
#include <memory>
#include "hrs/non_creatable.hpp"
#include <iostream>
#include "Core/Utils/ClassID.hpp"

namespace Test
{
    template<typename T>
    struct type_arg
    {
        using type = T;
    };

    template<typename T>
    using type_arg_t = type_arg<T>;

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

            return true;
        }
    };

    using EmitterList = NodeList<EmitterNode>;
    using ListenerList = NodeList<ListenerNode>;

    template<Event E, EventHandlerFunc<E> F>
    EventHandlerResult event_handler_caller_wrapper(void* memory, const void* event)
    {
        return (*reinterpret_cast<F*>(memory))(*static_cast<const E*>(event));
    }

    template<Event E, EventHandlerFunc<E> F>
    void event_handler_deleter_wrapper(void* memory) noexcept
    {
        delete reinterpret_cast<F*>(memory);
    }

    class EventHandlerRef;

    class EventHandler : public EmitterNode,
                         public ListenerNode,
                         hrs::non_copyable,
                         hrs::non_movable
    {
    public:
        template<Event E, EventHandlerFunc<E> F>
        requires std::constructible_from<std::remove_cvref_t<F>, F>
        EventHandler(type_arg<E> _arg,
                     F&& func,
                     bool _enabled,
                     EmitterList* emitter_list,
                     ListenerList* listener_list)
            : handler_memory(new F(std::forward<F>(func))),
              caller(event_handler_caller_wrapper<E, F>),
              deleter(event_handler_deleter_wrapper<E, F>),
              enabled(_enabled),
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
        bool enabled;

        std::uint64_t counter;
    };

    class EventHandlerRef
    {
    public:
        EventHandlerRef() noexcept
            : handler(nullptr)
        {}

        EventHandlerRef(EventHandler* _handler) noexcept
            : handler(_handler)
        {
            if(handler)
                handler->IncRef();
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
                handler->IncRef();
        }

        EventHandlerRef(EventHandlerRef&& ref) noexcept
            : handler(std::exchange(ref.handler, nullptr))
        {}

        EventHandlerRef& operator=(const EventHandlerRef& ref) noexcept
        {
            this->~EventHandlerRef();

            handler = ref.handler;

            if(handler)
                handler->IncRef();

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
                handler->ChainDisconnect();
                handler->RefDisconnect();
                handler = nullptr;
            }
        }
    private:
        EventHandler* handler;
    };

    class EventListener : hrs::non_copyable
    {
        friend class EventEmitter;
    public:
        EventListener() noexcept
            : list(std::unique_ptr<ListenerList>(
                  new ListenerList(ListenerNode{.prev = nullptr, .next = nullptr}, nullptr)))
        {}

        ~EventListener()
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

        EventListener(EventListener&&) = default;
        EventListener& operator=(EventListener&&) = default;
    private:
        ListenerList* GetList() noexcept
        {
            assert((list.get() != nullptr) &&
                   "Listener without valid list. Possible use after move!");

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
            EmitterList insert_list = EmitterList::CreateEmpty();
            auto [it, _] = mapping.insert(std::pair{Core::ClassID<E>::ID, insert_list});

            EventHandler* handler = new EventHandler(type_arg<E>{},
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
{
public:
    CommonEmitter()
        : Test::EventEmitter({EventType::ID})
    {}
};

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

                if(i % 11 == 0)
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