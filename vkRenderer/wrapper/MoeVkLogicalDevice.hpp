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

    // weird thing may happen on a shallow copy (like trying to allocate stuff from a VK_NULL_HANDLE)
    // TODO: Either implement copy / move or delete them for ALL wrappers
    MoeVkLogicalDevice(const MoeVkLogicalDevice& other) = delete;
    MoeVkLogicalDevice(MoeVkLogicalDevice&& other) = delete;
    MoeVkLogicalDevice& operator=(const MoeVkLogicalDevice& other) = delete;
    MoeVkLogicalDevice& operator=(MoeVkLogicalDevice&& other) = delete;


    void create(VkInstance instance, MoeVkPhysicalDevice& device,
            const std::vector<const char*>& extensions,
            unsigned int desiredQueueCount = 4);

    VkDevice& device() { return _device; }
    VkQueue& graphicsQueue() { return _graphicsQueue; }
    VkQueue& presentationQueue() { return _presentationQueue; }

private:

    // handle to the logical device
    VkDevice _device;
    // handles to the queues of our device
    VkQueue _graphicsQueue;
    VkQueue _presentationQueue;
};
}