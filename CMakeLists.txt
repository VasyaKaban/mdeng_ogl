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
add_subdirectory("src/Game")
add_subdirectory("src/OpenGLRenderBackend")

add_custom_target(
    out ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/game ${OUTPUT_DIRECTORY}/game)


#set(OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
#add_custom_target(
#    out ALL
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIRECTORY}
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:core> ${OUTPUT_DIRECTORY}/
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:game> ${OUTPUT_DIRECTORY}/
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2> ${OUTPUT_DIRECTORY}/)


#add_subdirectory(deps/glad)

#add_executable(${PROJECT_NAME})

#file(GLOB_RECURSE SOURCES
#		src/**.cpp
#		src/**.h
#		src/**.hpp)

#target_sources(
#${PROJECT_NAME}
#    PRIVATE
#	    ${SOURCES}
#)

#target_include_directories(${PROJECT_NAME} PRIVATE deps/glad/include)

#find_package(SDL2 REQUIRED)
#find_package(SDL2_ttf REQUIRED)
#find_package(SDL2_image REQUIRED)
#find_package(OpenGL REQUIRED)

#target_link_libraries(${PROJECT_NAME}
#	                    PUBLIC glad
#						PUBLIC SDL2::SDL2main
#						PUBLIC SDL2::SDL2
#						PUBLIC SDL2_ttf::SDL2_ttf
#						PUBLIC SDL2_image::SDL2_image
#						PUBLIC OpenGL::GL
#					)