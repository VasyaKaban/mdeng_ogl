#pragma once

class TaskTree;
class CommandBuffer;
class Pipeline;

struct EvaluateDesc
{
    CommandBuffer* cmd;
    Pipeline* pipeline;
};

class Task
{
public:
    virtual ~Task() = 0;

    virtual void Evaluate(EvaluateDesc& eval_desc) = 0;

    virtual bool IsEnabled() const noexcept;
    virtual void Enable();
    virtual void Disable();

    virtual Task* GetParent() noexcept = 0;
    virtual const Task* GetParent() const noexcept = 0;
protected:
    bool is_enabled;
};