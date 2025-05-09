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

file(GLOB OnnxRuntimeDlls "${OnnxRuntimeLibsPath}/*.dll")
foreach(OnnxRuntimeDll ${OnnxRuntimeDlls})
	add_custom_command(
		TARGET TextToSpeechTest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OnnxRuntimeDll} $<TARGET_FILE_DIR:TextToSpeechTest>
	)
endforeach()

add_custom_command(
	TARGET TextToSpeechTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${eSpeakNGDebugDLLPath}>$<$<NOT:$<CONFIG:Debug>>:${eSpeakNGReleaseDLLPath}> $<TARGET_FILE_DIR:TextToSpeechTest>
)

add_custom_command(
	TARGET TextToSpeechTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${eSpeakNGDataPath} $<TARGET_FILE_DIR:TextToSpeechTest>/espeak-ng-data
)

set_property(TARGET TextToSpeechTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
