
#include "MoeVkVertexBuffer.hpp"
#include "../Exceptions/InitException.hpp"

#include <memory.h>

namespace moe {

MoeVkVertexBuffer::MoeVkVertexBuffer(MoeVkPhysicalDevice& physDevice,
        MoeVkLogicalDevice &device,
        MoeVkCommandPool& commandPool,
        const std::vector<Vertex> &vertices)
    :   _buffer        { physDevice, device, sizeof(Vertex) * vertices.size(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
{
    MoeVkBuffer stagingBuffer { physDevice, device, sizeof(Vertex) * vertices.size(),
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    // fill the vertex buffer by mapping the buffer memory
    VkDeviceSize size = sizeof(Vertex) * vertices.size();
    void* data;
    vkMapMemory(device.device(), stagingBuffer.memory(), 0, size, 0, &data);
    memcpy(data, vertices.data(), (size_t)size);
    vkUnmapMemory(device.device(), stagingBuffer.memory());

    // TODO copy staging buffer
    stagingBuffer.copyToDst(_buffer, commandPool, size);
}

MoeVkVertexBuffer::~MoeVkVertexBuffer() {
    // nothing to do, all resources are handles by MoeVkBuffer
}

}