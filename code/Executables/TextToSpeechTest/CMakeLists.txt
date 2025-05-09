cmake_minimum_required(VERSION 3.16)

add_executable(TextToSpeechTest)

target_sources(TextToSpeechTest
	PRIVATE
		Precompile.h
		main.cpp
)

target_precompile_headers(TextToSpeechTest PRIVATE Precompile.h)
target_compile_features(TextToSpeechTest PRIVATE cxx_std_23)

target_include_directories(TextToSpeechTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TextToSpeechTest PRIVATE Core)
target_link_libraries(TextToSpeechTest PRIVATE Piper)
target_link_libraries(TextToSpeechTest PRIVATE OnnxRuntimeLib)

set_property(TARGET TextToSpeechTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
