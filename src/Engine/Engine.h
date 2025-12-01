#pragma once

#include "hrs/os.hpp"
#include "Engine/Game/Game.h"
#include "Core/Window/GraphicWindow.h"
#include "RenderEngine/RenderEngine.h"
#include "ResourceManager/ResourceManager.h"
#include "Timer/Timer.h"

namespace Engine
{
    struct GameEngineInfo
    {
        Core::GraphicWindow* window;
        RenderEngine* render_engine;
        ResourceManager* resource_manager;
        std::uint32_t max_fps;
        float reference_update_factor;
    };

    class GameEngine : hrs::non_copyable, hrs::non_movable
    {
        GameEngine(const GameEngineInfo& info, GameState&& _game_state);
        ~GameEngine();
    public:
        static int Enter(int argc, char** argv);
        static GameEngine* GetInstance() noexcept;

        Core::GraphicWindow* GetWindow() const noexcept;
        RenderEngine* GetRenderEngine() const noexcept;
        ResourceManager* GetResourceManager() const noexcept;

        std::uint64_t GetFrameIndex() const noexcept;
        const Timer& GetTimer() const noexcept;

        std::uint32_t GetMaxFPS() const noexcept;
        float GetUpdateFactor() const noexcept;
    private:
        void loop();
    private:
        constexpr static auto DEFAULT_GAME_IMPLEMENTATION_NAME = "Game";
        constexpr static float DEFAULT_REFERENCE_UPDATE_FACTOR = 120;

        static inline GameEngine* instance = nullptr;

        GameState game_state;

        Core::GraphicWindow* window;
        std::unique_ptr<RenderEngine> render_engine;
        std::unique_ptr<ResourceManager> resource_manager;

        std::unique_ptr<Game> game;

        std::uint64_t frame_index;
        Timer frame_timer;

        std::uint32_t max_fps;

        float reference_update_factor;
        float update_factor;
    };
};