cmake_minimum_required(VERSION 4.0)

add_library(spdlog STATIC IMPORTED GLOBAL)

set(spdlogPath ${CMAKE_CURRENT_SOURCE_DIR}/1.15.2)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_target_properties(spdlog PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${spdlogPath}/include
		IMPORTED_LOCATION_DEBUG
			${spdlogPath}/lib/win64/spdlogd.lib
		IMPORTED_LOCATION
			${spdlogPath}/lib/win64/spdlog.lib
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set_target_properties(spdlog PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${spdlogPath}/include
		IMPORTED_LOCATION
			${spdlogPath}/lib/linux/libspdlog.a
	)
endif ()

