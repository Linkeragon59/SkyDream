cmake_minimum_required(VERSION 3.16)

set(ImGuiVersion "1.92.5")
set(ImPlotVersion "implot-0.17")

add_library(ImGui)
target_sources(ImGui
	PRIVATE
		${ImGuiVersion}/imconfig.h
		${ImGuiVersion}/imgui.h
		${ImGuiVersion}/imgui_helpers.h
		${ImGuiVersion}/imgui_internal.h
		${ImGuiVersion}/imstb_rectpack.h
		${ImGuiVersion}/imstb_textedit.h
		${ImGuiVersion}/imstb_truetype.h
		${ImGuiVersion}/imgui.cpp
		${ImGuiVersion}/imgui_demo.cpp
		${ImGuiVersion}/imgui_draw.cpp
		${ImGuiVersion}/imgui_helpers.cpp
		${ImGuiVersion}/imgui_tables.cpp
		${ImGuiVersion}/imgui_widgets.cpp
		${ImPlotVersion}/implot.h
		${ImPlotVersion}/implot_internal.h
		${ImPlotVersion}/implot.cpp
		${ImPlotVersion}/implot_demo.cpp
		${ImPlotVersion}/implot_items.cpp
)

target_compile_features(ImGui PRIVATE cxx_std_23)

target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${ImGuiVersion})
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${ImPlotVersion})
target_compile_definitions(ImGui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_compile_definitions(ImGui PUBLIC IMGUI_SKYDREAM)
