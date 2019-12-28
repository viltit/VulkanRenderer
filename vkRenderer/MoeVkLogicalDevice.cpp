#include <assert.h>

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
    assert(physDevice.queueFamily().numQueues.size() == physDevice.queueFamily().graphics.size());

    // we just take the first queue family that supports graphics for now
    // TODO: Take the best queueFamily !! What is the best ? Do not overthink: Just take a dedicated transfer queue if there is one
    unsigned int queueIndex = physDevice.queueFamily().graphics[0];
    unsigned int maxQueues = physDevice.queueFamily().numQueues[0];

    unsigned int numQueues = maxQueues >= desiredQueueCount ? desiredQueueCount : maxQueues;
    float queuePriorities = 1.0;

    VkDeviceQueueCreateInfo queueCreateInfo { };
    queueCreateInfo.sType               = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext               = nullptr;
    queueCreateInfo.flags               = 0;
    queueCreateInfo.queueFamilyIndex    = queueIndex;
    queueCreateInfo.queueCount          = numQueues;
    queueCreateInfo.pQueuePriorities    = &queuePriorities;

    // TODO - add specific features we need from the gpu -> only add features we really use !
    VkPhysicalDeviceFeatures features = { };
    features.geometryShader = true;

    VkDeviceCreateInfo createInfo { };
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                    = nullptr;
    createInfo.flags                    = 0;
    createInfo.queueCreateInfoCount     = 1;    // TODO: We could use multiple queues
    createInfo.pQueueCreateInfos        = &queueCreateInfo;
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
    // TODO: We defined multiple queues above -> we need multiple queue handles to take advantage of this. For now, we just take queue 0
    vkGetDeviceQueue(_device, queueIndex, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, queueIndex, 0, &_presentationQueue);
}
}