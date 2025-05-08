cmake_minimum_required(VERSION 3.16)

add_library(OnnxRuntimeInclude INTERFACE)
add_library(OnnxRuntimeLib INTERFACE)

target_include_directories(OnnxRuntimeInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/1.21.0_win64/include)

target_link_directories(OnnxRuntimeLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/1.21.0_win64/lib)
target_link_libraries(OnnxRuntimeLib INTERFACE onnxruntime)
