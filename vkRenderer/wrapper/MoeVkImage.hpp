#pragma once

#include <string>
#include <optional>
#include <vulkan/vulkan.h>

#include "MoeVkLogicalDevice.hpp"
#include "MoeVkCommandPool.hpp"

#define STB_IMAGE_IMPLEMENTATION

namespace moe {
class MoeVkImage {
public:
    MoeVkImage();
    MoeVkImage(const std::string& filename);

    MoeVkImage(const MoeVkImage& other)   = delete;
    MoeVkImage(MoeVkImage&& other)        = delete;
    MoeVkImage& operator = (const MoeVkImage& image)  = delete;
    MoeVkImage& operator = (MoeVkImage&& image)       = delete;

    ~MoeVkImage();

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

    VkSampler& sampler() {
        assert(_sampler != VK_NULL_HANDLE);
        return _sampler;
    }
    VkImageView& view() {
        assert(_view != VK_NULL_HANDLE);
        return _view;
    }



private:
    void setLayout(MoeVkCommandPool& commandPool, VkQueue& queue, VkImageLayout layout);
    void transferBuffer(MoeVkLogicalDevice& device, MoeVkCommandPool& pool, VkQueue& queue, MoeVkBuffer& buffer);


    int _w;
    int _h;
    int _channels;   // warning: this returns the channels on disk, not what we loaded into _pixels
    std::optional<unsigned char*> _pixels;

    // with a reference, we need to initialize the device on creation
    MoeVkLogicalDevice* _device;

    VkImage         _image;
    VkDeviceMemory  _memory;
    VkImageView     _view;
    VkImageLayout   _layout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    VkSampler       _sampler;   // used if texel size is not equal to pixel size in the renderered scene
};
}
