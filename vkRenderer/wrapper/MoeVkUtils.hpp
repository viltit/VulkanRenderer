#pragma once

#include <vulkan/vulkan.h>

// TODO: Make sure we define this before GLM gets included the first time !!
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace moe {
/*
 * Helper functions needed by different other MoeVk-Classes
 * */
class MoeVkUtils {
public:
    static uint32_t getMemoryType(MoeVkPhysicalDevice& physDevice, uint32_t typeFilter, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties props;
        vkGetPhysicalDeviceMemoryProperties(physDevice.device(), &props);

        for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (props.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
        throw MoeInitError("Failed to find suitable memory type.", __FILE__, __FUNCTION__, __LINE__);
    }
    static bool isStencilFormat(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }
};

struct UBO {
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 P;
    glm::vec3 lightPos;
};

}
