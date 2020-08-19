#include "MoeVkCommandPool.hpp"
#include "../../Exceptions/InitException.hpp"
#include "MoeVkArrayBuffer.hpp"

#include <iostream>

namespace moe {

void MoeVkCommandPool::create(MoeVkLogicalDevice &device,
        MoeVkQueueFamily queueFamily,
        MoeVkFramebuffer& framebuffer,
        MoeVkPipeline& pipeline,
        MoeVkSwapChain& swapChain) {

    // because we only submit commands for drawing, we use the graphics queue here
    auto indices = queueFamily.selectedGraphicsIndex;

    VkCommandPoolCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = indices;
    createInfo.flags = 0;   // can give hints that the buffer is re-recorded regularly

    if (vkCreateCommandPool(device.device(), &createInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw InitException("Failed to create a command Pool.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkCommandPool::destroy(moe::MoeVkLogicalDevice &device) {
    std::cout << "Destroying Command Pool\n";
    vkDestroyCommandPool(device.device(), _pool, nullptr);
}

void MoeVkCommandPool::createCommandBuffers(MoeVkLogicalDevice &device, MoeVkFramebuffer &framebuffer,
                                            MoeVkPipeline &pipeline, MoeVkSwapChain &swapChain,
                                            MoeVkArrayBuffer<moe::Vertex> &vertexBuffer,
                                            MoeVkArrayBuffer<uint32_t>& indexBuffer,
                                            MoeVkUniformBuffer& uniformBuffer) {

    size_t numBuffers = framebuffer.buffers().size();
    _buffer.create(device, *this, numBuffers);

    for (size_t i = 0; i < numBuffers; i++) {
        // setup rendering recording
        // TODO: Customize recording options
        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = 0;
        info.pInheritanceInfo = nullptr;
        if (vkBeginCommandBuffer(_buffer.at(i), &info) != VK_SUCCESS) {
            throw InitException("Failed to beginn recoring command buffer", __FILE__, __FUNCTION__, __LINE__);
        }

        // TODO: Let user decide
        VkClearValue clearColor = {0.f, 0.f, 0.f, 1.f};

        // once a command buffer is recorded, it can not be appended
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.renderPass();
        renderPassInfo.framebuffer = framebuffer.buffers()[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.extent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        // start recording
        vkCmdBeginRenderPass(_buffer.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(_buffer.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline());

        // When setting viewport as dynamic state in the pipeline creation, we MUST set it here
        VkViewport viewport;
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = swapChain.extent().width;
        viewport.height = swapChain.extent().height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(_buffer.at(i), 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = {swapChain.extent().width, swapChain.extent().height};
        vkCmdSetScissor(_buffer.at(i), 0, 1, &scissor);

        // bind vertex buffer
        VkBuffer vertexBuffers[] = {vertexBuffer.buffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(_buffer.at(i), 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(_buffer.at(i), indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);

        // bind descriptor set
        vkCmdBindDescriptorSets(_buffer.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline.layout(), 0, 1,
                &(uniformBuffer.set(i)), 0, nullptr);

        // vertex count, instance count, firstVertex, firstInstance
        vkCmdDrawIndexed(_buffer.at(i), indexBuffer.numVertices(), 1, 0, 0, 0);
        vkCmdEndRenderPass(_buffer.at(i));

        if (vkEndCommandBuffer(_buffer.at(i)) != VK_SUCCESS) {
            throw InitException("Failed to end command buffer", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

void MoeVkCommandPool::destroyCommandBuffers(MoeVkLogicalDevice& device) {
    _buffer.destroy();
}

}