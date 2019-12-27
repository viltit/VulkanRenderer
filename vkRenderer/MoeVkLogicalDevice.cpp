#include <assert.h>

#include "MoeVkLogicalDevice.hpp"

namespace moe {

MoeVkLogicalDevice::MoeVkLogicalDevice() {

}

MoeVkLogicalDevice::~MoeVkLogicalDevice() {

}

void MoeVkLogicalDevice::create(VkInstance instance, MoeVkPhysicalDevice device, unsigned int desiredQueueCount) {

    assert(device.queueFamily().graphics.size() > 0);
    assert(device.queueFamily().numQueues.size() == device.queueFamily().graphics.size());

    // we just take the first queue family that supports graphics for now
    unsigned int queueIndex = device.queueFamily().graphics[0];
    unsigned int maxQueues = device.queueFamily().numQueues[0];

    unsigned int numQueues = maxQueues >= desiredQueueCount ? desiredQueueCount : maxQueues;




}

}