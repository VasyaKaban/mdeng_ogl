add_library(OpenGLRenderBackend SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
OpenGLRenderBackend
    PRIVATE
	    ${SOURCES}
)

target_include_directories(OpenGLRenderBackend PUBLIC "../")

find_package(OpenGL REQUIRED)

target_link_libraries(OpenGLRenderBackend
						PUBLIC Core
						PUBLIC glad
						PUBLIC OpenGL::GL
					)