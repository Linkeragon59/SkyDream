cmake_minimum_required(VERSION 3.16)

add_library(fmtInclude INTERFACE)
add_library(fmtLib INTERFACE)

target_include_directories(fmtInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/11.2.0/include)

target_link_directories(fmtLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/11.2.0/lib/win64)
target_link_libraries(fmtLib INTERFACE debug fmtd)
target_link_libraries(fmtLib INTERFACE optimized fmt)
