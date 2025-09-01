cmake_minimum_required(VERSION 3.28)

add_library(glad STATIC)

target_sources(
glad
    PRIVATE
	    src/gl.c
        include/glad/gl.h
        include/KHR/khrplatform.h
)

target_include_directories(glad PUBLIC include/)

set_source_files_properties(src/gl.c PROPERTIES LANGUAGE CXX)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE CXX)