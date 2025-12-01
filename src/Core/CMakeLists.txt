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

target_include_directories(Core 
							PUBLIC "../")

target_link_libraries(Core
	                    PUBLIC hrs
						PUBLIC SDL2::SDL2main
						PUBLIC SDL2::SDL2
						PUBLIC nlohmann_json::nlohmann_json
					)