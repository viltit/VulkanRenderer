
#include "MoeVkBuffer.hpp"
#include "MoeVkCommandPool.hpp"
#include "MoeVkUtils.hpp"
#include "MoeVkCommandBuffer.hpp"

namespace moe {

MoeVkBuffer::MoeVkBuffer(MoeVkPhysicalDevice &physDevice, MoeVkLogicalDevice &device,
                         VkDeviceSize size, VkBufferUsageFlags bufferFlags,
                         VkMemoryPropertyFlags memoryFlags)
        : _device{ &device } {
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = bufferFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.device(), &info, nullptr, &_buffer) != VK_SUCCESS) {
        throw InitException("Failed to create vertex buffer", __FILE__, __FUNCTION__, __LINE__);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.device(), _buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = MoeVkUtils::getMemoryType(physDevice, memRequirements.memoryTypeBits,
                                              memoryFlags);
    if (vkAllocateMemory(device.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS) {
        throw InitException("Failed to allocate memory from GPU", __FILE__, __FUNCTION__, __LINE__);
    }
    vkBindBufferMemory(device.device(), _buffer, _memory, 0);
}

MoeVkBuffer::~MoeVkBuffer() {
    vkDestroyBuffer(_device->device(), _buffer, nullptr);
    vkFreeMemory(_device->device(), _memory, nullptr);
}

void MoeVkBuffer::copyToDst(MoeVkBuffer& destination, MoeVkCommandPool& commandPool, VkDeviceSize size) {

    MoeVkCommandBuffer commandBuffer { };
    commandBuffer.create(*_device, commandPool, 1);

    VkCommandBufferBeginInfo bufferBeginInfo;
    bufferBeginInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.pNext       = nullptr;
    bufferBeginInfo.flags       = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    bufferBeginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer.at(0), &bufferBeginInfo) != VK_SUCCESS) {
        throw InitException("Could not beginn recording command buffer", __FILE__, __FUNCTION__, __LINE__);
    }

    VkBufferCopy copy;
    copy.dstOffset = 0;
    copy.srcOffset = 0;
    copy.size = size;
    vkCmdCopyBuffer(commandBuffer.at(0), _buffer, destination.buffer(), 1, &copy);

    if (vkEndCommandBuffer(commandBuffer.at(0)) != VK_SUCCESS) {
        throw InitException("Failed to end command buffer recording.", __FILE__, __FUNCTION__, __LINE__);
    }
    VkSubmitInfo submitInfo { };
    submitInfo.sType            = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext            = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(commandBuffer.at(0));
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    // graphics queue is guaranteed to allow memory tranfers. We COULD get some more performance by using
    // a dedicated transfer queue.
    if (vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw InitException("Failed to end command buffer recording.", __FILE__, __FUNCTION__, __LINE__);
    }

    // TODO: Use a fence here
    vkQueueWaitIdle(_device->graphicsQueue());

    commandBuffer.destroy();

}
}