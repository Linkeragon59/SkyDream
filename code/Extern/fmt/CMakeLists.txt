cmake_minimum_required(VERSION 3.16)

add_library(fmt STATIC IMPORTED GLOBAL)

set(fmtPath ${CMAKE_CURRENT_SOURCE_DIR}/11.2.0)

set_target_properties(fmt PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES
		${fmtPath}/include
	IMPORTED_LOCATION_DEBUG
		${fmtPath}/lib/win64/fmtd.lib
	IMPORTED_LOCATION
		${fmtPath}/lib/win64/fmt.lib
)
