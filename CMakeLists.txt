cmake_minimum_required(VERSION 2.6)

project(Unlike3D)

if(${MSVC})
	set(MODULE_SUFFIX .dll)
endif()

set(VULKAN_INCLUDE_DIRECTORY C:/VulkanSDK/1.1.97.0/Include)
set(VULKAN_LIBRARY_DIRECTORY C:/VulkanSDK/1.1.97.0/Lib32)
set(VULKAN_LIBRARY_NAME vulkan-1)

add_compile_definitions(UNLIKE3D_MODULE_SUFFIX="${MODULE_SUFFIX}")

add_subdirectory(3rd)
add_subdirectory(src)