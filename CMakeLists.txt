cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

project(moe)

# Staticly link spdlog, a logger library for C++
# See https://github.com/gabime/spdlog
add_subdirectory(external/spdlog)

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
        vkRenderer/MoeVkSwapChain.cpp
        vkRenderer/MoeVkSwapChainProps.hpp
        vkRenderer/MoeVkPipeline.cpp
        vkRenderer/MoeVkFramebuffer.cpp
        vkRenderer/MoeVkCommandPool.cpp
        vkRenderer/MoeVkSemaphore.cpp
        vkRenderer/MoeVkFence.cpp
        Colors/ColorRGB.hpp)


target_link_libraries(vkEngine
        spdlog
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES})