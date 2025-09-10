#include "Task.h"

Task::Task(Task* _parent, TaskKey&& key, bool _is_enabled) noexcept
    : parent(_parent),
      is_enabled(_is_enabled)
{
    if(parent)
        handle = parent->Insert(std::move(key), this);
}

//called by last rc_ptr
Task::~Task()
{
    for(auto [k, v]: tasks.get_entry<0>())
        v->Detach();
}

//called by object itself
void Task::Erase()
{
    if(parent)
    {
        Task* p_parent = parent;
        this->Detach();
        p_parent->Drop(handle);
    }
}

hrs::rc_ptr<Task> Task::Find(const HashedStringView& name) noexcept
{
    auto it = tasks.find<1>(name);
    if(it == tasks.get_entry<1>().end())
        return hrs::rc_ptr<Task>();

    return it.value();
}

void Task::Evaluate(const EvaluateDesc& eval_desc)
{
    EvaluateDesc after_begin_eval_desc = this->Begin(eval_desc);

    auto entry = tasks.get_entry<0>();
    auto start_it = entry.end();
    for(auto it = entry.begin(); it != entry.end(); it++)
    {
        if(it.value()->IsEnabled())
        {
            start_it = it;
            break;
        }
    }

    if(start_it != entry.end())
    {
        for(; start_it != entry.end(); start_it++)
            start_it.value()->Evaluate(after_begin_eval_desc);
    }

    this->End(after_begin_eval_desc);
}

bool Task::IsEnabled() const noexcept
{
    return is_enabled;
}

void Task::Enable()
{
    is_enabled = true;
}

void Task::Disable()
{
    is_enabled = false;
}

Task* Task::GetParent() const noexcept
{
    return parent;
}

bool Task::IsDetached() const noexcept
{
    return parent == nullptr;
}

void Task::Detach() noexcept
{
    parent = nullptr;
}

Task::TaskHandle<> Task::Insert(TaskKey&& key, Task* task)
{
    hrs::rc_ptr<Task> task_ptr(task);
    auto [it, inserted, _] = tasks.insert(task_ptr, key.priority, std::move(key.name));
    if(!inserted)
        throw std::runtime_error("Failed to create new task");

    return it;
}

void Task::Drop(Task::TaskHandle<> handle)
{
    tasks.erase(handle.it);
}