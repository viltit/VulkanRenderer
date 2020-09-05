#pragma once

#include <vector>
#include <string>

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkDescriptorPool.hpp"
#include "MoeVkShader.hpp"

namespace moe {

class MoeVkPipeline {
public:
    MoeVkPipeline() { }
    ~MoeVkPipeline() { };

    void prepare(std::vector<MoeVkShader>& shaders, uint32_t width, uint32_t height);
    void create(MoeVkLogicalDevice& device,
            MoeVkPhysicalDevice& physicalDevice,
            const MoeVkSwapChain& swapChain,
            MoeVkDescriptorPool& uniformBuffer);
    void destroy(MoeVkLogicalDevice& device);

    const VkRenderPass& renderPass() const { return _renderPass; }
    const VkPipeline& pipeline() const  { return _pipeline; }
    const VkPipelineLayout& layout() const { return _layout; }

private:
    void createRenderPass(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice, const MoeVkSwapChain& swapChain);

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo(bool opaque = true);

    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

    VkRenderPass        _renderPass;
    VkPipelineLayout    _layout;
    VkPipeline          _pipeline;
};
}
