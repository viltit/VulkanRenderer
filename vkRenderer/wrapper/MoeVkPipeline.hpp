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
    MoeVkPipeline()
        :   _device     { nullptr },
            _pipeline   { VK_NULL_HANDLE },
            _layout     { VK_NULL_HANDLE },
            _renderPass { VK_NULL_HANDLE }
    { }
    ~MoeVkPipeline() { };

    void prepare(std::vector<MoeVkShader>& shaders, uint32_t width, uint32_t height);
    void create(MoeVkLogicalDevice& device,
            MoeVkPhysicalDevice& physicalDevice,
            const MoeVkSwapChain& swapChain,
            MoeVkDescriptorPool& uniformBuffer);
    void destroy();

    const VkRenderPass& renderPass() const { return _renderPass; }
    const VkPipeline& pipeline() const  { return _pipeline; }
    const VkPipelineLayout& layout() const { return _layout; }

private:
    void createRenderPass(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice, const MoeVkSwapChain& swapChain);

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo(bool opaque = true);

    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

    // helper structs for create infos
    VkPushConstantRange _pushConstantRange;
    VkVertexInputBindingDescription _bindingDesciption;
    std::vector<VkVertexInputAttributeDescription> _attributeDescription;

    // vulkan create infos
    VkPipelineVertexInputStateCreateInfo    _vertexInputCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo  _inputAssemblyCreateInfo;
    VkPipelineViewportStateCreateInfo       _viewportCreateInfo;
    VkPipelineRasterizationStateCreateInfo  _rasterizationCreateInfo;
    VkPipelineMultisampleStateCreateInfo    _multisampleCreateInfo;
    VkPipelineColorBlendAttachmentState     _colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo     _blendingCreateInfo;
    VkPipelineDynamicStateCreateInfo        _dynamicStateCreateInfo;
    VkPipelineDepthStencilStateCreateInfo   _dephtStencilCreateInfo;

    MoeVkLogicalDevice* _device;
    VkRenderPass        _renderPass;
    VkPipelineLayout    _layout;
    VkPipeline          _pipeline;
};
}
