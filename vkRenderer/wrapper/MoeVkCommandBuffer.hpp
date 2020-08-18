#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace moe {

class MoeVkLogicalDevice;
class MoeVkCommandPool;

class MoeVkCommandBuffer {
public:
    MoeVkCommandBuffer() { }
    ~MoeVkCommandBuffer() { }

    void create(MoeVkLogicalDevice& device, MoeVkCommandPool& commandPool, uint32_t numBuffers);
    void destroy();

    VkCommandBuffer& at(int index);

private:
    std::vector<VkCommandBuffer> _buffers;
    VkDevice _device;
    VkCommandPool _pool;
};
}