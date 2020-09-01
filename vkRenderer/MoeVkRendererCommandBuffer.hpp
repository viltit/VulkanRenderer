#pragma once

#include <vector>
#include "MoeVkDrawable.hpp"
#include "MoeVertex.hpp"
#include "wrapper/MoeVkArrayBuffer.hpp"
#include "wrapper/MoeVkCommandBuffer.hpp"

namespace moe {

class MoeVkCommandPool;
class MoeVkLogicalDevice;
class MoeVkFramebuffer;
class MoeVkPipeline;
class MoeVkSwapChain;
class Vertex;

class MoeVkRendererCommandBuffer {
public:
    MoeVkRendererCommandBuffer() { }

    void record(MoeVkLogicalDevice &device, MoeVkFramebuffer &framebuffer,
            MoeVkPipeline &pipeline, MoeVkSwapChain &swapChain,
            MoeVkCommandPool& commandPool,
            MoeVkArrayBuffer<Vertex> &vertexBuffer,
            MoeVkArrayBuffer<uint32_t>& indexBuffer,
            std::vector<MoeVkDrawable*>& drawables);

    VkCommandBuffer& buffer(unsigned int index) {
        return _buffer.at(index);
    }

private:
    MoeVkCommandBuffer _buffer;


};
}
