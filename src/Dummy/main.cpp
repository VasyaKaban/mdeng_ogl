#include <iostream>
#include <vector>
#include <format>
#include "Core/Events/Events.h"

struct EventType : Core::ClassID<EventType>
{
    int data;
};

class CommonEmitter : public Core::ReservedEventEmitter<EventType>
{};

class CommonListener : public Core::EventListener
{};

int main(int argc, char** argv)
{
    constexpr std::size_t SIZE = 100;

    CommonEmitter emitter;
    CommonListener listener;

    std::size_t num = 0;

    std::vector<Core::EventHandlerRef> refs;
    refs.reserve(SIZE);
    for(std::size_t i = 0; i < SIZE; i++)
    {
        auto ref = emitter.Connect<EventType>(
            [&num, i](const EventType& event) -> Core::EventHandlerResult
            {
                std::cout << std::format("Num: {}; Index: {}; data: {}", num, i, event.data)
                          << std::endl;

                num++;

                if(i % 11 == 0)
                    return Core::EventHandlerResult::Erase;

                return Core::EventHandlerResult::None;
            },
            /*&listener,*/ nullptr,
            Core::EventHandlerState::Enabled);

        refs.push_back(ref);
    }

    num = 0;
    emitter.Emit(EventType{.data = 42});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        if(i % 7 == 0)
            refs[i].Disable();
    }

    num = 0;
    emitter.Emit(EventType{.data = 123});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        if(i % 9 == 0)
            refs[i].Disconnect();
    }

    num = 0;
    emitter.Emit(EventType{.data = 1024});

    for(std::size_t i = 0; i < refs.size(); i++)
    {
        refs[i].Disconnect();
    }

    num = 0;
    emitter.Emit(EventType{.data = 0});
}