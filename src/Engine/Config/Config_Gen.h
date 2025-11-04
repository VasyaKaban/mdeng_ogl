#include "Core/JSON/MetaPrefix.h"
#include "Core/Window/RenderBackend.h"

JSON_BEGIN(WindowConfig)
    JSON_VALUE(title, std::string)
    JSON_VALUE(width, std::uint32_t)
    JSON_VALUE(height, std::uint32_t)
JSON_END(WindowConfig)

JSON_BEGIN(EngineConfig)
    JSON_VALUE(transfer_storage_size,
               std::uint64_t,
               [](std::uint64_t transfer_storage_size, [[maybe_unused]] const EngineConfig&)
               {
                   if(transfer_storage_size == 0)
                       throw std::runtime_error("transfer_storage_size must be grater than zero");
               })
    JSON_VALUE(reference_update_factor,
               float,
               [](float reference_update_factor, [[maybe_unused]] const EngineConfig&)
               {
                   if(reference_update_factor <= 0)
                       throw std::runtime_error("reference_update_factor must be grater than zero");
               })
    JSON_VALUE(max_fps,
               float,
               [](float max_fps, [[maybe_unused]] const EngineConfig&)
               {
                   if(max_fps <= 0)
                       throw std::runtime_error("max_fps must be grater than zero");
               })
    JSON_VALUE(window, WindowConfig)
JSON_END(EngineConfig)

JSON_BEGIN(RenderConfig)
    JSON_VALUE(backend, RenderBackendType)
    JSON_VALUE(enable_debug, bool)
    JSON_VALUE(
        implementation,
        std::string,
        [](const std::string& render_backend_implementation, [[maybe_unused]] const RenderConfig&)
        {
            if(render_backend_implementation.empty())
                throw std::runtime_error(
                    "render_backend_implementation must not be an empty string");
        })
    JSON_RAW(render_implementation_config)
JSON_END(RenderConfig)

JSON_BEGIN(GameConfig)
    JSON_VALUE(implementation,
               std::string,
               [](const std::string& implementation, [[maybe_unused]] const GameConfig&)
               {
                   if(implementation.empty())
                       throw std::runtime_error("implementation must not be an empty string");
               })
    JSON_RAW(implementation_config)
JSON_END(GameConfig)

JSON_BEGIN(Config)
    JSON_VALUE(engine_config, EngineConfig)
    JSON_VALUE(render_config, RenderConfig)
    JSON_VALUE(game_config, GameConfig)
JSON_END(Config)
