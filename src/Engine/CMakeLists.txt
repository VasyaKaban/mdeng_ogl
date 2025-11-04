add_executable(engine)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
engine
    PRIVATE
	    ${SOURCES}
)

target_compile_definitions(engine PRIVATE ROOT_SUBDIR=\"../../\")

target_include_directories(engine PUBLIC "../")

target_link_libraries(engine 
						PRIVATE core
					)