cmake_minimum_required(VERSION 4.0)

add_executable(NeatPoleBalancing2)

target_sources(NeatPoleBalancing2
	PRIVATE
		Precompile.h
		main.cpp
)

target_precompile_headers(NeatPoleBalancing2 PRIVATE Precompile.h)

target_include_directories(NeatPoleBalancing2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NeatPoleBalancing2 PRIVATE Core)
target_link_libraries(NeatPoleBalancing2 PRIVATE Render)
target_link_libraries(NeatPoleBalancing2 PRIVATE ImGui)
target_link_libraries(NeatPoleBalancing2 PRIVATE NEAT)

set_property(TARGET NeatPoleBalancing2 PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
