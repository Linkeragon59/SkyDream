cmake_minimum_required(VERSION 4.0)

add_library(GoogleTest STATIC IMPORTED GLOBAL)

set(GoogleTestPath ${CMAKE_CURRENT_SOURCE_DIR}/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.7)
set(GoogleTestLibsPath ${GoogleTestPath}/lib/native/v140/windesktop/msvcstl/static/rt-dyn/x64)

set_target_properties(GoogleTest PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES
		${GoogleTestPath}/build/native/include
	IMPORTED_LOCATION_DEBUG
		${GoogleTestLibsPath}/Debug/gtestd.lib
	IMPORTED_LOCATION
		${GoogleTestLibsPath}/Release/gtest.lib
)
