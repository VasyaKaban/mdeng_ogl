#include "Resolve.h"
#include "../Context/Context.h"
#include <stdexcept>
#include "WGL/WGL.h"

namespace OpenGL
{
    extern "C" Render::Resolve* RenderResolve()
    {
        return new Resolve;
    }

    Resolve::Resolve()
        : loader(nullptr),
          ctx(nullptr)
    {}

    Resolve::~Resolve()
    {
        delete ctx;
        delete loader;
    }

    void Resolve::Init(const Render::ResolveWin32Info& info)
    {
        if(loader != nullptr || ctx != nullptr)
            throw std::runtime_error("Cannot init already inited resolve object");

        loader = new WGL(info);
    }

    std::span<const Render::ContextInitProperties> Resolve::GetAvailableContexts()
    {
        if(!loader)
            throw std::runtime_error(
                "Cannot get available context because resolve has not been initialized yet");

        return std::span{&loader->GetInitProperties(), 1};
    }

    Render::Context* Resolve::CreateContext(const Render::SelectedContextDesc& desc)
    {
        if(ctx)
            throw std::runtime_error("Cannot createnew context due to the OpenGL restrictions");

        ctx = loader->CreateContext(desc);

        return ctx;
    }
};