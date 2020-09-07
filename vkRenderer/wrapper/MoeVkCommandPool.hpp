#pragma once

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkArrayBuffer.hpp"
#include "MoeVkCommandBuffer.hpp"

#include <vector>
#include <assert.h>

namespace moe {

class MoeVkFramebuffer;
class MoeVkSwapChain;
class MoeVkDescriptorSet;

class MoeVkCommandPool {
public:
    MoeVkCommandPool() { }
    ~MoeVkCommandPool() { }

    void create(MoeVkLogicalDevice& device,
            MoeVkQueueFamily queueFamily,
            MoeVkSwapChain& swapChain);
    void destroy(MoeVkLogicalDevice& device);

    VkCommandPool& pool() { return _pool; }

private:
    VkCommandPool _pool;
};
}
