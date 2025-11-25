cmake_minimum_required(VERSION 3.28)

project(mdeng LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/out/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/bin/")

add_subdirectory(deps/glad)
add_subdirectory("src/hrs")
add_subdirectory("src/Core")
add_subdirectory("src/Engine")
#add_subdirectory("src/Game")
add_subdirectory("src/OpenGLRenderBackend")
add_subdirectory("src/Dummy")

find_package(Python COMPONENTS Interpreter REQUIIRED)

add_custom_target(
    out ALL
    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/game/shaders/ogl/sc.py ${PROJECT_SOURCE_DIR}/game/shaders/ogl/src ${PROJECT_SOURCE_DIR}/game/shaders/ogl/compiled
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/game ${OUTPUT_DIRECTORY}/game
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/config.json ${OUTPUT_DIRECTORY}/)

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