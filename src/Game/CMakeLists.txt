add_executable(game)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
game
    PRIVATE
	    ${SOURCES}
)

target_include_directories(game PUBLIC "../")

target_link_libraries(game
	                    PRIVATE hrs
						PRIVATE core
						PRIVATE opengl_render_backend
					)