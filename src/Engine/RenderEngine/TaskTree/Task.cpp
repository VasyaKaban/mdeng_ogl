#include "Task.h"

TaskBase::TaskBase(TaskBase* _parent, TaskKey&& key)
    : parent(_parent)
{
    if(parent)
    {
        root = parent->GetRoot();
        handle = parent->Insert(std::move(key), this);
    }
    else
    {
        root = static_cast<TaskRoot*>(this);
    }
}

TaskBase::~TaskBase()
{}

TaskPriority TaskBase::GetPriority() const noexcept
{
    if(!parent)
        return 0;

    return std::get<0>(handle.it.keys());
}

HashedStringView TaskBase::GetName() const noexcept
{
    if(!parent)
        return {};

    return std::get<1>(handle.it.keys());
}

TaskPriority TaskBase::GetNextPriority() const noexcept
{
    TaskPriority prior = GetPriority();
    if(prior == std::numeric_limits<TaskPriority>::max())
        return std::numeric_limits<TaskPriority>::max();

    return prior + 1;
}

TaskPriority TaskBase::GetPrevPriority() const noexcept
{
    TaskPriority prior = GetPriority();
    if(prior == 0)
        return 0;

    return prior - 1;
}

//called by object itself
void TaskBase::Erase()
{
    if(parent)
        parent->Drop(handle);
}

TaskRoot* TaskBase::GetRoot() const noexcept
{
    return root;
}

TaskBase* TaskBase::GetParent() const noexcept
{
    return parent;
}

TaskQueryResult TaskBase::Query(const std::filesystem::path& path) noexcept
{
    TaskQueryResult res{.obj = nullptr, .non_existent_index = 0};

    auto normal = path.lexically_normal();
    if(normal.empty())
        return res;

    TaskBase* current = this;
    std::size_t i = 0;
    for(auto it = normal.begin(); it != normal.end(); it++)
    {
        auto str = it->string();
        HashedStringView view(str);
        current = current->Find(view);
        if(!current)
        {
            res.non_existent_index = i;
            break;
        }

        i++;
    }

    res.obj = current;

    return res;
}

std::uint64_t TaskBase::DEBUG_GetID() const noexcept
{
    return reinterpret_cast<std::uint64_t>(this);
}

//TaskRoot
TaskRoot::TaskRoot(TaskBase* _parent, TaskKey&& key)
    : TaskBase(nullptr, {}),
      deffered_erase_tasks_first(nullptr),
      deffered_erase_tasks_last(nullptr)
{}

TaskRoot::~TaskRoot()
{}

TaskType TaskRoot::GetType() const noexcept
{
    return TaskType::Root;
}

TaskBase* TaskRoot::Find(const HashedStringView& name) noexcept
{
    auto it = tasks.find<1>(name);
    if(it == tasks.get_entry<1>().end())
        return nullptr;

    return it.value().get();
}

TaskBase::TaskHandle<> TaskRoot::Insert(TaskKey&& key, TaskBase* task)
{
    std::unique_ptr<TaskBase> ptr(task);
    auto [it, inserted, _] = tasks.insert(std::move(ptr), key.priority, std::move(key.name));
    if(!inserted)
        throw std::runtime_error("Failed to create new TaskBase");

    return it;
}

void TaskRoot::Drop(TaskHandle<> handle)
{
    tasks.erase(handle.it);
}

TaskBase::Container::node_t* TaskRoot::Decouple(TaskHandle<> handle)
{
    return tasks.decouple(handle.it);
}

void TaskRoot::AppendToDefferedErase(Container::node_t* node)
{
    if(deffered_erase_tasks_first == nullptr)
    {
        deffered_erase_tasks_first = node;
        deffered_erase_tasks_last = node;
    }
    else
    {
        deffered_erase_tasks_last->children[0].left = node;
        deffered_erase_tasks_last = node;
    }
}

