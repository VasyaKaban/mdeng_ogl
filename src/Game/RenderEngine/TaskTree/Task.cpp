#include "Task.h"
#include "../RenderEngine.h"

TaskBase::TaskBase(TaskBase* _parent, TaskKey&& key) noexcept
    : parent(_parent)
{
    if(parent)
    {
        root = parent->GetRoot();
        handle = parent->Insert(std::move(key), this);
    }
    else
    {
        root = static_cast<Task<RenderEngine>*>(this);
    }
}

//called by last rc_ptr
TaskBase::~TaskBase()
{}

//called by object itself
void TaskBase::Erase()
{
    if(parent)
        parent->Drop(handle);
}

TaskBase* TaskBase::Find(const HashedStringView& name) noexcept
{
    auto it = tasks.find<1>(name);
    if(it == tasks.get_entry<1>().end())
        return nullptr;

    return it.value().get();
}

void TaskBase::Evaluate(const EvaluateDesc& eval_desc)
{
    EvaluateDesc after_begin_eval_desc = this->Begin(eval_desc);

    auto entry = tasks.get_entry<0>();
    for(auto it = entry.begin(); it != entry.end(); it++)
        it.value()->Evaluate(after_begin_eval_desc);

    this->End(after_begin_eval_desc);
}

RenderEngine* TaskBase::GetRoot() const noexcept
{
    return root;
}

TaskBase* TaskBase::GetParent() const noexcept
{
    return parent;
}

TaskBase::TaskHandle<> TaskBase::Insert(TaskKey&& key, TaskBase* task)
{
    std::unique_ptr<TaskBase> ptr(task);
    auto [it, inserted, _] = tasks.insert(std::move(ptr), key.priority, std::move(key.name));
    if(!inserted)
        throw std::runtime_error("Failed to create new TaskBase");

    return it;
}

void TaskBase::Drop(TaskBase::TaskHandle<> handle)
{
    tasks.erase(handle.it);
}