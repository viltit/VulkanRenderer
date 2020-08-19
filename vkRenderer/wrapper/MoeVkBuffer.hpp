#pragma once

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include <vulkan/vulkan.h>
#include "../../Exceptions/InitException.hpp"

namespace moe {

class MoeVkCommandPool;

class MoeVkBuffer {
public:
    MoeVkBuffer() { }
    MoeVkBuffer(MoeVkPhysicalDevice& physDevice, MoeVkLogicalDevice &device,
            VkDeviceSize size, VkBufferUsageFlags bufferFlags,
            VkMemoryPropertyFlags memoryFlags,
            const std::string& name = "");

    ~MoeVkBuffer();

    VkBuffer& buffer()          { return _buffer; }
    VkDeviceMemory& memory()    { return _memory; }

    /**
     * NOT an identical copy: Copies from host visible staging buffer to a device buffer,
     * ie. the calling buffer MUST have VK_BUFFER_USAGE_TRANSFER_SRC_BIT set, and the buffer given to the function
     * MUST have VK_BUFFER_USAGE_TRANSFER_DST_BIT set
     */
    void copyToDst(MoeVkBuffer& destination, MoeVkCommandPool& commandPool, VkDeviceSize size);

private:
    uint32_t getMemoryType(MoeVkPhysicalDevice& physDevice, uint32_t typeFilter, VkMemoryPropertyFlags flags);

    MoeVkLogicalDevice*  _device;
    VkBuffer        _buffer;
    VkDeviceMemory  _memory;

    const std::string _name;
};
}