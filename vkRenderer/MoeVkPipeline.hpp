#pragma once

#include <vector>
#include <string>

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkSwapChain.hpp"

namespace moe {

class MoeVkPipeline {
public:
    MoeVkPipeline();
    ~MoeVkPipeline();

    void create(MoeVkLogicalDevice& device, const MoeVkSwapChain& swapChain);
    void destroy(MoeVkLogicalDevice& device);

    const VkRenderPass& renderPass() const { return _renderPass; }

private:
    /// reads a compiled shader file (ie a .spv-file)
    std::vector<char> readShader(const std::string& filename);
    VkShaderModule createShader(MoeVkLogicalDevice& device, const std::vector<char>& bytecode);
    void createRenderPass(MoeVkLogicalDevice& device, const MoeVkSwapChain& swapChain);

    VkRenderPass        _renderPass;
    VkPipelineLayout    _layout;
    VkPipeline          _pipeline;
};
}
