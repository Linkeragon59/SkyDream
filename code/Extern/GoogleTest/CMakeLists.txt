cmake_minimum_required(VERSION 4.0)

add_library(GoogleTest STATIC IMPORTED GLOBAL)

set(GoogleTestPath ${CMAKE_CURRENT_SOURCE_DIR}/1.17.0)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_target_properties(GoogleTest PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${GoogleTestPath}/include
		IMPORTED_LOCATION_DEBUG
			${GoogleTestPath}/lib/win64/gtestd.lib
		IMPORTED_LOCATION
			${GoogleTestPath}/lib/win64/gtest.lib
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set_target_properties(GoogleTest PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${GoogleTestPath}/include
		IMPORTED_LOCATION
			${GoogleTestPath}/lib/linux/libgtest.a
	)
endif ()

