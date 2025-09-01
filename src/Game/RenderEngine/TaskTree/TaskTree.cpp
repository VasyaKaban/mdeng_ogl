#include "TaskTree.h"
#include <stdexcept>

TaskTree::TaskTree() noexcept
{}

TaskTree::TaskHandle<>
TaskTree::Insert(Priority priority, const HashedString& name, std::unique_ptr<Task>&& task)
{
    auto [it, inserted, _] = tasks.insert(std::move(task), priority, name);
    if(!inserted)
        throw std::runtime_error("Failed to create new task");

    return it;
}

TaskTree::TaskHandle<>
TaskTree::Insert(Priority priority, HashedString&& name, std::unique_ptr<Task>&& task)
{
    auto [it, inserted, _] = tasks.insert(std::move(task), priority, std::move(name));
    if(!inserted)
        throw std::runtime_error("Failed to create new task");

    return it;
}

TaskTree::TaskHandle<>
TaskTree::Insert(Priority priority, const HashedStringView& name, std::unique_ptr<Task>&& task)
{
    auto [it, inserted, _] = tasks.insert(std::move(task), priority, name);
    if(!inserted)
        throw std::runtime_error("Failed to create new task");

    return it;
}

void TaskTree::Erase(TaskHandle<> handle)
{
    tasks.erase(handle.it);
}

std::optional<TaskTree::TaskHandle<>> TaskTree::Find(const HashedStringView& name) noexcept
{
    auto it = tasks.find<1>(name);
    if(it == tasks.get_entry<1>().end())
        return std::nullopt;

    return TaskHandle<>(it.rebind<0>());
}

void TaskTree::Evaluate(EvaluateDesc& eval_desc)
{
    for(auto [k, task]: tasks.get_entry<0>())
        task->Evaluate(eval_desc);
}

void TaskTree::Evaluate(EvaluateDesc& eval_desc, TaskHandle<> first_enabled)
{
    auto end = tasks.get_entry<0>().end();
    for(; first_enabled.it != end; first_enabled.it++)
        first_enabled->Evaluate(eval_desc);
}

std::optional<TaskTree::TaskHandle<>> TaskTree::GetFirstEnabled() noexcept
{
    for(auto it = tasks.get_entry<0>().begin(); it != tasks.get_entry<0>().end(); it++)
    {
        if(it.value()->IsEnabled())
            return TaskHandle<>(it);
    }

    return std::nullopt;
}
