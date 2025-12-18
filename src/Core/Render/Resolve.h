#pragma once

#include "hrs/detail/winapi/winapi.h"
#include "Render.h"

namespace Render
{
    struct ResolveWin32Info
    {
        HWND window;
        HDC hdc;
        HINSTANCE instance;
    };

    class Resolve
    {
    public:
        virtual ~Resolve()
        {}

        //1. Init resolver
        virtual void Init(const ResolveWin32Info& info) = 0;

        //2. Get available contexts and select one of them
        virtual std::span<const ContextInitProperties> GetAvailableContexts() = 0;

        //3. Create context from selected properties
        virtual Render::Context* CreateContext(const SelectedContextDesc& desc) = 0;

        //4. Close the resolver(Init call does not important) -> call operator delete(should be overloaded from the implementation side, maybe just free some objects)
    };
};