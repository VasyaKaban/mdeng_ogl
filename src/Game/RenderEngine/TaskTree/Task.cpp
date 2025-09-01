#include "Task.h"

Task::~Task()
{}

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