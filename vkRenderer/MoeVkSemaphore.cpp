#include <iostream>

#include "MoeVkSemaphore.hpp"
#include "../Exceptions/InitException.hpp"

namespace moe {
    void MoeVkSemaphore::create(MoeVkLogicalDevice& device) {
        VkSemaphoreCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        if (vkCreateSemaphore(device.device(), &info, nullptr, &_semaphore) != VK_SUCCESS) {
            throw InitException("Failed to create semaphore", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    void MoeVkSemaphore::destroy(moe::MoeVkLogicalDevice &device) {
        std::cout << "Destroying semaphore." << std::endl;
        vkDestroySemaphore(device.device(), _semaphore, nullptr);
    }
}

