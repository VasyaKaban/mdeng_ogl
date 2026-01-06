#include "Events.hpp"

namespace Core
{
    EventHandlerAction EventHandlerNode::operator()(const void* event)
    {
        return func_caller(func.get(), event);
    }

    void EventHandlerNode::Disable() noexcept
    {
        enabled = false;
    }

    void EventHandlerNode::Enable() noexcept
    {
        enabled = true;
    }

    bool EventHandlerNode::IsEnabled() const noexcept
    {
        return enabled;
    }

    //null <- List <-> Node -> List
    void EventHandlerNode::DecoupleEmitterList() noexcept
    {
        bool is_first = (this->EmitterNode::prev->prev == nullptr);
        bool is_last = (this->EmitterNode::next->prev == nullptr);

        if(is_first && is_last)
        {
            EmitterNodeList* list = static_cast<EmitterNodeList*>(this->EmitterNode::prev);
            list->next = nullptr;
            list->last = nullptr;
        }
        else if(is_first)
        {
            EmitterNodeList* list = static_cast<EmitterNodeList*>(this->EmitterNode::prev);
            list->next = this->EmitterNode::next;
            this->EmitterNode::next->prev = list;
        }
        else if(is_last)
        {
            EmitterNodeList* list = static_cast<EmitterNodeList*>(this->EmitterNode::next);
            list->last = this->EmitterNode::prev;
            this->EmitterNode::prev->next = list;
        }
        else
        {
            auto prev_node = this->EmitterNode::prev;
            auto next_node = this->EmitterNode::next;

            prev_node->next = next_node;
            next_node->prev = prev_node;
        }
    }

    void EventHandlerNode::DecoupleListenerList() noexcept
    {
        if(this->ListenerNode::prev == nullptr)
            return;

        bool is_first = (this->ListenerNode::prev->prev == nullptr);
        bool is_last = (this->ListenerNode::next->prev == nullptr);

        if(is_first && is_last)
        {
            ListenerNodeList* list = static_cast<ListenerNodeList*>(this->ListenerNode::prev);
            list->next = nullptr;
            list->last = nullptr;
        }
        else if(is_first)
        {
            ListenerNodeList* list = static_cast<ListenerNodeList*>(this->ListenerNode::prev);
            list->next = this->ListenerNode::next;
            this->ListenerNode::next->prev = list;
        }
        else if(is_last)
        {
            ListenerNodeList* list = static_cast<ListenerNodeList*>(this->ListenerNode::next);
            list->last = this->ListenerNode::prev;
            this->ListenerNode::prev->next = list;
        }
        else
        {
            auto prev_node = this->ListenerNode::prev;
            auto next_node = this->ListenerNode::next;

            prev_node->next = next_node;
            next_node->prev = prev_node;
        }
    }

    EventHandlerRef::EventHandlerRef(EventHandlerNode* _node) noexcept
        : node(_node)
    {}

    void EventHandlerRef::Disable() noexcept
    {
        node->Disable();
    }

    void EventHandlerRef::Enable() noexcept
    {
        node->Enable();
    }

    bool EventHandlerRef::IsEnabled() const noexcept
    {
        return node->IsEnabled();
    }

    void EventHandlerRef::Disconnect() noexcept
    {
        node->DecoupleEmitterList();
        node->DecoupleListenerList();
        delete node;
    }
};