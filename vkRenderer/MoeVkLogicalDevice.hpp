#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkPhysicalDevice.hpp"

namespace moe {
class MoeVkLogicalDevice {
public:
    MoeVkLogicalDevice();
    ~MoeVkLogicalDevice();

    void create(VkInstance instance, MoeVkPhysicalDevice device, unsigned int desiredQueueCount = 4);

private:
    VkDevice _device;
};
}