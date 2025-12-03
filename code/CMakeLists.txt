cmake_minimum_required(VERSION 3.16)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# warning level 4 and all warnings as errors
add_compile_options(/W4 /WX)
# support Hot Reload
add_compile_options(/ZI)
# utf8 support compile side (necessary for fmt lib to support utf8)
add_compile_options(/utf-8)

add_subdirectory(Extern)
add_subdirectory(Frameworks)
add_subdirectory(Modules)
add_subdirectory(Executables)
