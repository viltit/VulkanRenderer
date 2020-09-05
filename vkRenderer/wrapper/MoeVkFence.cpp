
#include "MoeVkFence.hpp"
#include "MoeExceptions.hpp"

namespace moe {
void MoeVkFence::create(MoeVkLogicalDevice &device) {

    VkFenceCreateInfo info { };
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(device.device(), &info, nullptr, &_fence) != VK_SUCCESS) {
        throw MoeInitError("Failed to create a fence", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkFence::destroy(MoeVkLogicalDevice &device) {
    vkDestroyFence(device.device(), _fence, nullptr);
}
}