#include "MoeVkCommandPool.hpp"
#include "../../Exceptions/InitException.hpp"
#include "MoeVkArrayBuffer.hpp"
#include "MoeVkDescriptorSet.hpp"
#include "MoeVkPipeline.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkDescriptorPool.hpp"
#include "MoeVkFramebuffer.hpp"

#include <iostream>

namespace moe {

void MoeVkCommandPool::create(MoeVkLogicalDevice &device,
        MoeVkQueueFamily queueFamily,
        MoeVkPipeline& pipeline,
        MoeVkSwapChain& swapChain) {

    // because we only submit commands for drawing, we use the graphics queue here
    auto indices = queueFamily.selectedGraphicsIndex;

    VkCommandPoolCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = indices;
    createInfo.flags = 0;   // can give hints that the buffer is re-recorded regularly

    if (vkCreateCommandPool(device.device(), &createInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw InitException("Failed to create a command Pool.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandPool::destroy(moe::MoeVkLogicalDevice &device) {
    std::cout << "Destroying Command Pool\n";
    vkDestroyCommandPool(device.device(), _pool, nullptr);
}


void MoeVkCommandPool::destroyCommandBuffers(MoeVkLogicalDevice& device) {
   // _buffer.destroy();
}

}