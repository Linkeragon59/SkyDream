cmake_minimum_required(VERSION 3.16)

add_executable(Asteorhythm)

target_sources(Asteorhythm
	PRIVATE
		Precompile.h
		AsteorhythmModule.h
		AsteorhythmModule.cpp
		main.cpp
)

target_precompile_headers(Asteorhythm PRIVATE Precompile.h)
target_compile_features(Asteorhythm PRIVATE cxx_std_23)

target_include_directories(Asteorhythm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Asteorhythm PRIVATE Core)
target_link_libraries(Asteorhythm PRIVATE Editor)
target_link_libraries(Asteorhythm PRIVATE Render)

set_property(TARGET Asteorhythm PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
