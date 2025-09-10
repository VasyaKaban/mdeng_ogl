#pragma once

#include <string>
#include <optional>
#include "hrs/rc.hpp"
#include "hrs/multikey_map/multikey_map.hpp"
#include "Core/Render/Render.h"

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

class Task : public hrs::rc
{
public:
    using Priority = std::uint32_t;
    struct TaskKey
    {
        Priority priority;
        HashedString name;
    };

    using Container =
        hrs::multikey_map<hrs::rc_ptr<Task>,
                          hrs::key<Priority, hrs::map_shared_key, std::less<Priority>>,
                          hrs::key<HashedString, hrs::map_unique_key, HashedStringComparator>>;

    template<typename T = Task>
    class TaskHandle
    {
    private:
        friend class Task;
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
            static_assert(std::same_as<T, Task> || std::derived_from<T, Task>);

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

    Task(Task* _parent, TaskKey&& key, bool _is_enabled = true) noexcept;
    virtual ~Task();

    void Erase();
    hrs::rc_ptr<Task> Find(const HashedStringView& name) noexcept;

    void Evaluate(const EvaluateDesc& eval_desc);

    virtual EvaluateDesc Begin(const EvaluateDesc& eval_desc) = 0;
    virtual void End(const EvaluateDesc& eval_desc) = 0;

    virtual bool IsEnabled() const noexcept;
    virtual void Enable();
    virtual void Disable();

    virtual Task* GetParent() const noexcept;

    virtual bool IsDetached() const noexcept;
private:
    virtual void Detach() noexcept;
    TaskHandle<> Insert(TaskKey&& key, Task* task);
    void Drop(TaskHandle<> handle);
protected:
    Task* parent;
    bool is_enabled;
private:
    TaskHandle<> handle;
    Container tasks;
};
