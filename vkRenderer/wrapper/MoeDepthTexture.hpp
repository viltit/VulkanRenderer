#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkImage.hpp"

namespace moe {
class MoeDepthTexture {
public:
    MoeDepthTexture();
    ~MoeDepthTexture();

    void create(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
            MoeVkCommandPool& commandPool, VkQueue& queue, uint32_t w, uint32_t h);
    void destroy();

    VkImageView& imageView() { return _image.view(); }

private:
    MoeVkImage          _image;
    MoeVkLogicalDevice* _device;
};
}
