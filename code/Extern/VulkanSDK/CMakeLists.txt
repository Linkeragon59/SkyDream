cmake_minimum_required(VERSION 3.16)

set(VulkanPath ${CMAKE_CURRENT_SOURCE_DIR}/1.3.275.0)

#------------------------------------------------------------------------------------------------

add_library(GLM INTERFACE)
target_include_directories(GLM INTERFACE ${VulkanPath}/include/glm)

#------------------------------------------------------------------------------------------------

add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE ${VulkanPath}/include/vma)

#------------------------------------------------------------------------------------------------

add_library(Vulkan STATIC IMPORTED GLOBAL)

set_target_properties(Vulkan PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES
		${VulkanPath}/include
	IMPORTED_LOCATION
		${VulkanPath}/lib/win64/vulkan-1.lib
)
