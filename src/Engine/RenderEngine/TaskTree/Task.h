#pragma once

#include <string>
#include <filesystem>
#include "hrs/multikey_map/multikey_map.hpp"
#include "Core/Render/Render.h"
#include "Core/Events/Events.hpp"

#include <iostream>
#include <format>

class HashedString;

class HashedStringView
{
    friend class HashedString;
    HashedStringView(std::string_view v, std::size_t _hash) noexcept
        : str(v),
          hash(_hash)
    {}
public:
    HashedStringView() = default;
    HashedStringView(std::string_view v)
        : str(v),
          hash(std::hash<std::string_view>{}(v))
    {}
    ~HashedStringView() = default;
    HashedStringView(const HashedStringView&) = default;
    HashedStringView(HashedStringView&&) = default;
    HashedStringView& operator=(const HashedStringView&) = default;
    HashedStringView& operator=(HashedStringView&&) = default;

    std::string_view GetStringView() const noexcept
    {
        return str;
    }

    std::size_t GetHash() const noexcept
    {
        return hash;
    }
private:
    std::string_view str;
    std::size_t hash;
};

class HashedString
{
public:
    HashedString() = default;

    HashedString(std::string_view v)
        : str(v),
          hash(std::hash<std::string_view>{}(v))
    {}

    HashedString(const HashedStringView& v)
        : str(v.GetStringView()),
          hash(v.GetHash())
    {}

    ~HashedString() = default;
    HashedString(const HashedString&) = default;
    HashedString(HashedString&&) = default;
    HashedString& operator=(const HashedString&) = default;
    HashedString& operator=(HashedString&&) = default;

    HashedString& operator=(const HashedStringView& v)
    {
        str = v.GetStringView();
        hash = v.GetHash();

        return *this;
    }

    const std::string& GetString() const noexcept
    {
        return str;
    }

    std::size_t GetHash() const noexcept
    {
        return hash;
    }

    operator HashedStringView() const& noexcept
    {
        return HashedStringView(str, hash);
    }
private:
    std::string str;
    std::size_t hash;
};

struct HashedStringComparator
{
    bool operator()(const HashedString& str1, const HashedString& str2) const noexcept
    {
        bool hash_cmp = (str1.GetHash() < str2.GetHash());
        if(hash_cmp)
            return true;

        return str1.GetString() < str2.GetString();
    }

    bool operator()(const HashedString& str1, const HashedStringView& str2) const noexcept
    {
        bool hash_cmp = (str1.GetHash() < str2.GetHash());
        if(hash_cmp)
            return true;

        return str1.GetString() < str2.GetStringView();
    }

    bool operator()(const HashedStringView& str1, const HashedString& str2) const noexcept
    {
        bool hash_cmp = (str1.GetHash() < str2.GetHash());
        if(hash_cmp)
            return true;

        return str1.GetStringView() < str2.GetString();
    }
};

struct EvaluateDesc
{
    Render::CommandBuffer* cmd;
    Render::Pipeline* pipeline;
};

struct TaskEraseEvent
{};

enum class TaskType
{
    Root,
    Branch,
    Leaf
};

template<typename T>
class Task;

using TaskPriority = std::uint32_t;
struct TaskKey
{
    TaskPriority priority;
    HashedString name;
};

class TaskBase;

struct TaskQueryResult
{
    TaskBase* obj;
    std::uint32_t non_existent_index;
};

class TaskRoot;

class TaskBase : public Events::EventEmitter<TaskEraseEvent>
{
public:
    using Container =
        hrs::multikey_map<std::unique_ptr<TaskBase>,
                          hrs::key<TaskPriority, hrs::map_shared_key, std::less<TaskPriority>>,
                          hrs::key<HashedString, hrs::map_unique_key, HashedStringComparator>>;

    template<typename T = TaskBase>
    class TaskHandle
    {
    private:
        friend class TaskBase;
        friend class TaskRoot;
        friend class TaskBranch;
        TaskHandle(Container::entry_t<0>::iterator _it) noexcept
            : it(_it)
        {}
    public:
        TaskHandle() = default;
        ~TaskHandle() = default;
        TaskHandle(const TaskHandle&) = default;
        TaskHandle(TaskHandle&&) = default;
        TaskHandle& operator=(const TaskHandle&) = default;
        TaskHandle& operator=(TaskHandle&&) = default;

