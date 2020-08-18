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
        throw InitException("Failed to create command buffer.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandBuffer::destroy() {
    vkFreeCommandBuffers(_device, _pool, static_cast<uint32_t>(_buffers.size()), _buffers.data());
}

VkCommandBuffer &MoeVkCommandBuffer::at(int index) {
    assert(index < _buffers.size());
    return _buffers[index];
}
}