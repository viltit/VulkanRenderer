#pragma once

#include "wrapper/MoeVkCommandBuffer.hpp"
#include "MoeDrawable.hpp"
#include "MoeVertex.hpp"
#include "wrapper/MoeVkArrayBuffer.hpp"

/**
 * This command buffer is made for rendering a scene of object. It will be re-recorded every frame
 *
 * TODO allow different geometries
 * TODO allow different pipelines (ie, objects with different shaders)
 *
 * TODO minor: Only re-record when the visible scene has changed or use sub-buffers for some performance gain
 * (only gets relevant for really huge scenes)
 */
namespace moe {

class MoeVkLogicalDevice;
class MoeVkCommandPool;
class MoeVkFramebuffer;
class MoeVkPipeline;
class MoeVkSwapChain;
class MoeVkUniformBuffer;

class MoeRendererCommandBuffer {
public:

    MoeRendererCommandBuffer(MoeVkLogicalDevice& device, MoeVkCommandPool& commandPool, uint32_t numBuffers);
    ~MoeRendererCommandBuffer();

    void record(MoeVkCommandPool& commandPool, std::vector<Drawable>& drawables,
                MoeVkFramebuffer &framebuffer,
                MoeVkPipeline &pipeline, MoeVkSwapChain &swapChain,
                MoeVkArrayBuffer<Vertex> &vertexBuffer,
                MoeVkArrayBuffer<uint32_t>& indexBuffer,
                MoeVkUniformBuffer& uniformBuffer);


private:
    MoeVkCommandBuffer  _buffer;
    MoeVkLogicalDevice* _device;
};
}