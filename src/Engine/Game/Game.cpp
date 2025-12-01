#include "Game.h"
#include "Engine/Engine.h"

namespace Engine
{
    GameState::GameState(const std::filesystem::path& implementation_path)
    {
        auto res = lib.open(implementation_path);
        if(res.has_value())
            throw res.value();

        auto resolve_pfn =
            reinterpret_cast<PFN_ResolveGame>(lib.get_proc_address(GAME_RESOLVE_FUNCTION_NAME));

        resolve.reset(resolve_pfn());
    }

    GameEngineInfo GameState::Init()
    {
        return resolve->Init();
    }

    GameResolve* GameState::GetResolve() const noexcept
    {
        return resolve.get();
    }
};