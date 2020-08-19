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

    void startRecording(MoeVkCommandPool& commandPool, VkCommandBufferUsageFlags flags, int bufferIndex);
    void stopRecording(VkQueue& queue, MoeVkCommandPool& commandPool, int bufferIndex);

    VkCommandBuffer& at(int i) {
        // TODO: Make sure i is a valid index
        return _buffers[i];
    }

private:
    std::vector<VkCommandBuffer> _buffers;
    VkDevice _device;
    VkCommandPool _pool;
};
}