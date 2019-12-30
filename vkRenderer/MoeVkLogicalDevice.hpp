#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkPhysicalDevice.hpp"

namespace moe {

/**
 * The logical device is an abstraction thats allows us to communicate with the physical device. We could build several logical
 * devices on top of the same physical, and we can use only a fraction of the physical device capabilities in the logical
 * device
 */
class MoeVkLogicalDevice {
public:
    MoeVkLogicalDevice();
    ~MoeVkLogicalDevice();

    void create(VkInstance instance, MoeVkPhysicalDevice& device,
            const std::vector<const char*>& extensions,
            unsigned int desiredQueueCount = 4);

    VkDevice& device() { return _device; }

private:

    // handle to the logical device
    VkDevice _device;
    // handles to the queues of our device
    VkQueue _graphicsQueue;
    VkQueue _presentationQueue;
};
}