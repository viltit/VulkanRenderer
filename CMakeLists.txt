cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

project(moe)

#Locate SDL
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#Locate Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

add_executable(vkEngine main.cpp
        vkRenderer/MoeVkFunctions.hpp
        vkRenderer/MoeVkInstance.cpp
        vkRenderer/MoeVkRenderer.cpp
        vkRenderer/MoeVkQueueFamily.hpp
        vkRenderer/VkWindow.cpp
        vkRenderer/MoeVkPhysicalDevice.cpp
        vkRenderer/MoeVkLogicalDevice.cpp
        Colors/ColorRGB.hpp)

target_link_libraries(vkEngine
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES})