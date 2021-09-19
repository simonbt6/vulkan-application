cmake_minimum_required(VERSION 3.0.0)
project(TemplateVulcanApplication VERSION 1.0.0)

include(CTest)
enable_testing()


find_package(OpenGL REQUIRED)

set(VULKAN_PATH "C:\\VulkanSDK\\1.2.189.2\\")

set(VULKAN_INCLUDE "C:\\VulkanSDK\\1.2.189.2\\Include")
set(GLFW_INCLUDE "C:\\GLFW\\glfw-3.3.4.bin.WIN64\\include")

set(VULKAN_LIB "C:\\VulkanSDK\\1.2.189.2\\Lib")
set(GLFW_LIB "C:\\GLFW\\glfw-3.3.4.bin.WIN64\\lib-vc2019")

set(VULKAN_LIB_LIST 
    "vulkan-1.lib"
)

add_executable(VulkanGameEngine main.cpp)

# Includes
include_directories(${VULKAN_INCLUDE})
include_directories(${GLFW_INCLUDE})

link_directories(${VULKAN_LIB})
link_directories(${GLFW_LIB})

target_link_libraries(VulkanGameEngine ${VULKAN_LIB}/${VULKAN_LIB_LIST})
target_link_libraries(VulkanGameEngine ${GLFW_LIB}/glfw3.lib)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
