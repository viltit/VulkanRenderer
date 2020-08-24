
#include <iostream>
#include <spdlog/spdlog.h>

#include "MoeVkFramebuffer.hpp"
#include "../../Exceptions/InitException.hpp"

namespace moe {

MoeVkFramebuffer::MoeVkFramebuffer() { }

MoeVkFramebuffer::~MoeVkFramebuffer() { }

void MoeVkFramebuffer::create(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
        MoeVkSwapChain &swapChain, const MoeVkPipeline &pipeline, MoeVkCommandPool& commandPool) {

    spdlog::trace("Creating MoeVkFramebuffer");

    auto extent = swapChain.extent();
    // TODO: Maybe put dephtImage sowhere else
    _depthTexture.create(device, physicalDevice, commandPool, device.graphicsQueue(), extent.width, extent.height);
    auto images = swapChain.images();
    _framebuffers.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {

        VkImageView attachment[] = { images[i], _depthTexture.imageView() };
        VkFramebufferCreateInfo createInfo { };
        createInfo.sType            = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext            = nullptr;
        createInfo.flags            = 0;
        createInfo.renderPass       = pipeline.renderPass();
        createInfo.attachmentCount  = 2;
        createInfo.pAttachments     = attachment;
        createInfo.width            = swapChain.extent().width;
        createInfo.height           = swapChain.extent().height;
        createInfo.layers           = 1;

        if (vkCreateFramebuffer(device.device(), &createInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS) {
            throw InitException("Could not initialize Framebuffer", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

void MoeVkFramebuffer::destroyBuffers(MoeVkLogicalDevice& device) {
    for (size_t i = 0; i < _framebuffers.size(); i++) {
        vkDestroyFramebuffer(device.device(), _framebuffers[i], nullptr);
    }
    _depthTexture.destroy();
}

void MoeVkFramebuffer::destroy(MoeVkLogicalDevice& device) {
    spdlog::trace("Creating MoeVkFramebuffer");
    for (size_t i = 0; i < _framebuffers.size(); i++) {
        vkDestroyFramebuffer(device.device(), _framebuffers[i], nullptr);
    }
    _depthTexture.destroy();
}
}