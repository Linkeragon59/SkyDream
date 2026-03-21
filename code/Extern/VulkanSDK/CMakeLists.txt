cmake_minimum_required(VERSION 4.0)

set(VulkanPath ${CMAKE_CURRENT_SOURCE_DIR}/1.4.341.1)

#------------------------------------------------------------------------------------------------

add_library(GLM INTERFACE)
target_include_directories(GLM INTERFACE ${VulkanPath}/include/glm)

#------------------------------------------------------------------------------------------------

add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE ${VulkanPath}/include/vma)

#------------------------------------------------------------------------------------------------

add_library(Vulkan STATIC IMPORTED GLOBAL)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_target_properties(Vulkan PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${VulkanPath}/include
		IMPORTED_LOCATION
			${VulkanPath}/lib/win64/vulkan-1.lib
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set_target_properties(Vulkan PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			${VulkanPath}/include
		IMPORTED_LOCATION
			${VulkanPath}/lib/linux/libvulkan.so
	)
endif ()
