#pragma once

#include <vector>
#include <string>

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkDescriptorPool.hpp"
#include "MoeVkShader.hpp"

namespace moe {

// TODO: Do we want to expose Vulkan here or wrap everything around our own enums ?
/* enum class MoeVkPolygonMode {

}; */

class MoeVkRenderPass;

class MoeVkPipeline {
public:
    MoeVkPipeline()
        :   _device     { nullptr },
            _pipeline   { VK_NULL_HANDLE },
            _layout     { VK_NULL_HANDLE },
            _isCreated  { false },
            _isPrepared { false }
    { }
    ~MoeVkPipeline() { };

    // only allow move assignment (for vector.push_back(MoePipeline(...)))
    MoeVkPipeline(const MoeVkPipeline&)                 = delete;
    MoeVkPipeline(MoeVkPipeline&& other)                = default;
    MoeVkPipeline& operator = (const MoeVkPipeline&)    = delete;
    MoeVkPipeline& operator = (MoeVkPipeline&&)         = delete;


    void prepare(std::vector<MoeVkShader>& shaders,
            uint32_t width, uint32_t height,
            uint32_t offsetX = 0,
            uint32_t offsetY = 0);
    void create(MoeVkLogicalDevice& device,
            MoeVkPhysicalDevice& physicalDevice,
            const MoeVkSwapChain& swapChain,
            const MoeVkRenderPass& renderPass,
            MoeVkDescriptorPool& uniformBuffer);
    void destroy();

    const VkPipeline&           pipeline() const    { return _pipeline; }
    const VkPipelineLayout&     layout() const      { return _layout; }
    const VkViewport&           viewport() const    { return _renderingViewport; }

    void setRenderingViewport(uint32_t w, uint32_t h,
            uint32_t offsetX = 0, uint32_t offsetY = 0) {

        _renderingViewport.width = w;
        _renderingViewport.height = h;
        _renderingViewport.x = offsetX;
        _renderingViewport.y = offsetY;
    }

    // for now, we allow direct access to the most important create infos so the user can modify them before calling
    // "create"
    VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo() {
        return _rasterizationCreateInfo;
    }

private:

    bool _isPrepared;
    bool _isCreated;

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

    // viewport this pipeline draws into
    VkViewport                              _viewport;

    // this viewport will be given to the rendering command buffer and can be changed without re-creating the
    // pipeline
    VkViewport                              _renderingViewport;

    MoeVkLogicalDevice* _device;
    VkPipelineLayout    _layout;
    VkPipeline          _pipeline;
};
}
