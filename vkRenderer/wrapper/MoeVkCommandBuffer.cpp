//
// Created by titus on 18.08.20.
//

#include "MoeVkCommandBuffer.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkCommandPool.hpp"
#include <assert.h>

namespace moe {


void MoeVkCommandBuffer::create(MoeVkLogicalDevice &device, MoeVkCommandPool& commandPool, uint32_t numBuffers) {

    _device = device.device();
    _pool = commandPool.pool();
    _buffers.resize(numBuffers);

    VkCommandBufferAllocateInfo allocInfo { };
    allocInfo.sType             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext             = nullptr;
    allocInfo.commandPool       = commandPool.pool();
    allocInfo.level             = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // TODO: As parameter
    allocInfo.commandBufferCount = numBuffers;

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, _buffers.data()) != VK_SUCCESS) {
        throw MoeInitError("Failed to create command buffer.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandBuffer::destroy() {
    vkFreeCommandBuffers(_device, _pool, static_cast<uint32_t>(_buffers.size()), _buffers.data());
}

void MoeVkCommandBuffer::startRecording(MoeVkCommandPool& commandPool, VkCommandBufferUsageFlags flags, int bufferIndex) {

    // TODO: Add functions "begin()" and "end()" to MoeVkCommandBuffer
    VkCommandBufferBeginInfo commandBeginInfo { };
    commandBeginInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBeginInfo.pNext              = nullptr;
    commandBeginInfo.flags              = flags;
    commandBeginInfo.pInheritanceInfo   = nullptr;

    if (vkBeginCommandBuffer(_buffers.at(bufferIndex), &commandBeginInfo) != VK_SUCCESS) {
        throw MoeInitError("Failed to begin recording command buffer", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandBuffer::stopRecording(VkQueue& queue, MoeVkCommandPool& commandPool, int bufferIndex) {

    if (vkEndCommandBuffer(_buffers[bufferIndex]) != VK_SUCCESS) {
        throw MoeInitError("Failed to end command buffer recording", __FILE__, __FUNCTION__, __LINE__);
    }

    VkSubmitInfo submitInfo { };
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.waitSemaphoreCount   = 0;
    submitInfo.pWaitSemaphores      = nullptr;
    submitInfo.pWaitDstStageMask    = nullptr;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &(_buffers[bufferIndex]);
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores    = nullptr;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}
}