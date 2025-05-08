cmake_minimum_required(VERSION 3.16)

add_library(spdlogInclude INTERFACE)
add_library(spdlogLib INTERFACE)

target_include_directories(spdlogInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/1.15.2/include)

target_link_directories(spdlogLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/1.15.2/lib/win64)
target_link_libraries(spdlogLib INTERFACE debug spdlogd)
target_link_libraries(spdlogLib INTERFACE optimized spdlog)
