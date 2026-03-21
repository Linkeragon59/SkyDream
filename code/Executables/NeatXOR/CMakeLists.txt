cmake_minimum_required(VERSION 4.0)

add_executable(NeatXOR)

target_sources(NeatXOR
	PRIVATE
		Precompile.h
		main.cpp
)

target_precompile_headers(NeatXOR PRIVATE Precompile.h)

target_include_directories(NeatXOR PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NeatXOR PRIVATE Core)
target_link_libraries(NeatXOR PRIVATE NEAT)

set_property(TARGET NeatXOR PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
