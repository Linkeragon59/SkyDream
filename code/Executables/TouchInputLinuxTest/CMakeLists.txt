cmake_minimum_required(VERSION 4.0)

add_executable(TouchInputLinuxTest)

target_sources(TouchInputLinuxTest
	PRIVATE
		Precompile.h
		main.cpp
)
target_precompile_headers(TouchInputLinuxTest PRIVATE Precompile.h)
target_include_directories(TouchInputLinuxTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TouchInputLinuxTest PRIVATE Core)
target_link_libraries(TouchInputLinuxTest PRIVATE GLFW)

set_property(TARGET TouchInputLinuxTest PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
