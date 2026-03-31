cmake_minimum_required(VERSION 4.0)

add_executable(UnitTestsSample)

target_sources(UnitTestsSample
	PRIVATE
		Precompile.h
		main.cpp
)
target_precompile_headers(UnitTestsSample PRIVATE Precompile.h)
target_include_directories(UnitTestsSample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UnitTestsSample PRIVATE GoogleTest)
target_link_libraries(UnitTestsSample PRIVATE fmt)
target_link_libraries(UnitTestsSample PRIVATE spdlog)

set_property(TARGET UnitTestsSample PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
