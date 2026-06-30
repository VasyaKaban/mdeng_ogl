#pragma once

#include "Span.hpp"
#include "StringView.hpp"
#include "String.hpp"
#include "Sequence.hpp"
#include "System.h"

enum class EntryResult : Int32
{
    Success = 0,
    Error = 1
};

EntryResult EntryPoint(Core::Span<const Core::StringView> arguments);

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show)
{
    Core::System::SetCmdShow(cmd_show);
    Core::System::SetMainThreadID();

    int argc = 0;
    auto args = CommandLineToArgvW(cmd_line, &argc);

    if(args == nullptr)
        exit(-1);

    Core::Sequence<Core::String> string_arguments(argc);

    for(int i = 0; i < argc; i++)
        string_arguments.Push(Core::String(args[i], wcslen(args[i])));

    LocalFree(args);

    Core::Sequence<Core::StringView> arguments;
    arguments.Reserve(string_arguments.GetSize());
    for(DeviceSize i = 0; i < string_arguments.GetSize(); i++)
        arguments.Push(string_arguments[i]);

    EntryResult result = EntryPoint(Core::Span(arguments.GetData(), arguments.GetSize()));
    return static_cast<int>(result);
}
#elif defined(linux)
int main(int argc, char** argv)
{
    Core::Sequence<Core::StringView> arguments(argc);
    for(int i = 0; int < argc; i++)
        arguments[i] = Core::StringView(reinterpret_cast<const UTF8Char*>(argv[i]), strlen(argv[i]));

    EntryResult result = EntryPoint(Core::Span(arguments.GetData(), arguments.GetSize()));
    return static_cast<int>(result);
}
#endif