#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include "MoeVertex.hpp"

// TODO: Generalize class ? (ie, just buffer)
namespace moe {

class MoeVkVertexBuffer {
public:
    MoeVkVertexBuffer(MoeVkPhysicalDevice& physDevice, MoeVkLogicalDevice& device, const std::vector<Vertex>& vertices);
    ~MoeVkVertexBuffer();

    VkBuffer& buffer() { return _buffer; }

private:
    uint32_t  getMemoryType(MoeVkPhysicalDevice& physDevice, uint32_t typeFilter, VkMemoryPropertyFlags flags);

    VkDevice _device;
    VkBuffer _buffer;
    VkDeviceMemory _memory;
};
}