add_library(core SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
core
    PRIVATE
	    ${SOURCES}
)

find_package(SDL2 REQUIRED)

target_include_directories(core 
							PUBLIC "../")

target_link_libraries(core
	                    PUBLIC hrs
						PUBLIC SDL2::SDL2main
						PUBLIC SDL2::SDL2
					)