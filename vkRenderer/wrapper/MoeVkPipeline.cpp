
#include "MoeVkPipeline.hpp"
#include "../MoeVertex.hpp"

#include "MoeExceptions.hpp"
#include "MoeVkRenderPass.hpp"

#include <spdlog/spdlog.h>

namespace moe {

void MoeVkPipeline::prepare(std::vector<MoeVkShader>& shaders, uint32_t width, uint32_t height) {

    if (_isCreated) {
        spdlog::warn("Calling pipeline.prepare after is has already been created has not effect.");
    }

    _shaderStages.resize(shaders.size());
    // TODO: We could check if user gave two identical shader stages
    for (size_t i = 0; i < shaders.size(); i++) {
        _shaderStages[i] = VkPipelineShaderStageCreateInfo {
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                static_cast<VkShaderStageFlagBits>(shaders[i].stage()),
                shaders[i].module(),
                "main",
                nullptr
        };
    }

    /* Pipeline stages: Vertex input ---------------------------------------------------- */
    _bindingDesciption = Vertex::getBindingDescription();
    _attributeDescription = Vertex::getAttributeDescription();
    _vertexInputCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    _vertexInputCreateInfo.pNext         = nullptr;
    _vertexInputCreateInfo.flags         = 0;
    _vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    _vertexInputCreateInfo.pVertexBindingDescriptions = &_bindingDesciption;
    _vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t >(_attributeDescription.size());
    _vertexInputCreateInfo.pVertexAttributeDescriptions = _attributeDescription.data();

    /* Pipeline stages: Assembly input ---------------------------------------------------- */
    _inputAssemblyCreateInfo.sType       = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _inputAssemblyCreateInfo.pNext       = nullptr;
    _inputAssemblyCreateInfo.flags       = 0;
    // here we could change the geometries to triangle_fan, triangle_strip and others
    _inputAssemblyCreateInfo.topology    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    _inputAssemblyCreateInfo.primitiveRestartEnable  = VK_FALSE;

    /* Pipeline stages: Viewport --------------------------------------------------------- */
    // viewport describes the region of the SwapChain Framebuffer that will be rendered on screen
    VkViewport viewport { };
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    // the scissor can cut parts of the SwapChain image before presenting it on screen
    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { width, height };
    _viewportCreateInfo.sType        = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _viewportCreateInfo.pNext        = nullptr;
    _viewportCreateInfo.flags        = 0;
    _viewportCreateInfo.viewportCount = 1;
    _viewportCreateInfo.pViewports   = &viewport;
    _viewportCreateInfo.scissorCount  = 1;
    _viewportCreateInfo.pScissors    = &scissor;


    _rasterizationCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _rasterizationCreateInfo.pNext                   = nullptr;
    _rasterizationCreateInfo.flags                   = 0;
    // if enabled, fragments with a depth beyond { 0, 1 } are clamped and not discarded -> useful for shadow mapping
    _rasterizationCreateInfo.depthClampEnable        = VK_FALSE;
    _rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    // use LINE for wireframes
    _rasterizationCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    _rasterizationCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    _rasterizationCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterizationCreateInfo.depthBiasEnable         = VK_FALSE;
    _rasterizationCreateInfo.depthBiasConstantFactor = 0;
    _rasterizationCreateInfo.depthBiasClamp          = VK_FALSE;
    _rasterizationCreateInfo.depthBiasSlopeFactor    = 0;
    _rasterizationCreateInfo.lineWidth               = 1.f;

    // TODO: Add multisampling at a later stage
    _multisampleCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _multisampleCreateInfo.pNext                     = nullptr;
    _multisampleCreateInfo.flags                     = 0;
    _multisampleCreateInfo.rasterizationSamples      = VK_SAMPLE_COUNT_1_BIT;
    _multisampleCreateInfo.sampleShadingEnable       = VK_FALSE;
    _multisampleCreateInfo.minSampleShading          = 1.f;
    _multisampleCreateInfo.pSampleMask               = nullptr;
    _multisampleCreateInfo.alphaToCoverageEnable     = VK_FALSE;
    _multisampleCreateInfo.alphaToOneEnable          = VK_FALSE;

    // TODO later: VkPipelineDepthStencilStateCreateInfo   -> we do not use one yet

    // color blending on a per-framebuffer basis:
    _colorBlendAttachment.blendEnable            = VK_FALSE; // TODO: Revisit later
    _colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD;
    _colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD;
    _colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    // fetch create info for depth buffer:
    _dephtStencilCreateInfo = depthStencilStateCreateInfo();

    // color blending on a global basis, ie applied to ALL framebuffers:
    _blendingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _blendingCreateInfo.pNext                = nullptr;
    _blendingCreateInfo.flags                = 0;
    _blendingCreateInfo.logicOpEnable        = VK_FALSE;
    _blendingCreateInfo.logicOp              = VK_LOGIC_OP_COPY;
    _blendingCreateInfo.attachmentCount      = 1;
    _blendingCreateInfo.pAttachments         = &_colorBlendAttachment;
    _blendingCreateInfo.blendConstants[0]    = 0.f;
    _blendingCreateInfo.blendConstants[1]    = 0.f;
    _blendingCreateInfo.blendConstants[2]    = 0.f;
    _blendingCreateInfo.blendConstants[3]    = 0.f;

    // Define dynamic states so that we do not need to re-recreate the pipeline from scratch after a window resize
    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };
    _dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _dynamicStateCreateInfo.flags = 0;
    _dynamicStateCreateInfo.pNext = nullptr;
    _dynamicStateCreateInfo.dynamicStateCount = 2;
    _dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    // Push constants are much faster than uniforms on most GPUs, but they are guaranteed to NOT be slower than uniforms
    // However, they have a very limited memory size (guaranteed minimum: 128 Bytes)
    // TODO: If we push more than 128 bytes, we have to check the device limits
    _pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    _pushConstantRange.offset = 0;
    _pushConstantRange.size = sizeof(VkBool32);

