#pragma once

#include <string>
#include <optional>
#include <vulkan/vulkan.h>

#include "wrapper/MoeVkLogicalDevice.hpp"
#include "wrapper/MoeVkCommandPool.hpp"

#define STB_IMAGE_IMPLEMENTATION

namespace moe {
class Image {
public:
    Image();
    Image(const std::string& filename);

    ~Image();

    void load(const std::string& filename);
    void upload(MoeVkLogicalDevice& device, MoeVkPhysicalDevice& physicalDevice,
                 MoeVkCommandPool& commandPool, VkQueue& queue);
    void destroy();

    int w() const { return _w; }
    int h() const { return _h; }
    // int channels() const { return _channels; }
    int byteSize() const {
        return _w * _h * 4; // 4 because we always load as rgba
    }
    unsigned char* raw() const {
        return _pixels.value_or(nullptr);
    }

private:

    void setLayout(MoeVkCommandPool& commandPool, VkQueue& queue, VkImageLayout& layout);

    int _w;
    int _h;
    int _channels;   // warning: this returns the channels on disk, not what we loaded into _pixels
    std::optional<unsigned char*> _pixels;

    VkImage         _image;
    VkDeviceMemory  _memory;
    VkImageView     _view;
    VkImageLayout   _layout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    VkDevice        _device;
    VkSampler       _sampler;   // used if texel size is not equal to pixel size in the renderered scene
};
}
