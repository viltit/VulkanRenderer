include_directories(..)

project(vkRenderer)

#Locate SDL
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#Locate Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

add_library(
        vkRenderer
        MoeVkFunctions.hpp
        MoeVkInstance.cpp
        MoeVkRenderer.cpp
        MoeVkQueueFamily.hpp
        VkWindow.cpp
        MoeVkPhysicalDevice.cpp
        MoeVkLogicalDevice.cpp
        MoeVkSwapChain.cpp
        MoeVkSwapChainProps.hpp
        MoeVkPipeline.cpp
        MoeVkFramebuffer.cpp
        MoeVkCommandPool.cpp
        MoeVkSemaphore.cpp
        MoeVkFence.cpp
)

target_link_libraries(vkRenderer
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES})