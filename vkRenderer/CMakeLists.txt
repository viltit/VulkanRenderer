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
        wrapper/MoeTexture.cpp
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
        wrapper/MoeVkDescriptorPool.cpp
        wrapper/MoeVkDescriptorSet.cpp
        wrapper/MoeVkCommandBuffer.cpp
        wrapper/MoeVkImage.cpp
        wrapper/MoeDepthTexture.cpp
        MoeVertex.hpp
        wrapper/MoeVkArrayBuffer.hpp
        wrapper/MoeVkBuffer.cpp
        MoeDrawable.hpp
        MoeVkDrawable.cpp
        wrapper/MoeVkDescriptorPool.cpp
)

target_link_libraries(vkRenderer
        PUBLIC spdlog
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES})