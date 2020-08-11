#include "MoeVkCommandPool.hpp"
#include "../Exceptions/InitException.hpp"

#include <iostream>

namespace moe {

void MoeVkCommandPool::create(MoeVkLogicalDevice &device,
        MoeVkQueueFamily queueFamily,
        MoeVkFramebuffer& framebuffer,
        MoeVkPipeline& pipeline,
        MoeVkSwapChain& swapChain,
        MoeVkVertexBuffer& vertexBuffer) {

    // because we only submit commands for drawing, we use the graphics queue here
    auto indices = queueFamily.selectedGraphicsIndex;

    VkCommandPoolCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = indices;
    createInfo.flags = 0;   // can give hints that the buffer is re-recorded regularly

    if (vkCreateCommandPool(device.device(), &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw InitException("Failed to create a command Pool.", __FILE__, __FUNCTION__, __LINE__);
    }

    createCommandBuffers(device, framebuffer, pipeline, swapChain, vertexBuffer);
}

void MoeVkCommandPool::destroy(moe::MoeVkLogicalDevice &device) {
    std::cout << "Destroying Command Pool\n";
    vkDestroyCommandPool(device.device(), pool, nullptr);
}

// TODO: More flexibility. Right now this is hardcoded for a specific vertex setup
void MoeVkCommandPool::createCommandBuffers(moe::MoeVkLogicalDevice &device,
        MoeVkFramebuffer& framebuffer,
        MoeVkPipeline& pipeline,
        MoeVkSwapChain& swapChain,
        MoeVkVertexBuffer& vertexBuffer) {
    buffer.resize(framebuffer.buffers().size());

    VkCommandBufferAllocateInfo allocInfo { };
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)buffer.size();

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, buffer.data()) != VK_SUCCESS) {
        throw new InitException("Failed to allocate command buffers.", __FILE__, __FUNCTION__, __LINE__);
    }

    for (size_t i = 0; i < buffer.size(); i++) {
        // setup rendering recording
        // TODO: Customize recording options
        VkCommandBufferBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = 0;
        info.pInheritanceInfo = nullptr;
        if (vkBeginCommandBuffer(buffer[i], &info) != VK_SUCCESS) {
            throw InitException("Failed to beginn recoring command buffer", __FILE__, __FUNCTION__, __LINE__);
        }

        // TODO: Let user decide
        VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };

        // once a command buffer is recorded, it can not be appended
        VkRenderPassBeginInfo renderPassInfo { };
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.renderPass();
        renderPassInfo.framebuffer = framebuffer.buffers()[i];
        renderPassInfo.renderArea.offset = { 0,0 };
        renderPassInfo.renderArea.extent = swapChain.extent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        // start recording
        vkCmdBeginRenderPass(buffer[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline());

        // bind vertex buffer
        VkBuffer vertexBuffers[] = { vertexBuffer.buffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer[i], 0, 1, vertexBuffers, offsets);

        // vertex count, instance count, firstVertex, firstInstance
        vkCmdDraw(buffer[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(buffer[i]);

        if (vkEndCommandBuffer(buffer[i]) != VK_SUCCESS) {
            throw InitException("Failed to end command buffer", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

void MoeVkCommandPool::destroyCommandBuffers(MoeVkLogicalDevice& device) {
    vkFreeCommandBuffers(device.device(), pool, static_cast<uint32_t>(buffer.size()), buffer.data());
}

}