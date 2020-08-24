
#include "MoeRendererCommandBuffer.hpp"
#include "wrapper/MoeVkCommandBuffer.hpp"
#include "wrapper/MoeVkLogicalDevice.hpp"

#include "wrapper/MoeVkFramebuffer.hpp"
#include "wrapper/MoeVkPipeline.hpp"
#include "wrapper/MoeVkSwapChain.hpp"
#include "wrapper/MoeVkUniformBuffer.hpp"

#include "../Exceptions/InitException.hpp"

namespace moe {

MoeRendererCommandBuffer::MoeRendererCommandBuffer(
        MoeVkLogicalDevice &device,
        MoeVkCommandPool &commandPool,
        uint32_t numBuffers)

        : _device { &device }
{
    _buffer.create(device, commandPool, numBuffers);
}

MoeRendererCommandBuffer::~MoeRendererCommandBuffer() {
    _buffer.destroy();
}

void MoeRendererCommandBuffer::record(MoeVkCommandPool& commandPool, std::vector<Drawable> &drawables,
                                      MoeVkFramebuffer &framebuffer,
                                      MoeVkPipeline &pipeline, MoeVkSwapChain &swapChain,
                                      MoeVkArrayBuffer<Vertex> &vertexBuffer,
                                      MoeVkArrayBuffer<uint32_t>& indexBuffer,
                                      MoeVkUniformBuffer& uniformBuffer) {

    // TODO: For now, we assume each drawable has the same geometry
    for (size_t i = 0; i < _buffer.size(); i++) {
        // setup rendering recording
        // TODO: Customize recording options

        _buffer.startRecording(commandPool, 0, i);

        // TODO: Let user decide
        std::vector<VkClearValue> clearColor = {
                { 0.f, 0.f, 0.f, 1.f },   // color buffer
                { 1.f, 0.f }             // depth buffer
        };

        // once a command buffer is recorded, it can not be appended
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.renderPass();
        renderPassInfo.framebuffer = framebuffer.buffers()[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.extent();
        renderPassInfo.clearValueCount = clearColor.size();
        renderPassInfo.pClearValues = clearColor.data();

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
        // TODO: How to bind different geometries here ????
        VkBuffer vertexBuffers[] = {vertexBuffer.buffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(_buffer.at(i), 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(_buffer.at(i), indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);

        // bind descriptor set for each drawable and start a draw call
        for (size_t i = 0; i < drawables.size(); i++) {
            vkCmdBindDescriptorSets(_buffer.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline.layout(), 0, 1,
                                    &(uniformBuffer.set(i)), 0, nullptr);


            // vertex count, instance count, firstVertex, firstInstance
            vkCmdDrawIndexed(_buffer.at(i), indexBuffer.numVertices(), 1, 0, 0, 0);
            vkCmdEndRenderPass(_buffer.at(i));
        }

        if (vkEndCommandBuffer(_buffer.at(i)) != VK_SUCCESS) {
            throw InitException("Failed to end command buffer", __FILE__, __FUNCTION__, __LINE__);
        }
    }

}
}