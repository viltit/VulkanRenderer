#include <assert.h>
#include <set>
#include <iostream>

#include "MoeVkLogicalDevice.hpp"
#include "../../Exceptions/MoeExceptions.hpp"

namespace moe {

MoeVkLogicalDevice::MoeVkLogicalDevice() {
    std::cout << "Creating logical device\n";
}

MoeVkLogicalDevice::~MoeVkLogicalDevice() {
    std::cout << "Destroying logical device\n";
    vkDestroyDevice(_device, nullptr);
}

void MoeVkLogicalDevice::create(VkInstance instance, MoeVkPhysicalDevice& physDevice,
        const std::vector<const char*>& extensions,
        unsigned int desiredQueueCount) {

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
    features.samplerAnisotropy = true;  // TODO: Optional

    VkDeviceCreateInfo createInfo { };
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                    = nullptr;
    createInfo.flags                    = 0;
    createInfo.queueCreateInfoCount     = queueCreateInfos.size();
    createInfo.pQueueCreateInfos        = queueCreateInfos.data();
    // Old vulkan versions needed the validation layer here too - for newer ones, it suffices to activate them on the instance
    createInfo.enabledLayerCount        = 0;
    createInfo.ppEnabledLayerNames      = nullptr;
    createInfo.enabledExtensionCount    = extensions.size();
    createInfo.ppEnabledExtensionNames  = extensions.data();
    createInfo.pEnabledFeatures         = &features;

    if (vkCreateDevice(physDevice.device(), &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw MoeInitError("Could not create a logical GPU.", __FILE__, __FUNCTION__, __LINE__);
    }

    // get a handle to our queues:
    vkGetDeviceQueue(_device, queues.front(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, queues.back(), 0, &_presentationQueue);
}
}