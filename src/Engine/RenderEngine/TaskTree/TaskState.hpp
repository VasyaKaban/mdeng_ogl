#pragma once

#include "Task.h"

template<typename P, typename C>
class StateTransfer : public TaskLeaf, public Events::EventListener<TaskEraseEvent>
{
    static_assert(std::is_base_of_v<TaskBase, P> && std::is_base_of_v<TaskBase, C>);
public:
    StateTransfer(P* _producer, C* _consumer, TaskBase* _parent, TaskKey&& key)
        : TaskLeaf(_parent, std::move(key)),
          producer(_producer),
          consumer(_consumer)
    {
        Events::Connect(this, producer, &StateTransfer::ProducerEraseHandle);
        Events::Connect(this, consumer, &StateTransfer::ConsumerEraseHandle);
    }

    ~StateTransfer() = default;

    virtual void End(const EvaluateDesc& eval_desc) override
    {
        //noop
    }

    virtual void DEBUG_DrawGraph(std::ostream& os) const override
    {
        os << std::format(
            "{} [label=\"Priority: {}; Name: {}\", fillcolor=\"aquamarine\", style=filled];\n",
            DEBUG_GetID(this),
            this->GetPriority(),
            this->GetName().GetStringView());
        os << std::format("{} -> {};\n", DEBUG_GetID(this->GetParent()), DEBUG_GetID(this));
        os << std::format("{} -> {} [style=dashed, label=\"Producer\"];\n",
                          DEBUG_GetID(this),
                          DEBUG_GetID(producer));
        os << std::format("{} -> {} [style=dashed, label=\"Consumer\"];\n",
                          DEBUG_GetID(this),
                          DEBUG_GetID(consumer));
    }
private:
    Events::HandlerAction ProducerEraseHandle(const TaskEraseEvent&)
    {
        EraseLater();
        return Events::HandlerAction::None;
    }

    Events::HandlerAction ConsumerEraseHandle(const TaskEraseEvent&)
    {
        EraseLater();
        return Events::HandlerAction::None;
    }
protected:
    P* producer;
    C* consumer;
};

#define FUNCTIONAL_STATE_TRANSFER(NAME, PRODUCER, CONSUMER, ...) \
    class NAME : public StateTransfer<PRODUCER, CONSUMER> \
    { \
    public: \
        NAME(PRODUCER* _producer, CONSUMER* _consumer, TaskBase* _parent, TaskKey&& key) \
            : StateTransfer<PRODUCER, CONSUMER>(_producer, _consumer, _parent, std::move(key)) \
        {} \
\
        ~NAME() = default; \
\
        virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) override \
        { \
            __VA_OPT__(__VA_ARGS__;) \
            return eval_desc; \
        }; \
    };

/*template<typename T>
concept TaskState = requires(T* ptr, const T* c_ptr) {
    ptr->Detach();
    { c_ptr->IsDetached() } noexcept -> std::same_as<bool>;
};

class DefaultTaskStateBase
{
public:
    DefaultTaskStateBase(bool _detached = false) noexcept
        : detached(_detached)
    {}
    ~DefaultTaskStateBase() = default;
    DefaultTaskStateBase(const DefaultTaskStateBase&) = default;
    DefaultTaskStateBase(DefaultTaskStateBase&&) = default;
    DefaultTaskStateBase& operator=(const DefaultTaskStateBase&) = default;
    DefaultTaskStateBase& operator=(DefaultTaskStateBase&&) = default;

    void Detach() noexcept
    {
        detached = true;
    }

    bool IsDeatched() const noexcept
    {
        return detached;
    }
private:
    bool detached;
};

template<typename T>
class TaskStateOwner : hrs::non_copyable
{
    static_assert(TaskState<T>);
public:
    TaskStateOwner() = default;
    TaskStateOwner(T* _state) noexcept
        : state(_state)
    {}

    ~TaskStateOwner()
    {
        if(state)
            state->Detach();
    }

    TaskStateOwner(TaskStateOwner&&) = default;
    TaskStateOwner& operator=(TaskStateOwner&&) = default;

    T* operator->() const noexcept
    {
        return state.get();
    }

    T& operator*() const noexcept
    {
        return *state.get();
    }

    hrs::rc_ptr<T> Get() const noexcept
    {
        return state;
    }
private:
    hrs::rc_ptr<T> state;
};*/
