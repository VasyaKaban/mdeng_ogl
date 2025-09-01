#pragma once

#include "TaskTree.h"

template<std::derived_from<Task> T>
class TaskTreeHolder : hrs::non_copyable
{
public:
    TaskTreeHolder() noexcept
    {}

    ~TaskTreeHolder() = default;
    TaskTreeHolder(TaskTreeHolder&&) = default;
    TaskTreeHolder& operator=(TaskTreeHolder&&) = default;

    TaskTree::TaskHandle<>
    Insert(TaskTree::Priority priority, const HashedString& name, std::unique_ptr<T>&& task)
    {
        return task_tree.Insert(priority, name, std::move(task));
    }

    TaskTree::TaskHandle<>
    Insert(TaskTree::Priority priority, HashedString&& name, std::unique_ptr<T>&& task)
    {
        return task_tree.Insert(priority, std::move(name), std::move(task));
    }

    TaskTree::TaskHandle<>
    Insert(TaskTree::Priority priority, const HashedStringView& name, std::unique_ptr<T>&& task)
    {
        return task_tree.Insert(priority, name, std::move(task));
    }

    void Erase(TaskTree::TaskHandle<> handle)
    {
        task_tree.Erase(handle);
    }

    std::optional<TaskTree::TaskHandle<>> Find(const HashedStringView& name) noexcept
    {
        return task_tree.Find(name);
    }
protected:
    TaskTree task_tree;
};