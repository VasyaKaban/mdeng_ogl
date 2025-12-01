#pragma once

#include "Engine/Game/Game.h"
#include "Config/Config.h"

class GameResolve : public Engine::GameResolve, hrs::non_copyable, hrs::non_movable
{
public:
    GameResolve();

    virtual ~GameResolve() override;

    virtual Engine::GameEngineInfo Init() override;

    virtual Engine::Game* CreateGame() override;

    static void operator delete(void* ptr) noexcept;
private:
    Config config;
};

class Game : public Engine::Game, hrs::non_copyable, hrs::non_movable
{
public:
    Game();

    virtual ~Game() override;
};