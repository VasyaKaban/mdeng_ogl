add_library(Core SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

if(WIN32)
list(FILTER SOURCES EXCLUDE REGEX "Utils/Linux/*")
elseif(LINUX)
list(FILTER SOURCES EXCLUDE REGEX "Utils/WinAPI/*")
else()
message(FATAL_ERROR "Not implemented yet...")
endif()

target_sources(
Core
    PRIVATE
	    ${SOURCES}
)

target_include_directories(Core 
							PUBLIC "../")

target_compile_definitions(Core PRIVATE CORE_API_EXPORT)

target_link_libraries(Core
						PUBLIC SDL2::SDL2main
						PUBLIC SDL2::SDL2
						PUBLIC nlohmann_json::nlohmann_json
					)