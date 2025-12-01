#pragma once

#include <memory>
#include <functional>
#include "hrs/os.hpp"
#include "hrs/non_creatable.hpp"

namespace Engine
{
    struct GameEngineInfo;

    class Game;

    class GameResolve
    {
    public:
        virtual ~GameResolve()
        {}

        virtual GameEngineInfo Init() = 0;

        virtual Game* CreateGame() = 0;
    };

    class Game
    {
    public:
        virtual ~Game()
        {}
    };

    constexpr inline auto GAME_RESOLVE_FUNCTION_NAME = "ResolveGame";
    using PFN_ResolveGame = GameResolve* (*)();

    class GameState : hrs::non_copyable
    {
    public:
        GameState(const std::filesystem::path& implementation_path);
        ~GameState() = default;
        GameState(GameState&&) = default;
        GameState& operator=(GameState&&) = default;

        GameEngineInfo Init();
        GameResolve* GetResolve() const noexcept;
    private:
        hrs::dynamic_library lib;
        std::unique_ptr<GameResolve> resolve;
    };
};