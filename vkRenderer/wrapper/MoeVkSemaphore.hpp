#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"

namespace moe {

class MoeVkSemaphore {
public:
    MoeVkSemaphore() { }
    ~MoeVkSemaphore() { }

    void create(MoeVkLogicalDevice& device);
    void destroy(MoeVkLogicalDevice& device);

    VkSemaphore& semaphore() { return _semaphore; }

private:
    VkSemaphore _semaphore;
};

}
