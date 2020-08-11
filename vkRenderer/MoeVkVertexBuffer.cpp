
#include "MoeVkVertexBuffer.hpp"
#include "../Exceptions/InitException.hpp"

#include <memory.h>

namespace moe {

MoeVkVertexBuffer::MoeVkVertexBuffer(MoeVkPhysicalDevice& physDevice, MoeVkLogicalDevice &device, const std::vector<Vertex> &vertices)
    : _device { device.device() }
{
    VkBufferCreateInfo info { };
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = sizeof(Vertex) * vertices.size();
    info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.device(), &info, nullptr, &_buffer) != VK_SUCCESS) {
        throw InitException("Failed to create vertex buffer", __FILE__, __FUNCTION__, __LINE__);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.device(), _buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo { };
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryType(physDevice, memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (vkAllocateMemory(device.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS) {
        throw InitException("Failed to allocate memory from GPU", __FILE__, __FUNCTION__, __LINE__);
    }
    vkBindBufferMemory(device.device(), _buffer, _memory, 0);

    // fill the vertex buffer by mapping the buffer memory
    void* data;
    vkMapMemory(device.device(), _memory, 0, info.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)info.size);
    vkUnmapMemory(device.device(), _memory);
}

MoeVkVertexBuffer::~MoeVkVertexBuffer() {
    vkDestroyBuffer(_device, _buffer, nullptr);
    vkFreeMemory(_device, _memory, nullptr);
}

uint32_t MoeVkVertexBuffer::getMemoryType(MoeVkPhysicalDevice& physDevice, uint32_t typeFilter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(physDevice.device(), &props);

    // TODO: Always try to use dedicated vram in props.memoryHeaps
    for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (props.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;
        }
    }
    throw InitException("Failed to find suitable memory type.", __FILE__, __FUNCTION__, __LINE__);
}

}