        T* operator*() const noexcept
        {
            static_assert(std::same_as<T, TaskBase> || std::derived_from<T, TaskBase>);

            return static_cast<T*>(it.value().get());
        }

        T* operator->() const noexcept
        {
            return **this;
        }

        template<typename U>
        TaskHandle<U> Rebind() noexcept
        {
            return TaskHandle<U>(it);
        }
    private:
        Container::entry_t<0>::iterator it;
    };

    TaskBase(TaskBase* _parent, TaskKey&& key);
    virtual ~TaskBase();

    virtual TaskType GetType() const noexcept = 0;

    TaskPriority GetPriority() const noexcept;
    HashedStringView GetName() const noexcept;
    TaskPriority GetNextPriority() const noexcept;
    TaskPriority GetPrevPriority() const noexcept;

    void Erase();

    virtual TaskBase* Find(const HashedStringView& name) noexcept = 0;
    virtual TaskHandle<> Insert(TaskKey&& key, TaskBase* task) = 0;
    virtual void Drop(TaskHandle<> handle) = 0;
    virtual Container::node_t* Decouple(TaskHandle<> handle) = 0;

    virtual void Evaluate(const EvaluateDesc& eval_desc) = 0;

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) = 0;
    virtual void End(const EvaluateDesc& eval_desc) = 0;

    TaskRoot* GetRoot() const noexcept;
    TaskBase* GetParent() const noexcept;

    TaskQueryResult Query(const std::filesystem::path& path) noexcept;

    template<typename T>
    requires std::is_base_of_v<TaskBase, T>
    T* As() noexcept
    {
        return static_cast<T*>(this);
    }

    virtual void DEBUG_DrawGraph(std::ostream& os) const = 0;

    std::uint64_t DEBUG_GetID() const noexcept;
private:
    template<typename T>
    friend class TaskRootWrapper;

    template<typename T>
    friend class TaskBranchWrapper;

    friend class TaskLeaf;

    virtual void task_dummy_virtual() const noexcept = 0;
private:
    TaskBase* parent;
    TaskRoot* root;
    TaskHandle<> handle;
};

class TaskRoot : public TaskBase
{
    template<typename T>
    friend class TaskRootWrapper;
public:
    TaskRoot(TaskBase* _parent, TaskKey&& key);
    virtual ~TaskRoot() override;

    virtual TaskType GetType() const noexcept override;

    virtual TaskBase* Find(const HashedStringView& name) noexcept override;
    virtual TaskHandle<> Insert(TaskKey&& key, TaskBase* task) override;
    virtual void Drop(TaskHandle<> handle) override;
    virtual Container::node_t* Decouple(TaskHandle<> handle) override;
    void AppendToDefferedErase(Container::node_t* node);

    virtual void Evaluate(const EvaluateDesc& eval_desc) override;

    void ClearDefferedEraseTasks();
private:
    template<typename T>
    friend class Task;

    friend class TaskBase;

    Container tasks;

    Container::node_t* deffered_erase_tasks_first;
    Container::node_t* deffered_erase_tasks_last;
};

class TaskBranch : public TaskBase
{
    template<typename T>
    friend class TaskBranchWrapper;
public:
    TaskBranch(TaskBase* _parent, TaskKey&& key);
    virtual ~TaskBranch() override;

    virtual TaskType GetType() const noexcept override;

    virtual TaskBase* Find(const HashedStringView& name) noexcept override;
    virtual TaskHandle<> Insert(TaskKey&& key, TaskBase* task) override;
    virtual void Drop(TaskHandle<> handle) override;
    virtual Container::node_t* Decouple(TaskHandle<> handle) override;

    virtual void Evaluate(const EvaluateDesc& eval_desc) override;
private:
    template<typename T>
    friend class Task;

    Container tasks;
};

//Do not call Erase or EraseLater in task tree lookup
//Do not call Erase in event listener polling. You can call EraseLater in event callback but from this moment you cannot rely on parent consistency. Also you can call Erase method on task which listener method is not being called right now.
class TaskLeaf : public TaskBase
{
public:
    TaskLeaf(TaskBase* _parent, TaskKey&& key);
    virtual ~TaskLeaf() override;

    virtual TaskType GetType() const noexcept override;

    virtual TaskBase* Find(const HashedStringView& name) noexcept override;
    virtual TaskHandle<> Insert(TaskKey&& key, TaskBase* task) override;
    virtual void Drop(TaskHandle<> handle) override;
    virtual Container::node_t* Decouple(TaskHandle<> handle) override;

