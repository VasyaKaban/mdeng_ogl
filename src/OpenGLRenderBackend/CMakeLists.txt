add_library(OpenGLRenderBackend SHARED)

file(GLOB_RECURSE SOURCES
		**.cpp
		**.h
		**.hpp)

target_sources(
OpenGLRenderBackend
    PRIVATE
	    ${SOURCES}
)

target_include_directories(OpenGLRenderBackend PUBLIC "../")

find_package(OpenGL REQUIRED)

set(GLAD_LOADER_NON_REQUIRED_EXTENSIONS "GL_EXT_texture_compression_s3tc" "GL_EXT_texture_sRGB")
#set(GLAD_LOADER_REQUIRED_EXTENSIONS "")

glad_add_library(glad_loader STATIC EXCLUDE_FROM_ALL REPRODUCIBLE LANGUAGE "c" MX API gl:core=4.6 EXTENSIONS ${GLAD_LOADER_NON_REQUIRED_EXTENSIONS} ${GLAD_LOADER_REQUIRED_EXTENSIONS})

string(REPLACE ";" "\",\"" OPENGL_NON_REQUIRED_EXTENSIONS_SET "${GLAD_LOADER_NON_REQUIRED_EXTENSIONS}")
#string(REPLACE ";" "\",\"" OPENGL_REQUIRED_EXTENSIONS_SET "${GLAD_LOADER_REQUIRED_EXTENSIONS}")

#target_compile_definitions(OpenGLRenderBackend PRIVATE OPENGL_REQUIRED_EXTENSIONS=\"${OPENGL_REQUIRED_EXTENSIONS_SET}\")

target_link_libraries(OpenGLRenderBackend
						PUBLIC Core
						PRIVATE glad_loader
						PRIVATE OpenGL::GL
					)