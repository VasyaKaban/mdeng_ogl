cmake_minimum_required(VERSION 3.28)

project(mdeng LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXECUTABLE_ENABLE_EXPORTS ON)

set(OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/out")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/bin")

add_subdirectory(deps/SDL EXCLUDE_FROM_ALL)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(deps/json EXCLUDE_FROM_ALL)

add_subdirectory(deps/glad/cmake EXCLUDE_FROM_ALL)

add_compile_definitions(ROOT_SUBDIR=\"../../\")
add_compile_definitions(BIN_SUBDIR=\"../\")
add_compile_definitions(GAME_SUBDIR=\"../../game\")

#add_subdirectory(deps/glad)
add_subdirectory("src/hrs")
add_subdirectory("src/Core")
add_subdirectory("src/Engine")
add_subdirectory("src/Game")
add_subdirectory("src/OpenGLRenderBackend")
add_subdirectory("src/Dummy" EXCLUDE_FROM_ALL)

find_package(Python COMPONENTS Interpreter REQUIRED)

add_custom_target(
    out ALL
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/game/shaders/ogl/sc.py ${PROJECT_SOURCE_DIR}/game/shaders/ogl/src ${PROJECT_SOURCE_DIR}/game/shaders/ogl/compiled
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/game ${OUTPUT_DIRECTORY}/game
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/config.json ${OUTPUT_DIRECTORY})

#deps/glad as static
#Core + hrs as static
#Engine -> exe
#Game -> dll(plugin)
#OpenGLRenderBackend -> dll


#[[
FS:
	bin:
		Core.dll
		Engine.exe
        Game.dll
		Render
			OpenGLRender.dll
			VulkanRender.dll -> TODO
	game:
		images
		shaders:
			ogl
				src
				compiled
		objects

]]