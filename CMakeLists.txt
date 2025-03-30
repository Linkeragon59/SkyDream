cmake_minimum_required(VERSION 3.16)

project(SkyDream)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# warning level 4 and all warnings as errors
add_compile_options(/W4 /WX)
# support Hot Reload
add_compile_options(/ZI)

add_subdirectory(code/Extern)
add_subdirectory(code/Frameworks)
add_subdirectory(code/Modules)
add_subdirectory(code/Executables)
