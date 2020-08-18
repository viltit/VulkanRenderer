#pragma once

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkFramebuffer.hpp"
#include "MoeVkArrayBuffer.hpp"
#include "MoeVkCommandBuffer.hpp"

#include <vector>
#include <assert.h>

namespace moe {

class MoeVkCommandPool {
public:
    MoeVkCommandPool() { }
    ~MoeVkCommandPool() { }

    void create(MoeVkLogicalDevice& device,
            MoeVkQueueFamily queueFamily,
            MoeVkFramebuffer& framebuffer,
            MoeVkPipeline& pipeline,
            MoeVkSwapChain& swapChain);
    void destroy(MoeVkLogicalDevice& device);

    VkCommandBuffer& commandBuffer(unsigned int index) {
        return _buffer.at(index);
    }

    void createCommandBuffers(MoeVkLogicalDevice& device,
                              MoeVkFramebuffer& framebuffer,
                              MoeVkPipeline& pipeline,
                              MoeVkSwapChain& swapChain,
                              MoeVkArrayBuffer<Vertex>& vertexBuffer,
                              MoeVkArrayBuffer<uint32_t>& indexBuffer,
                              MoeVkUniformBuffer& uniformBuffer);

    void destroyCommandBuffers(MoeVkLogicalDevice& device);

    VkCommandPool& pool() { return _pool; }

private:
    VkCommandPool _pool;

    /**
     * allocates and records the commands for each swap chain image
     * TODO: This may be better suited in a separate class so we could use a CommandPool independently
     */
    MoeVkCommandBuffer _buffer;
};
}
