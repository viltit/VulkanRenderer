#pragma once

#include "MoeVkBuffer.hpp"
#include "MoeVertex.hpp"

// Uses staging buffer to upload data into VRAM that is not accessible from the CPU anymore
namespace moe {

class MoeVkVertexBuffer {
public:
    MoeVkVertexBuffer(MoeVkPhysicalDevice& physDevice, MoeVkLogicalDevice& device,
            MoeVkCommandPool& commandPool,
            const std::vector<Vertex>& vertices);
    ~MoeVkVertexBuffer();

    VkBuffer& buffer() { return _buffer.buffer(); }

private:
    MoeVkBuffer _buffer;
};
}