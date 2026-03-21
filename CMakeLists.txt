cmake_minimum_required(VERSION 3.16)

project(SkyDream)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX) # Warnings as errors
    add_compile_options(/ZI) # Hot Reload support
    add_compile_options(/utf-8) # utf8 support compile side
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(code/Extern)
add_subdirectory(code/Modules)
add_subdirectory(code/Executables)
