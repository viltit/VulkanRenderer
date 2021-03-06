#include "MoeVkCommandPool.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkDescriptorPool.hpp"

#include <iostream>

namespace moe {

void MoeVkCommandPool::create(MoeVkLogicalDevice &device,
        MoeVkQueueFamily queueFamily,
        MoeVkSwapChain& swapChain) {

    // because we only submit commands for drawing, we use the graphics queue here
    auto indices = queueFamily.selectedGraphicsIndex;

    VkCommandPoolCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = indices;
    createInfo.flags = 0;   // can give hints that the buffer is re-recorded regularly

    if (vkCreateCommandPool(device.device(), &createInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw MoeInitError("Failed to create a command Pool.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandPool::destroy(moe::MoeVkLogicalDevice &device) {
    std::cout << "Destroying Command Pool\n";
    vkDestroyCommandPool(device.device(), _pool, nullptr);
}

}