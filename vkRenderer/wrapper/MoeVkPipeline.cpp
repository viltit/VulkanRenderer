
#include "MoeVkPipeline.hpp"
#include "../MoeVertex.hpp"

#include "../../Exceptions/InitException.hpp"

#include <fstream>

namespace moe {

MoeVkPipeline::MoeVkPipeline() { }

MoeVkPipeline::~MoeVkPipeline() { }

void MoeVkPipeline::create(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
                           const MoeVkSwapChain& swapChain, MoeVkDescriptorPool& uniformBuffer) {

    // TODO later: Do not hardcode shader path
    auto vertexCode = readShader("Shaders/triangle.vert.spv");
    auto fragmentCode = readShader("Shaders/triangle.frag.spv");
    VkShaderModule vertexModule = createShader(device, vertexCode);
    VkShaderModule fragmentModule = createShader(device, fragmentCode);

    createRenderPass(device, physicalDevice, swapChain);

    VkPipelineShaderStageCreateInfo vertexCreateInfo { };
    vertexCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexCreateInfo.pNext          = nullptr;
    vertexCreateInfo.flags          = 0;
    vertexCreateInfo.stage          = VK_SHADER_STAGE_VERTEX_BIT;
    vertexCreateInfo.module         = vertexModule;
    // This would allow for multiple shaders in one module:
    vertexCreateInfo.pName          = "main";   // TODO: Let the calling code decide the name of the entry function
    vertexCreateInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo fragmentCreateInfo { };
    fragmentCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentCreateInfo.pNext          = nullptr;
    fragmentCreateInfo.flags          = 0;
    fragmentCreateInfo.stage          = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentCreateInfo.module         = fragmentModule;
    fragmentCreateInfo.pName          = "main";   // TODO: Let the calling code decide the name of the entry function
    fragmentCreateInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexCreateInfo, fragmentCreateInfo };

    /* Pipeline stages ---------------------------------------------------- */
    // TODO: How can we be more flexie in the vertex binding description, ie. drawing shapes with colors and shapes
    // with textures at the same time
    auto bindingDesciption = Vertex::getBindingDescription();
    auto attributeDescription = Vertex::getAttributeDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo { };
    vertexInputCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.pNext         = nullptr;
    vertexInputCreateInfo.flags         = 0;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDesciption;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t >(attributeDescription.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
    inputAssemblyCreateInfo.sType       = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.pNext       = nullptr;
    inputAssemblyCreateInfo.flags       = 0;
    // here we could change the geometries to triangle_fan, triangle_strip and others
    inputAssemblyCreateInfo.topology    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable  = VK_FALSE;

    // viewport describes the region of the SwapChain Framebuffer that will be rendered on screen
    VkViewport viewport { };
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)swapChain.extent().width;
    viewport.height = (float)swapChain.extent().height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    // the scissor can cut parts of the SwapChain image before presenting it on screen
    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain.extent();

    VkPipelineViewportStateCreateInfo viewportCreateInfo { };
    viewportCreateInfo.sType        = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCreateInfo.pNext        = nullptr;
    viewportCreateInfo.flags        = 0;
    viewportCreateInfo.viewportCount = 1;
    viewportCreateInfo.pViewports   = &viewport;
    viewportCreateInfo.scissorCount  = 1;
    viewportCreateInfo.pScissors    = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo { };
    rasterizationCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.pNext                   = nullptr;
    rasterizationCreateInfo.flags                   = 0;
    // if enabled, fragments with a depth beyond { 0, 1 } are clamped and not discarded -> useful for shadow mapping
    rasterizationCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    // use LINE for wireframes
    rasterizationCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0;
    rasterizationCreateInfo.depthBiasClamp          = VK_FALSE;
    rasterizationCreateInfo.depthBiasSlopeFactor    = 0;
    rasterizationCreateInfo.lineWidth               = 1.f;

    // TODO: Add multisampling at a later stage
    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = { };
    multisampleCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.pNext                     = nullptr;
    multisampleCreateInfo.flags                     = 0;
    multisampleCreateInfo.rasterizationSamples      = VK_SAMPLE_COUNT_1_BIT;
    multisampleCreateInfo.sampleShadingEnable       = VK_FALSE;
    multisampleCreateInfo.minSampleShading          = 1.f;
    multisampleCreateInfo.pSampleMask               = nullptr;
    multisampleCreateInfo.alphaToCoverageEnable     = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable          = VK_FALSE;

    // TODO later: VkPipelineDepthStencilStateCreateInfo   -> we do not use one yet

    // color blending on a per-framebuffer basis:
    VkPipelineColorBlendAttachmentState colorBlendAttachment { };
    colorBlendAttachment.blendEnable            = VK_FALSE; // TODO: Revisit later
    colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    // fetch create info for depth buffer:
    VkPipelineDepthStencilStateCreateInfo dephtStencilCreateInfo = depthStencilStateCreateInfo();

    // color blending on a global basis, ie applied to ALL framebuffers:
    VkPipelineColorBlendStateCreateInfo blendingCreateInfo { };
    blendingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendingCreateInfo.pNext                = nullptr;
    blendingCreateInfo.flags                = 0;
    blendingCreateInfo.logicOpEnable        = VK_FALSE;
    blendingCreateInfo.logicOp              = VK_LOGIC_OP_COPY;
    blendingCreateInfo.attachmentCount      = 1;
    blendingCreateInfo.pAttachments         = &colorBlendAttachment;
    blendingCreateInfo.blendConstants[0]    = 0.f;
    blendingCreateInfo.blendConstants[1]    = 0.f;
    blendingCreateInfo.blendConstants[2]    = 0.f;
    blendingCreateInfo.blendConstants[3]    = 0.f;

