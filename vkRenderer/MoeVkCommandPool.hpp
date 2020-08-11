#pragma once

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkFramebuffer.hpp"

#include <vector>
#include <assert.h>

namespace moe {
class MoeVkCommandPool {
public:
    MoeVkCommandPool() { }
    ~MoeVkCommandPool() { }

    void create(MoeVkLogicalDevice& device, MoeVkQueueFamily queueFamily, MoeVkFramebuffer& framebuffer, MoeVkPipeline& pipeline, MoeVkSwapChain& swapChain);
    void destroy(MoeVkLogicalDevice& device);

    VkCommandBuffer& commandBuffer(unsigned int index) {
        assert(index < buffer.size());
        return buffer[index];
    }

    void createCommandBuffers(MoeVkLogicalDevice& device, MoeVkFramebuffer& framebuffer, MoeVkPipeline& pipeline, MoeVkSwapChain& swapChain);
    void destroyCommandBuffers(MoeVkLogicalDevice& device);

private:
    VkCommandPool pool;

    /**
     * allocates and records the commands for each swap chain image
     */
    std::vector<VkCommandBuffer> buffer;
};
}
