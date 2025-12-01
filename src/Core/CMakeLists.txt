add_library(Core SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
Core
    PRIVATE
	    ${SOURCES}
)

find_package(SDL2 REQUIRED)

target_include_directories(Core 
							PUBLIC "../")

target_link_libraries(Core
	                    PUBLIC hrs
						PUBLIC SDL2::SDL2main
						PUBLIC SDL2::SDL2
					)