void TaskRoot::Evaluate(const EvaluateDesc& eval_desc)
{
    EvaluateDesc after_begin_eval_desc = this->Begin(eval_desc);

    auto entry = tasks.get_entry<0>();
    for(auto it = entry.begin(); it != entry.end(); it++)
        it.value()->Evaluate(after_begin_eval_desc);

    this->End(after_begin_eval_desc);
}

void TaskRoot::ClearDefferedEraseTasks()
{
    auto& allocator = tasks.get_allocator();
    auto ptr = deffered_erase_tasks_first;
    while(ptr)
    {
        auto next_ptr = ptr->children[0].left->to_node();

        std::allocator_traits<Container::allocator_t>::destroy(allocator, ptr);
        std::allocator_traits<Container::allocator_t>::deallocate(allocator, ptr, 1);

        ptr = next_ptr;
    }

    deffered_erase_tasks_first = nullptr;
    deffered_erase_tasks_last = nullptr;
}

//TaskBranch
TaskBranch::TaskBranch(TaskBase* _parent, TaskKey&& key)
    : TaskBase(_parent, std::move(key))
{}

TaskBranch::~TaskBranch()
{}

TaskType TaskBranch::GetType() const noexcept
{
    return TaskType::Branch;
}

TaskBase* TaskBranch::Find(const HashedStringView& name) noexcept
{
    auto it = tasks.find<1>(name);
    if(it == tasks.get_entry<1>().end())
        return nullptr;

    return it.value().get();
}

TaskBase::TaskHandle<> TaskBranch::Insert(TaskKey&& key, TaskBase* task)
{
    std::unique_ptr<TaskBase> ptr(task);
    auto [it, inserted, _] = tasks.insert(std::move(ptr), key.priority, std::move(key.name));
    if(!inserted)
        throw std::runtime_error("Failed to create new TaskBase");

    return it;
}

void TaskBranch::Drop(TaskHandle<> handle)
{
    tasks.erase(handle.it);
}

TaskBase::Container::node_t* TaskBranch::Decouple(TaskHandle<> handle)
{
    return tasks.decouple(handle.it);
}

void TaskBranch::Evaluate(const EvaluateDesc& eval_desc)
{
    EvaluateDesc after_begin_eval_desc = this->Begin(eval_desc);

    auto entry = tasks.get_entry<0>();
    for(auto it = entry.begin(); it != entry.end(); it++)
        it.value()->Evaluate(after_begin_eval_desc);

    this->End(after_begin_eval_desc);
}

//TaskLeaf
TaskLeaf::TaskLeaf(TaskBase* _parent, TaskKey&& key)
    : TaskBase(_parent, std::move(key)),
      marked_for_erase_later(false)
{}

TaskLeaf::~TaskLeaf()
{}

TaskType TaskLeaf::GetType() const noexcept
{
    return TaskType::Leaf;
}

TaskBase* TaskLeaf::Find(const HashedStringView& name) noexcept
{
    return nullptr;
}

TaskBase::TaskHandle<> TaskLeaf::Insert(TaskKey&& key, TaskBase* task)
{
    throw std::runtime_error("Insert call on TaskLeaf object");
}

void TaskLeaf::Drop(TaskHandle<> handle)
{
    throw std::runtime_error("Drop call on TaskLeaf object");
}

TaskBase::Container::node_t* TaskLeaf::Decouple(TaskHandle<> handle)
{
    throw std::runtime_error("Decouple call on TaskLeaf object");
}

void TaskLeaf::Evaluate(const EvaluateDesc& eval_desc)
{
    EvaluateDesc after_begin_eval_desc = this->Begin(eval_desc);

    this->End(after_begin_eval_desc);
}

void TaskLeaf::EraseLater()
{
    if(!marked_for_erase_later)
    {
        auto node = parent->Decouple(handle);
        GetRoot()->AppendToDefferedErase(node);
        marked_for_erase_later = true;
    }
}