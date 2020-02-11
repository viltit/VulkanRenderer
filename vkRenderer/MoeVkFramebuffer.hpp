#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "MoeVkSwapChain.hpp"
#include "MoeVkPipeline.hpp"

namespace moe {

class MoeVkFramebuffer {
public:
    MoeVkFramebuffer();
    ~MoeVkFramebuffer();

    /// creates a Framebuffer from the SwapChain and the Rendering Pipeline
    void create(MoeVkLogicalDevice& device, MoeVkSwapChain& swapChain, const MoeVkPipeline& pipeline);
    void destroy(MoeVkLogicalDevice& device);

private:
    std::vector<VkFramebuffer> _framebuffers;
};
}