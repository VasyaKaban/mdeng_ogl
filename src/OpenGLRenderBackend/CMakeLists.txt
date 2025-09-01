add_library(opengl_render_backend SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
opengl_render_backend
    PRIVATE
	    ${SOURCES}
)

target_include_directories(opengl_render_backend PUBLIC "../")

find_package(OpenGL REQUIRED)

target_link_libraries(opengl_render_backend
	                    PUBLIC hrs
						PUBLIC core
						PUBLIC glad
						PUBLIC OpenGL::GL
					)