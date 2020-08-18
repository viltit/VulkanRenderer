project(vkRenderer)

# Vector math header only lib, see https://glm.g-truc.net/0.9.9/index.html
include_directories(../glm)

#Locate SDL
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


#Locate Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

add_library(
        vkRenderer
        stb_image.h
        Image.cpp
        wrapper/MoeVkFunctions.hpp
        wrapper/MoeVkInstance.cpp
        MoeVkRenderer.cpp
        wrapper/MoeVkQueueFamily.hpp
        VkWindow.cpp
        wrapper/MoeVkPhysicalDevice.cpp
        wrapper/MoeVkLogicalDevice.cpp
        wrapper/MoeVkSwapChain.cpp
        wrapper/MoeVkSwapChainProps.hpp
        wrapper/MoeVkPipeline.cpp
        wrapper/MoeVkFramebuffer.cpp
        wrapper/MoeVkCommandPool.cpp
        wrapper/MoeVkSemaphore.cpp
        wrapper/MoeVkFence.cpp
        wrapper/MoeVkUtils.hpp
        wrapper/MoeVkUniformBuffer.cpp
        wrapper/MoeVkCommandBuffer.cpp
        MoeVertex.hpp
        wrapper/MoeVkArrayBuffer.hpp
        wrapper/MoeVkBuffer.cpp
        MoeDrawable.hpp
        wrapper/MoeVkUniformBuffer.cpp
)

target_link_libraries(vkRenderer
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES})