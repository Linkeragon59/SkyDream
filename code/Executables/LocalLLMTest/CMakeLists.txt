cmake_minimum_required(VERSION 3.16)

add_executable(LocalLLMTest)

target_sources(LocalLLMTest
	PRIVATE
		Precompile.h
		common.h
		common.cpp
		phi3.cpp
)

target_precompile_headers(LocalLLMTest PRIVATE Precompile.h)
target_compile_features(LocalLLMTest PRIVATE cxx_std_23)

target_include_directories(LocalLLMTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(LocalLLMTest PRIVATE Core)
target_link_libraries(LocalLLMTest PRIVATE OnnxRuntimeInclude)
target_link_libraries(LocalLLMTest PRIVATE OnnxRuntimeLib)
target_link_libraries(LocalLLMTest PRIVATE OnnxRuntimeGenAIInclude)
target_link_libraries(LocalLLMTest PRIVATE OnnxRuntimeGenAILib)

file(GLOB OnnxRuntimeDlls "${OnnxRuntimeLibsPath}/*.dll")
foreach(OnnxRuntimeDll ${OnnxRuntimeDlls})
	add_custom_command(
		TARGET LocalLLMTest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OnnxRuntimeDll} $<TARGET_FILE_DIR:LocalLLMTest>
	)
endforeach()

file(GLOB OnnxRuntimeGenAIDlls "${OnnxRuntimeGenAILibsPath}/*.dll")
foreach(OnnxRuntimeGenAIDll ${OnnxRuntimeGenAIDlls})
	add_custom_command(
		TARGET LocalLLMTest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OnnxRuntimeGenAIDll} $<TARGET_FILE_DIR:LocalLLMTest>
	)
endforeach()

set_property(TARGET LocalLLMTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
