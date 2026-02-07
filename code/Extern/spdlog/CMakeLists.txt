cmake_minimum_required(VERSION 3.16)

add_library(spdlog STATIC IMPORTED GLOBAL)

set(spdlogPath ${CMAKE_CURRENT_SOURCE_DIR}/1.15.2)

set_target_properties(spdlog PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES
		${spdlogPath}/include
	IMPORTED_LOCATION_DEBUG
		${spdlogPath}/lib/win64/spdlogd.lib
	IMPORTED_LOCATION
		${spdlogPath}/lib/win64/spdlog.lib
)
