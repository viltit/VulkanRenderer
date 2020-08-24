
#include "MoeDepthTexture.hpp"

namespace moe {

MoeDepthTexture::MoeDepthTexture()
    : _device { nullptr }
{ }

MoeDepthTexture::~MoeDepthTexture() {
    destroy();
};

void MoeDepthTexture::create(MoeVkLogicalDevice &device, MoeVkPhysicalDevice &physicalDevice,
                             MoeVkCommandPool &commandPool, VkQueue &queue, uint32_t w, uint32_t h) {

    if (_image.image() != VK_NULL_HANDLE) {
        throw InitException("Depth Image was already created", __FILE__, __FUNCTION__, __LINE__);
    }
    _device = &device;
    VkFormat depthFormat = physicalDevice.findDepthFormat();
    _image.create(device, physicalDevice, w, h, depthFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    _image.createView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    _image.changeLayout(commandPool, queue, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void MoeDepthTexture::destroy() {
    _image.destroy();
}


}