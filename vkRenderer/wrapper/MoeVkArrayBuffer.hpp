#pragma once

#include "MoeVkBuffer.hpp"
#include "../MoeVertex.hpp"
#include <spdlog/spdlog.h>

namespace moe {

enum class MoeBufferUsage : unsigned int {
    vertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    indexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT
};

/**
 * Creates and uploads a generic data array to the GPU via a staging buffer that is not accessible from the CPU anymore
 */
template <class T>
class MoeVkArrayBuffer {
public:
    MoeVkArrayBuffer(MoeVkPhysicalDevice& physDevice, MoeVkLogicalDevice& device,
                     MoeVkCommandPool& commandPool,
                     const std::vector<T>& data,
                     MoeBufferUsage usage,
                     const std::string& name = ""
                     )
        :   _buffer        { physDevice, device, sizeof(Vertex) * data.size(),
                             static_cast<VkBufferUsageFlags>(usage) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             name },
            _numVertices   { (uint32_t)data.size() }
    {
        spdlog::trace("Creating MoeVkArrayBuffer " + name);
        MoeVkBuffer stagingBuffer { physDevice, device, sizeof(Vertex) * data.size(),
                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    "staging for " + name };

        // fill the vertex buffer by mapping the buffer memory
        VkDeviceSize size = sizeof(T) * data.size();
        void* bufferData;
        vkMapMemory(device.device(), stagingBuffer.memory(), 0, size, 0, &bufferData);
        memcpy(bufferData, data.data(), (size_t)size);
        vkUnmapMemory(device.device(), stagingBuffer.memory());
        stagingBuffer.copyToDst(_buffer, commandPool, size);
    }

    ~MoeVkArrayBuffer() {

    }

    VkBuffer& buffer() { return _buffer.buffer(); }
    uint32_t  numVertices() const { return _numVertices; }

private:
    uint32_t    _numVertices;
    MoeVkBuffer _buffer;
};
}