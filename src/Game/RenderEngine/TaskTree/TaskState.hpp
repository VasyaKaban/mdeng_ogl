#pragma once

#include "hrs/rc.hpp"
#include "hrs/non_creatable.hpp"

template<typename T>
concept TaskState = requires(T* ptr) { ptr->Detach(); };

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
};
