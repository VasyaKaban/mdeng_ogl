add_library(hrs SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
hrs
    PRIVATE
	    ${SOURCES}
)

set_target_properties(hrs PROPERTIES LINKER_LANGUAGE CXX)