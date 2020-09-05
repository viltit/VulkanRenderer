
#include "MoeVkBuffer.hpp"
#include "MoeVkCommandPool.hpp"
#include "MoeVkUtils.hpp"
#include "MoeVkCommandBuffer.hpp"

#include <spdlog/spdlog.h>

namespace moe {

MoeVkBuffer::MoeVkBuffer(MoeVkPhysicalDevice &physDevice, MoeVkLogicalDevice &device,
                         VkDeviceSize size, VkBufferUsageFlags bufferFlags,
                         VkMemoryPropertyFlags memoryFlags, const std::string& name)
        :   _device{ &device },
            _name { name } {

    spdlog::trace("Creating MoeVkBuffer " + name);

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = bufferFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.device(), &info, nullptr, &_buffer) != VK_SUCCESS) {
        throw MoeInitError("Failed to create vertex buffer", __FILE__, __FUNCTION__, __LINE__);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.device(), _buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = MoeVkUtils::getMemoryType(physDevice, memRequirements.memoryTypeBits,
                                              memoryFlags);
    if (vkAllocateMemory(device.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS) {
        throw MoeInitError("Failed to allocate memory from GPU", __FILE__, __FUNCTION__, __LINE__);
    }
    vkBindBufferMemory(device.device(), _buffer, _memory, 0);
}

MoeVkBuffer::~MoeVkBuffer() {
    spdlog::trace("Destroying MoeVkBuffer " + _name);
    vkDestroyBuffer(_device->device(), _buffer, nullptr);
    vkFreeMemory(_device->device(), _memory, nullptr);
}

void MoeVkBuffer::copyToDst(MoeVkBuffer& destination, MoeVkCommandPool& commandPool, VkDeviceSize size) {

    MoeVkCommandBuffer commandBuffer { };
    commandBuffer.create(*_device, commandPool, 1);
    commandBuffer.startRecording(commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 0);

    VkBufferCopy copy;
    copy.dstOffset = 0;
    copy.srcOffset = 0;
    copy.size = size;
    vkCmdCopyBuffer(commandBuffer.at(0), _buffer, destination.buffer(), 1, &copy);

    commandBuffer.stopRecording(_device->graphicsQueue(), commandPool, 0);

    // TODO: Use a fence here
    vkQueueWaitIdle(_device->graphicsQueue());

    commandBuffer.destroy();

}
}