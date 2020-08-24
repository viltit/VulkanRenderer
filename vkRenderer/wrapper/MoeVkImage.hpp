#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkImage.hpp"
#include "MoeVkCommandPool.hpp"

namespace moe {

class MoeVkLogicalDevice;
class MoeVkPhysicalDevice;

class MoeVkImage {
public:
    MoeVkImage();
    ~MoeVkImage();
    void create(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice, uint32_t w, uint32_t height,
            VkFormat format, VkImageTiling tiling,
            VkImageLayout layout,
            VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);
    void createView(VkFormat format, VkImageAspectFlags aspectFlags);
    void changeLayout(MoeVkCommandPool& commandPool, VkQueue queue, VkFormat format, VkImageLayout newLayout);
    void destroy();

    VkImage& image()            { return _image; }
    VkImageView& view()         { return _view; }
    VkDeviceMemory& memory()    { return _memory; }

private:
    MoeVkLogicalDevice* _device;
    VkImage _image;
    VkImageView _view;
    VkDeviceMemory _memory;
    VkImageLayout _layout;
};
}
