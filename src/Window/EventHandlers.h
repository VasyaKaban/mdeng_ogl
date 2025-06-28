#pragma once

#include <functional>

struct EventHandlers
{
    using CloseHandler = void();

    std::function<CloseHandler> close_handler;
};