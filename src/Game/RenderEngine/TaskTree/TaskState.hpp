#pragma once

#include "hrs/rc.hpp"
#include "hrs/non_creatable.hpp"

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
