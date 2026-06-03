#pragma once

#include <span>
#include <vector>
#include <string_view>
#include "System.h"

int EntryPoint(std::span<const std::string_view> arguments);

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show)
{
    Core::System::SetCmdShow(cmd_show);
    Core::System::SetMainThreadID();

    int argc = 0;
    auto args = CommandLineToArgvW(cmd_line, &argc);

    if(args == nullptr)
        std::terminate();

    std::vector<std::string> string_arguments;
    string_arguments.reserve(1 + argc);

    if(args)
    {
        for(int i = 0; i < argc; i++)
            string_arguments.push_back(Core::System::WideToUTF8(std::wstring_view(args[0])));

        LocalFree(args);
    }
    else
        string_arguments.push_back(""); //empty app path

    std::vector<std::string_view> arguments;
    arguments.reserve(string_arguments.size());
    for(std::size_t i = 0; i < string_arguments.size(); i++)
        arguments.push_back(string_arguments[i]);

    return EntryPoint(std::span{arguments.data(), arguments.size()});
}
#elif defined(linux)
int main(int argc, char** argv)
{
    std::vector<std::string_view> arguments(argc, std::string_view{});
    for(int i = 0; int < argc; i++)
        arguments[i] = std::string_view(argv[i]);

    return EntryPoint(std::span{arguments.data(), arguments.size()});
}
#endif