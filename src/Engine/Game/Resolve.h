#pragma once

#include "Game.h"

class GameResolve
{
public:
    virtual ~GameResolve()
    {}

    virtual Game* CreateGame() = 0;
};

constexpr inline auto GAME_RESOLVE_FUNCTION_NAME = "GameResolve";
using PFN_GameResolve = GameResolve* (*)();