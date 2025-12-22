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

set(GLAD_OPENGL_LOADER_REQUIRED_EXTENSIONS 
	"GL_ARB_gpu_shader_int64" 
	"GL_ARB_polygon_offset_clamp" 
	"GL_ARB_shader_draw_parameters" 
	"GL_ARB_texture_filter_anisotropic" 
	"GL_EXT_texture_compression_s3tc" 
	"GL_EXT_texture_sRGB" 
	"GL_EXT_texture_sRGB_R8" 
	"GL_EXT_texture_sRGB_RG8"
	"GL_EXT_shader_image_load_formatted"
	"GL_AMD_gpu_shader_int16"
	"GL_EXT_depth_bounds_test"
	)

set(GLAD_WGL_LOADER_REQUIRED_EXTENSIONS
	"WGL_ARB_create_context"
	"WGL_ARB_create_context_no_error"#enable when WGL_CONTEXT_DEBUG_BIT_ARB(validation_layer) is not set
	"WGL_ARB_create_context_profile"
	"WGL_ARB_create_context_robustness"#enable when robustBufferAccess is enabled(+ add none notification)
	"WGL_ARB_extensions_string"
	"WGL_ARB_framebuffer_sRGB"#WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB in format choose
	"WGL_ARB_pixel_format"
	"WGL_ARB_pixel_format_float"#WGL_TYPE_RGBA_FLOAT_ARB in format choose
	"WGL_EXT_swap_control"#0 - for immediate, 1 - for FIFO
	"WGL_EXT_swap_control_tear"#-1 - for relaxed FIFO 
	"WGL_ARB_context_flush_control"
	)

#create dummy window and dummy HDC + resolve main WGL functions + create dummy context + make current dummy context
#destroy all prev dummy resources
#create another dummy windows and dummy HDC + create dummy context with OGL core 4.5 + get all available pixel formats and context properties
#destroy all prev dummy resources
#get properties and create context on current window

glad_add_library(glad_loader STATIC EXCLUDE_FROM_ALL REPRODUCIBLE LANGUAGE "c" MX API gl:core=4.6 wgl=1.0 EXTENSIONS ${GLAD_OPENGL_LOADER_REQUIRED_EXTENSIONS} ${GLAD_WGL_LOADER_REQUIRED_EXTENSIONS})

target_link_libraries(OpenGLRenderBackend
						PUBLIC Core
						PRIVATE glad_loader
						PRIVATE OpenGL::GL
					)

#Resolve -> WGL/GLX/EGL dummy context
#Instance -> Physical device and debug messenger holder
#PhysicalDevice -> Core OpenGL 4.5 with WGL/GLX/EGL native objects(WGL: Window, DC, GLRC) holder
#Surface -> Real window holder that has connected and disconnected states(connected: associated with device, disconnected: no device); cannot be disconnected after connection with device due to the inner pixel format strong association with window 
#Device -> Core OpenGL 4.5 holder with enabled features