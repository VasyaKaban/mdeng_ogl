#pragma once

#include <string>
#include "hrs/multikey_map/multikey_map.hpp"
#include "Core/Render/Render.h"
#include "Core/Events/Events.hpp"

class HashedStringView
{
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

class RenderEngine;

struct TaskEraseEvent
{};

//struct TaskEnabledStateSwitchEvent
//{};

template<typename T>
class Task;

class TaskBase : public Events::EventEmitter<TaskEraseEvent> /*,
                 public Events::EventEmitter<TaskEnabledStateSwitchEvent>*/
{
public:
    using Priority = std::uint32_t;
    struct TaskKey
    {
        Priority priority;
        HashedString name;
    };

    using Container =
        hrs::multikey_map<std::unique_ptr<TaskBase>,
                          hrs::key<Priority, hrs::map_shared_key, std::less<Priority>>,
                          hrs::key<HashedString, hrs::map_unique_key, HashedStringComparator>>;

    template<typename T = TaskBase>
    class TaskHandle
    {
    private:
        friend class TaskBase;
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

    TaskBase(TaskBase* _parent, TaskKey&& key, bool _enabled = true) noexcept;
    virtual ~TaskBase();

    void Erase();
    TaskBase* Find(const HashedStringView& name) noexcept;

    void Evaluate(const EvaluateDesc& eval_desc);

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) = 0;
    virtual void End(const EvaluateDesc& eval_desc) = 0;

    //virtual void Enable();
    //virtual void Disable();
    //bool IsEnabled() const noexcept;

    RenderEngine* GetRoot() const noexcept;
    virtual TaskBase* GetParent() const noexcept;
private:
    template<typename T>
    friend class Task;

    virtual void task_dummy_virtual() const noexcept = 0;

    TaskHandle<> Insert(TaskKey&& key, TaskBase* task);
    void Drop(TaskHandle<> handle);
protected:
    TaskBase* parent;
private:
    //std::uint32_t enabled_counter; //0 -> enabled; >= 1 -> disabled
    RenderEngine* root;
    TaskHandle<> handle;
    Container tasks;
};

template<typename T>
class Task : public T
{
    static_assert(std::is_base_of_v<TaskBase, T>);
public:
    using T::T;

    virtual ~Task() override
    {
        Events::Emit(this, TaskEraseEvent{});
        TaskBase::tasks.clear();
    }
private:
    virtual void task_dummy_virtual() const noexcept override
    {}
};

#define CHECK_TASK_IS_READY(CLASS) static_assert(!std::is_abstract_v<Task<CLASS>>);

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