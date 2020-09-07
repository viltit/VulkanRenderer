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
class MoeVkRenderPass;
class Vertex;

class MoeVkRendererCommandBuffer {
public:
    MoeVkRendererCommandBuffer() { }

    void record(MoeVkLogicalDevice &device, MoeVkFramebuffer &framebuffer,
            const MoeVkRenderPass& renderPass,
            std::vector<MoeVkPipeline> &pipelines,
            MoeVkSwapChain &swapChain,
            MoeVkCommandPool& commandPool,
            std::vector<MoeVkDrawable*>& drawables,
            VkBool32 debugNormals);

    void destroy();

    VkCommandBuffer& buffer(unsigned int index) {
        return _buffer.at(index);
    }

private:
    MoeVkCommandBuffer _buffer;


};
}
