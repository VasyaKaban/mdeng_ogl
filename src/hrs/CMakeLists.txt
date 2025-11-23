add_library(hrs STATIC)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

if(WIN32)
list(FILTER SOURCES EXCLUDE REGEX "detail/linux/*")
elseif(LINUX)
list(FILTER SOURCES EXCLUDE REGEX "detail/winapi/*")
else()
message(FATAL_ERROR "Not implemented yet...")
endif()

target_sources(
hrs
    PRIVATE
	    ${SOURCES}
)

set_target_properties(hrs PROPERTIES LINKER_LANGUAGE CXX)