add_executable(Engine)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
Engine
    PRIVATE
	    ${SOURCES}
)

target_include_directories(Engine PUBLIC "../")

target_link_libraries(Engine 
						PRIVATE Core
					)

