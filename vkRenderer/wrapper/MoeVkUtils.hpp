#pragma once

#include <vulkan/vulkan.h>

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
        throw InitException("Failed to find suitable memory type.", __FILE__, __FUNCTION__, __LINE__);
    }
};

struct UBO {
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 P;
};

}
