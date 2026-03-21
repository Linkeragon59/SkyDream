cmake_minimum_required(VERSION 4.0)

add_subdirectory(Core)
set_target_properties(Core PROPERTIES FOLDER "Modules")

add_subdirectory(Render)
set_target_properties(Render PROPERTIES FOLDER "Modules")

add_subdirectory(Debugger)
set_target_properties(Debugger PROPERTIES FOLDER "Modules")

add_subdirectory(Editor)
set_target_properties(Editor PROPERTIES FOLDER "Modules")

add_subdirectory(Localization)
set_target_properties(Localization PROPERTIES FOLDER "Modules")
