cmake_minimum_required(VERSION 3.28)

project(mdeng LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps/glad)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE SOURCES
		src/**.cpp
		src/**.h
		src/**.hpp)

target_sources(
${PROJECT_NAME}
    PRIVATE
	    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE deps/glad/include)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME}
	                    PUBLIC glad
						PUBLIC SDL2::SDL2main
						PUBLIC SDL2::SDL2
						PUBLIC SDL2_ttf::SDL2_ttf
						PUBLIC SDL2_image::SDL2_image
						PUBLIC OpenGL::GL
					)