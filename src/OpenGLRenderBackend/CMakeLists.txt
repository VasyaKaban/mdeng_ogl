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

set(GLAD_LOADER_NON_REQUIRED_EXTENSIONS 
	"GL_ARB_gpu_shader_int64" 
	"GL_ARB_polygon_offset_clamp" 
	"GL_ARB_shader_draw_parameters" 
	"GL_ARB_texture_filter_anisotropic" 
	"GL_EXT_texture_compression_s3tc" 
	"GL_EXT_texture_sRGB" 
	"GL_EXT_texture_sRGB_R8" 
	"GL_EXT_texture_sRGB_RG8"
	"GL_KHR_no_error"
	"GL_NV_vertex_attrib_integer_64bit")

glad_add_library(glad_loader STATIC EXCLUDE_FROM_ALL REPRODUCIBLE LANGUAGE "c" MX API gl:core=4.6 EXTENSIONS ${GLAD_LOADER_NON_REQUIRED_EXTENSIONS})

string(REPLACE ";" "\",\"" OPENGL_NON_REQUIRED_EXTENSIONS_SET "${GLAD_LOADER_NON_REQUIRED_EXTENSIONS}")

target_link_libraries(OpenGLRenderBackend
						PUBLIC Core
						PRIVATE glad_loader
						PRIVATE OpenGL::GL
					)