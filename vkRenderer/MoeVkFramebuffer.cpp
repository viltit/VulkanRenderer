
#include <iostream>

#include "MoeVkFramebuffer.hpp"
#include "../Exceptions/InitException.hpp"

namespace moe {

MoeVkFramebuffer::MoeVkFramebuffer() { }

MoeVkFramebuffer::~MoeVkFramebuffer() { }

void MoeVkFramebuffer::create(MoeVkLogicalDevice& device, MoeVkSwapChain &swapChain,
                              const MoeVkPipeline &pipeline) {


    auto images = swapChain.images();
    _framebuffers.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        VkImageView attachment[] = { images[i] };
        VkFramebufferCreateInfo createInfo { };
        createInfo.sType            = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext            = nullptr;
        createInfo.flags            = 0;
        createInfo.renderPass       = pipeline.renderPass();
        createInfo.attachmentCount  = 1;
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
}

void MoeVkFramebuffer::destroy(MoeVkLogicalDevice& device) {
    std::cout << "Destroying Framebuffer" << std::endl;
    for (size_t i = 0; i < _framebuffers.size(); i++) {
        vkDestroyFramebuffer(device.device(), _framebuffers[i], nullptr);
    }
}
}