#include <assert.h>
#include <set>
#include <iostream>

#include "MoeVkLogicalDevice.hpp"
#include "../Exceptions/InitException.hpp"

namespace moe {

MoeVkLogicalDevice::MoeVkLogicalDevice() {

}

MoeVkLogicalDevice::~MoeVkLogicalDevice() {
    vkDestroyDevice(_device, nullptr);
}

void MoeVkLogicalDevice::create(VkInstance instance, MoeVkPhysicalDevice physDevice, unsigned int desiredQueueCount) {

    assert(physDevice.queueFamily().graphics.size() > 0);
    assert(physDevice.queueFamily().presentation.size() > 0);
    assert(physDevice.queueFamily().numQueues.size() == physDevice.queueFamily().graphics.size());

    std::vector<uint> queues = physDevice.queueFamily().pickBest();
    float queuePriorities = 1.0;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queues.size());
    for (size_t i = 0; i < queues.size(); i++) {
        VkDeviceQueueCreateInfo queueCreateInfo { };
        queueCreateInfo.sType               = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext               = nullptr;
        queueCreateInfo.flags               = 0;
        queueCreateInfo.queueFamilyIndex    = queues[i];
        queueCreateInfo.queueCount          = 1;   // TODO: Is  it even worth taking more than 1 ??
        queueCreateInfo.pQueuePriorities    = &queuePriorities;
        queueCreateInfos[i] = queueCreateInfo;
    }

    // TODO - add specific features we need from the gpu -> only add features we really use !
    VkPhysicalDeviceFeatures features = { };
    features.geometryShader = true;

    VkDeviceCreateInfo createInfo { };
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                    = nullptr;
    createInfo.flags                    = 0;
    createInfo.queueCreateInfoCount     = queueCreateInfos.size();
    createInfo.pQueueCreateInfos        = queueCreateInfos.data();
    // Old vulkan versions needed the validation layer here too - for newer ones, it suffices to activate them on the instance
    createInfo.enabledLayerCount        = 0;
    createInfo.ppEnabledLayerNames      = nullptr;
    createInfo.enabledExtensionCount    = 0;
    createInfo.ppEnabledExtensionNames  = nullptr;
    createInfo.pEnabledFeatures         = &features;

    if (vkCreateDevice(physDevice.device(), &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw InitException("Could not create a logical GPU.", __FILE__, __FUNCTION__, __LINE__);
    }

    // get a handle to our queues:
    vkGetDeviceQueue(_device, queues.front(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, queues.back(), 0, &_presentationQueue);
}
}