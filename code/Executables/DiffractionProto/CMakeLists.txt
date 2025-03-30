cmake_minimum_required(VERSION 3.16)

add_executable(DiffractionProto)

target_sources(DiffractionProto
	PRIVATE
		Precompile.h
		DiffractionProtoModule.h
		DiffractionProtoModule.cpp
		TangiblesManager.h
		TangiblesManager.cpp
		Screen.h
		Screen.cpp
		MainScreen.h
		MainScreen.cpp
		SecondaryScreen.h
		SecondaryScreen.cpp
		ScreenManager.h
		ScreenManager.cpp
		StateManager.h
		StateManager.cpp
		main.cpp
)

target_precompile_headers(DiffractionProto PRIVATE Precompile.h)
target_compile_features(DiffractionProto PRIVATE cxx_std_23)

target_include_directories(DiffractionProto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(DiffractionProto PRIVATE Core)
target_link_libraries(DiffractionProto PRIVATE Render)
target_link_libraries(DiffractionProto PRIVATE Diffraction)
target_link_libraries(DiffractionProto PRIVATE Debugger)
target_link_libraries(DiffractionProto PRIVATE ImGui)
target_link_libraries(DiffractionProto PRIVATE RapidJSON)
target_link_libraries(DiffractionProto PRIVATE Localization)

set_property(TARGET DiffractionProto PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
