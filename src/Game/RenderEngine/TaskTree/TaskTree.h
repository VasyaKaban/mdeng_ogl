#pragma once

#include <memory>
#include <optional>
#include "hrs/non_creatable.hpp"
#include "hrs/multikey_map/multikey_map.hpp"
#include "Task.h"

class Context;

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

class TaskTree : hrs::non_copyable
{
public:
    using Priority = std::uint32_t;
    using Container =
        hrs::multikey_map<std::unique_ptr<Task>,
                          hrs::key<Priority, hrs::map_shared_key, std::less<Priority>>,
                          hrs::key<HashedString, hrs::map_unique_key, HashedStringComparator>>;

    template<typename T = Task>
    class TaskHandle
    {
    private:
        friend class TaskTree;
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

        T* operator*() noexcept
        {
            static_assert(std::same_as<T, Task> || std::derived_from<T, Task>);

            return static_cast<T*>(it.value().get());
        }

        const T* operator*() const noexcept
        {
            static_assert(std::same_as<T, Task> || std::derived_from<T, Task>);

            return static_cast<const T*>(it.value().get());
        }

        T* operator->() noexcept
        {
            return **this;
        }

        const T* operator->() const noexcept
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

    TaskTree() noexcept;
    ~TaskTree() = default;

    TaskHandle<> Insert(Priority priority, const HashedString& name, std::unique_ptr<Task>&& task);

    TaskHandle<> Insert(Priority priority, HashedString&& name, std::unique_ptr<Task>&& task);

    TaskHandle<>
    Insert(Priority priority, const HashedStringView& name, std::unique_ptr<Task>&& task);

    void Erase(TaskHandle<> handle);

    std::optional<TaskHandle<>> Find(const HashedStringView& name) noexcept;

    void Evaluate(EvaluateDesc& eval_desc);
    void Evaluate(EvaluateDesc& eval_desc, TaskHandle<> first_enabled);
    std::optional<TaskHandle<>> GetFirstEnabled() noexcept;
private:
    Container tasks;
};