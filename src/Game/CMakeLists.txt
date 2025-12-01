add_library(Game SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
Game
    PRIVATE
	    ${SOURCES}
)

target_include_directories(Game PUBLIC "../")

target_link_libraries(Game
						PRIVATE Core
						PRIVATE Engine
					)