    _isPrepared = true;
}

void MoeVkPipeline::create(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
                           const MoeVkSwapChain& swapChain,
                           const MoeVkRenderPass& renderPass,
                           MoeVkDescriptorPool& uniformBuffer) {

    _device = &device;

    if (!_isPrepared) {
        throw MoeInitError("Pipeline must be prepared before it can be created.", __FILE__, __FUNCTION__, __LINE__);
    }

    VkPipelineLayoutCreateInfo layoutCreateInfo { };
    layoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext                  = nullptr;
    layoutCreateInfo.flags                  = 0;
    layoutCreateInfo.setLayoutCount         = 1;
    layoutCreateInfo.pSetLayouts            = &(uniformBuffer.layout());
    layoutCreateInfo.pushConstantRangeCount = 1;
    layoutCreateInfo.pPushConstantRanges    = &_pushConstantRange;

    if (vkCreatePipelineLayout(device.device(), &layoutCreateInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw MoeInitError("Failed to create pipeline layout", __FILE__, __FUNCTION__, __LINE__);
    }

    /**
     * And FINALLY, we have all the objects that define the rendering pipeline:
     *      - Shader stages: They represent the programmable parts of the pipeline
     *      - Fixed function stages: Input assembly, rasterization etc.
     *      - Pipeline layout: Uniform and push values that can be used by the shader
     *      - Render pass: The attachments referenced by the pipeline stages and their usage
     */
    VkGraphicsPipelineCreateInfo pipelineCreateInfo { };
    pipelineCreateInfo.sType                = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext                = nullptr;
    pipelineCreateInfo.flags                = 0;
    pipelineCreateInfo.stageCount           = 2;    // vertex and fragment shader
    pipelineCreateInfo.pStages              = _shaderStages.data();
    pipelineCreateInfo.pVertexInputState    = &_vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState  = &_inputAssemblyCreateInfo;
    pipelineCreateInfo.pTessellationState   = nullptr;
    pipelineCreateInfo.pViewportState       = &_viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState  = &_rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState    = &_multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState   = &_dephtStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState     = &_blendingCreateInfo;
    pipelineCreateInfo.pDynamicState        = &_dynamicStateCreateInfo;   // allows to change viewport when recording command buffers
    pipelineCreateInfo.layout               = _layout;
    pipelineCreateInfo.renderPass           = renderPass.renderPass();
    pipelineCreateInfo.subpass              = 0;
    // When using multiple pipelines, we could define a basePipeline with common features
    // This is better for performance than using multiple completly different pipelines !!
    // We would also have to set the VK_PIPELINE_CREATE_DERIVATIVE_BIT in flags
    pipelineCreateInfo.basePipelineHandle   = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex    = -1;

    // TODO: Look into the PipelineCache once using multiple pipelines
    if (vkCreateGraphicsPipelines(
            device.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            nullptr,
            &_pipeline) != VK_SUCCESS) {

        throw MoeInitError("Failed to create graphics pipeline", __FILE__, __FUNCTION__, __LINE__);
    }
    _isCreated = true;
}

void MoeVkPipeline::destroy() {
    if (_isCreated && _device != nullptr) {
        vkDestroyPipelineLayout(_device->device(), _layout, nullptr);
        vkDestroyPipeline(_device->device(), _pipeline, nullptr);
    }
    _device = nullptr;
    _isCreated = false;
}

VkPipelineDepthStencilStateCreateInfo MoeVkPipeline::depthStencilStateCreateInfo(bool opaque) {
    VkPipelineDepthStencilStateCreateInfo result { };
    result.sType                = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    result.pNext                = nullptr;
    result.flags                = 0;
    result.depthTestEnable      = VK_TRUE;
    result.depthWriteEnable     = opaque ? VK_TRUE : VK_FALSE;
    result.depthCompareOp       = VK_COMPARE_OP_LESS;
    result.depthBoundsTestEnable = VK_FALSE;    // we could clip rendering into a range of the buffer
    result.stencilTestEnable    = VK_FALSE;
    result.front                = { };
    result.back                 = { };
    result.minDepthBounds = 0.0f;
    result.maxDepthBounds = 1.f;

    return result;
}

}