    // TODO later: VkDynamicState to avoid recrating the pipeline because, for example, we want a line width of 2

    // Define dynamic states so that we do not need to re-recreate the pipeline from scratch after a window resize
    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.flags = 0;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    // TODO later: We have to define the uniforms our shader is using here
    VkPipelineLayoutCreateInfo layoutCreateInfo { };
    layoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext                  = nullptr;
    layoutCreateInfo.flags                  = 0;
    layoutCreateInfo.setLayoutCount         = 1;
    layoutCreateInfo.pSetLayouts            = &(uniformBuffer.layout());
    layoutCreateInfo.pushConstantRangeCount = 0;
    layoutCreateInfo.pPushConstantRanges    = nullptr;

    if (vkCreatePipelineLayout(device.device(), &layoutCreateInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw InitException("Failed to create pipeline layout", __FILE__, __FUNCTION__, __LINE__);
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
    pipelineCreateInfo.pStages              = shaderStages;
    pipelineCreateInfo.pVertexInputState    = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState  = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pTessellationState   = nullptr;
    pipelineCreateInfo.pViewportState       = &viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState  = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState    = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState   = &dephtStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState     = &blendingCreateInfo;
    pipelineCreateInfo.pDynamicState        = &dynamicStateCreateInfo;   // allows to change viewport when recording command buffers
    pipelineCreateInfo.layout               = _layout;
    pipelineCreateInfo.renderPass           = _renderPass;
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

        throw InitException("Failed to create graphics pipeline", __FILE__, __FUNCTION__, __LINE__);
    }

    // TODO: These should be destroyed even if we get an Exception above !!
    vkDestroyShaderModule(device.device(), vertexModule, nullptr);
    vkDestroyShaderModule(device.device(), fragmentModule, nullptr);
}

void MoeVkPipeline::destroy(MoeVkLogicalDevice& device) {

    vkDestroyPipelineLayout(device.device(), _layout, nullptr);
    vkDestroyRenderPass(device.device(), _renderPass, nullptr);
    vkDestroyPipeline(device.device(), _pipeline, nullptr);
}

std::vector<char> MoeVkPipeline::readShader(const std::string& filename) {

    // seek to the end of file when opening and read in binary mode:
    std::ifstream instream { filename, std::ios::ate | std::ios::binary };
    if (!instream) {
        throw InitException("Could not find file " + filename, __FILE__, __FUNCTION__, __LINE__);
    }
    if (!instream.is_open()) {
        throw InitException("Could not open file " + filename, __FILE__, __FUNCTION__, __LINE__);
    }

    // because we opened the file at its end, we can directly get the files size:
    size_t filesize = (size_t)instream.tellg();
    std::vector<char> buffer(filesize);

    // put the reader back at the files beginning:
    instream.seekg(0);
    instream.read(buffer.data(), filesize);
    instream.close();

    return buffer;
}

VkShaderModule MoeVkPipeline::createShader(MoeVkLogicalDevice& device, const std::vector<char> &bytecode) {

    VkShaderModuleCreateInfo createInfo { };
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext    = nullptr;
    createInfo.flags    = 0;
    createInfo.codeSize = bytecode.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(bytecode.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &module) != VK_SUCCESS) {
        throw InitException("Failed to create a shader module.", __FILE__, __FUNCTION__, __LINE__);
    }

    return module;
}


void MoeVkPipeline::createRenderPass(MoeVkLogicalDevice &device, MoeVkPhysicalDevice& physicalDevice, const MoeVkSwapChain& swapChain) {

    // specific for rendering: add a subpass dependency for synchronization. "External" refers to the implicit
    // subpass before and after the rendering. Index 0 refers to our rendering pass, which is the only one
    VkSubpassDependency dependency { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    // the operations to wait on:
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    // the operations that should wait:
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription colorAttachment { };
    colorAttachment.format              = swapChain.format().format;
    colorAttachment.samples             = VK_SAMPLE_COUNT_1_BIT;    // TODO later: no multisampling yet -> stick to 1 sample
    colorAttachment.loadOp              = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp             = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp       = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp      = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout       = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout         = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // fetch depth attachment from physical device
    VkAttachmentDescription depthAttachment = physicalDevice.getDepthAttachment();

    // We could define several rendering subpasses, for example for postprocessing. For the fist triangle, however,
    // one subpass is enough
    VkAttachmentReference colorAttachmentRef { };
    // because we only have one colorAttachment, its index will be 0.
    // Important: The index directly references the fragments shader layout(location=0) out vec4 color index !!
    colorAttachmentRef.attachment       = 0;
    colorAttachmentRef.layout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef { };
    depthAttachmentRef.attachment       = 1;
    depthAttachmentRef.layout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass { };
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkAttachmentDescription attachmentDescriptions[] = {
            colorAttachment,
            depthAttachment
    };
    VkRenderPassCreateInfo renderPassCreateInfo { };
    renderPassCreateInfo.sType                = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext                = nullptr;
    renderPassCreateInfo.flags                = 0;
    renderPassCreateInfo.attachmentCount      = 2;
    renderPassCreateInfo.pAttachments         = attachmentDescriptions;
    renderPassCreateInfo.subpassCount         = 1;
    renderPassCreateInfo.pSubpasses           = &subpass;
    renderPassCreateInfo.dependencyCount      = 1;
    renderPassCreateInfo.pDependencies        = &dependency;

    if (vkCreateRenderPass(device.device(), &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        throw InitException("Failed to create Render Pass", __FILE__, __FUNCTION__, __LINE__);
    }
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