#pragma once

#include "Render.h"

namespace Render
{
    class Resolve
    {
    public:
        virtual ~Resolve()
        {}

        //1. Retrieve available backends
        virtual std::span<const RenderBackendType> GetAvailableBackends() = 0;

        //2. Init resolver with selected backend
        virtual void Init(RenderBackend* backend) = 0;

        //3. Get available contexts and select one of them
        virtual std::span<const Render::ContextProperties> GetAvailableContexts() = 0;

        //4. Create context from selected properties
        virtual Render::Context* CreateContext(const SelectedContextDesc& desc) = 0;

        //5. (after the successfull Init) Close the resolver -> call operator delete(should be overloaded from the implementation side, maybe just free some objects)
    };
};