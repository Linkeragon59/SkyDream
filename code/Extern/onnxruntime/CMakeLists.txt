cmake_minimum_required(VERSION 3.16)

#---------------------------------------------------------------------------------------------------------------------------

set(OnnxRuntimeFolder "onnxruntime-win-x64-1.21.0")
add_library(OnnxRuntimeInclude INTERFACE)
add_library(OnnxRuntimeLib INTERFACE)

target_include_directories(OnnxRuntimeInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${OnnxRuntimeFolder}/include)

set(OnnxRuntimeLibsPath ${CMAKE_CURRENT_SOURCE_DIR}/${OnnxRuntimeFolder}/lib CACHE INTERNAL "")
target_link_directories(OnnxRuntimeLib INTERFACE ${OnnxRuntimeLibsPath})
target_link_libraries(OnnxRuntimeLib INTERFACE onnxruntime)

#---------------------------------------------------------------------------------------------------------------------------

set(OnnxRuntimeGenAIFolder "onnxruntime-genai-0.7.1-win-x64")
add_library(OnnxRuntimeGenAIInclude INTERFACE)
add_library(OnnxRuntimeGenAILib INTERFACE)

target_include_directories(OnnxRuntimeGenAIInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${OnnxRuntimeGenAIFolder}/include)

set(OnnxRuntimeGenAILibsPath ${CMAKE_CURRENT_SOURCE_DIR}/${OnnxRuntimeGenAIFolder}/lib CACHE INTERNAL "")
target_link_directories(OnnxRuntimeGenAILib INTERFACE ${OnnxRuntimeGenAILibsPath})
target_link_libraries(OnnxRuntimeGenAILib INTERFACE onnxruntime-genai)
