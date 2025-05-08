cmake_minimum_required(VERSION 3.16)

# Interface Libraries

add_subdirectory(fmt)
add_subdirectory(onnxruntime)
add_subdirectory(rapidjson)
add_subdirectory(spdlog)
add_subdirectory(tinygltf)
add_subdirectory(VulkanSDK)

# Dependency Libraries

add_subdirectory(glfw)
set_target_properties(GLFW PROPERTIES FOLDER "Extern")
add_subdirectory(imgui)
set_target_properties(ImGui PROPERTIES FOLDER "Extern")
add_subdirectory(neat)
set_target_properties(NEAT PROPERTIES FOLDER "Extern")
add_subdirectory(piper)
set_target_properties(Piper PROPERTIES FOLDER "Extern")
add_subdirectory(SoLoud)
set_target_properties(SoLoud PROPERTIES FOLDER "Extern")