    virtual void Evaluate(const EvaluateDesc& eval_desc) override;

    void EraseLater();
private:
    bool marked_for_erase_later;
};

template<typename T>
class TaskRootWrapper : public T
{
    static_assert(std::is_base_of_v<TaskRoot, T>);
public:
    using T::T;

    virtual ~TaskRootWrapper() override
    {
        this->ClearDefferedEraseTasks();
        TaskRoot::tasks.clear();

        Events::Emit(this, TaskEraseEvent{});
    }

    virtual void DEBUG_DrawGraph(std::ostream& os) const override
    {
        os << "digraph G{\n";

        auto name = this->GetName().GetStringView();
        if(name.empty())
            name = typeid(T).name();
        os << std::format(
            "{} [label=\"Priority: {}; Name: {}\", fillcolor=\"tomato\", style=filled];\n",
            this->DEBUG_GetID(),
            this->GetPriority(),
            name);

        auto entry = TaskRoot::tasks.get_entry<0>();
        for(auto it = entry.begin(); it != entry.end(); it++)
            it.value()->DEBUG_DrawGraph(os);

        os << "}";
    }
private:
    virtual void task_dummy_virtual() const noexcept override
    {}
};

template<typename T>
class TaskBranchWrapper : public T
{
    static_assert(std::is_base_of_v<TaskBranch, T>);
public:
    using T::T;

    virtual ~TaskBranchWrapper() override
    {
        TaskBranch::tasks.clear();

        Events::Emit(this, TaskEraseEvent{});
    }

    virtual void DEBUG_DrawGraph(std::ostream& os) const override
    {
        os << std::format(
            "{} [label=\"Priority: {}; Name: {}\", fillcolor=\"gold1\", style=filled];\n",
            DEBUG_GetID(this),
            this->GetPriority(),
            this->GetName().GetStringView());
        os << std::format("{} -> {};\n", DEBUG_GetID(this->GetParent()), DEBUG_GetID(this));

        auto entry = TaskBranch::tasks.get_entry<0>();
        for(auto it = entry.begin(); it != entry.end(); it++)
            it.value()->DEBUG_DrawGraph(os);
    }
private:
    virtual void task_dummy_virtual() const noexcept override
    {}
};

template<typename T>
class TaskLeafWrapper : public T
{
    static_assert(std::is_base_of_v<TaskLeaf, T>);
public:
    using T::T;

    virtual ~TaskLeafWrapper() override
    {
        Events::Emit(this, TaskEraseEvent{});
    }

    virtual void DEBUG_DrawGraph(std::ostream& os) const override
    {
        os << std::format(
            "{} [label=\"Priority: {}; Name: {}\", fillcolor=\"springgreen\", style=filled];\n",
            DEBUG_GetID(this),
            this->GetPriority(),
            this->GetName().GetStringView());
        os << std::format("{} -> {};\n", DEBUG_GetID(this->GetParent()), DEBUG_GetID(this));
    }
private:
    virtual void task_dummy_virtual() const noexcept override
    {}
};

#pragma warning("FIX THIS!")
#define CHECK_TASK_IS_READY(CLASS)
/*
#define CHECK_TASK_IS_READY(CLASS) \
    static_assert( \
        !std::is_void_v<std::conditional_t< \
            std::is_base_of_v<TaskRoot, CLASS>, \
            std::conditional_t<!std::is_abstract_v<TaskRootWrapper<CLASS>>, int, void>, \
            std::conditional_t< \
                std::is_base_of_v<TaskBranch, CLASS>, \
                std::conditional_t<!std::is_abstract_v<TaskBranchWrapper<CLASS>>, int, void>, \
                std::conditional_t<std::is_base_of_v<TaskLeaf, CLASS>, \
                                   std::conditional_t<!std::is_abstract_v<CLASS>, int, void>, \
                                   void>>>>);
*/

//#define CHECK_TASK_IS_READY(CLASS) static_assert(     !std::is_abstract_v<Task<CLASS>>);

/*
iface
T
tasks

*/

/*
EVENTS:

CLASS:
    void EmitEvent(const EventType& event)
    {
        for(auto& listener : listeners)
        {
            listener.Handle(event);
        }
    }

    Handle Listen(Object* obj, const EventType& event)
    {

    }

    void Handle(const Event& event)
    {
    }
*/