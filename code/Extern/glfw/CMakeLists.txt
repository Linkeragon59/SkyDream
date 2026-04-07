cmake_minimum_required(VERSION 4.0)

add_library(GLFW)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_sources(GLFW
		PRIVATE
			3.3.9/include/GLFW/glfw3.h
			3.3.9/include/GLFW/glfw3native.h
			3.3.9/src/internal.h
			3.3.9/src/mappings.h
			3.3.9/src/win32_platform.h
			3.3.9/src/win32_joystick.h
			3.3.9/src/wgl_context.h
			3.3.9/src/egl_context.h
			3.3.9/src/osmesa_context.h
			3.3.9/src/context.c
			3.3.9/src/init.c
			3.3.9/src/input.c
			3.3.9/src/monitor.c
			3.3.9/src/vulkan.c
			3.3.9/src/window.c
			3.3.9/src/win32_init.c
			3.3.9/src/win32_joystick.c
			3.3.9/src/win32_monitor.c
			3.3.9/src/win32_time.c
			3.3.9/src/win32_thread.c
			3.3.9/src/win32_window.c
			3.3.9/src/wgl_context.c
			3.3.9/src/egl_context.c
			3.3.9/src/osmesa_context.c
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_sources(GLFW
		PRIVATE
			3.3.9/include/GLFW/glfw3.h
			3.3.9/include/GLFW/glfw3native.h
			3.3.9/src/internal.h
			3.3.9/src/mappings.h
			3.3.9/src/x11_platform.h
			3.3.9/src/linux_joystick.h
			3.3.9/src/posix_time.h
			3.3.9/src/posix_thread.h
			3.3.9/src/egl_context.h
			3.3.9/src/osmesa_context.h
			3.3.9/src/context.c
			3.3.9/src/init.c
			3.3.9/src/input.c
			3.3.9/src/monitor.c
			3.3.9/src/vulkan.c
			3.3.9/src/window.c
			3.3.9/src/x11_init.c
			3.3.9/src/x11_monitor.c
			3.3.9/src/linux_joystick.c
			3.3.9/src/posix_time.c
			3.3.9/src/posix_thread.c
			3.3.9/src/x11_window.c
			3.3.9/src/egl_context.c
			3.3.9/src/glx_context.c
			3.3.9/src/xkb_unicode.c
			3.3.9/src/osmesa_context.c
	)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_compile_definitions(GLFW PRIVATE _GLFW_WIN32)
	target_compile_options(GLFW PRIVATE -wd4100 -wd4152 -wd4201 -wd4244 -wd4706 -wd4996)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_compile_definitions(GLFW PRIVATE _GLFW_X11)
	target_compile_options(GLFW PRIVATE -Wno-pedantic -Wno-unused-parameter -Wno-missing-field-initializers -Wno-sign-compare)
endif()

target_include_directories(GLFW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3.3.9/include)
target_compile_definitions(GLFW PUBLIC GLFW_SKYDREAM)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(GLFW INTERFACE dl X11 Xi)
endif()
