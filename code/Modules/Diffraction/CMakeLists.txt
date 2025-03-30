cmake_minimum_required(VERSION 3.16)

add_library(Diffraction)
target_sources(Diffraction
	PRIVATE
		public/Diffraction_Data.h
		public/Diffraction_Module.h

		private/Diffraction_Precompile.h
		private/Diffraction_LightSpectrum.h
		private/Diffraction_LightSpectrum.cpp
		private/Diffraction_Module.cpp
		private/Diffraction_Renderer.h
		private/Diffraction_Renderer.cpp
)

target_precompile_headers(Diffraction PRIVATE private/Diffraction_Precompile.h)
target_compile_features(Diffraction PRIVATE cxx_std_23)

target_include_directories(Diffraction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_include_directories(Diffraction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
target_include_directories(Diffraction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Render/private)

target_link_libraries(Diffraction PRIVATE Core)
target_link_libraries(Diffraction PRIVATE Render)
target_link_libraries(Diffraction PRIVATE VulkanMemoryAllocator)
target_link_libraries(Diffraction PRIVATE VulkanInclude)
target_link_libraries(Diffraction PRIVATE GLFW)
target_link_libraries(Diffraction PRIVATE tinyglTF)
