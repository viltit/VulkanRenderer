
#include "MoeVkRenderPass.hpp"

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkSwapChain.hpp"

#include "MoeExceptions.hpp"

namespace moe {

MoeVkRenderPass::MoeVkRenderPass(const MoeVkLogicalDevice &device,
                                         const MoeVkPhysicalDevice& physicalDevice,
                                         const MoeVkSwapChain& swapChain)
    : _device { device }
{

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
        throw MoeInitError("Failed to create Render Pass", __FILE__, __FUNCTION__, __LINE__);
    }
}

MoeVkRenderPass::~MoeVkRenderPass() {
    vkDestroyRenderPass(_device.device(), _renderPass, nullptr);
}
}