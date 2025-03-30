cmake_minimum_required(VERSION 3.16)

add_library(Localization)
target_sources(Localization
	PRIVATE
		public/Localization_Module.h

		private/Localization_Precompile.h
		private/Localization_Module.cpp
)

target_precompile_headers(Localization PRIVATE private/Localization_Precompile.h)
target_compile_features(Localization PRIVATE cxx_std_23)

target_include_directories(Localization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_include_directories(Localization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(Localization PRIVATE Core)
target_link_libraries(Localization PRIVATE ImGui)
