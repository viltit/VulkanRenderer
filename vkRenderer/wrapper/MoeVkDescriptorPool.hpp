#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkBuffer.hpp"

namespace moe {

class MoeTexture;

class MoeVkDescriptorPool {


public:
    MoeVkDescriptorPool() { }
    void createLayout(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device);
    void createPool(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device, size_t numBuffers);

    void destroy(MoeVkLogicalDevice& device);

    VkDescriptorSetLayout& layout() { return _layout; }
    VkDescriptorPool& pool() { return _pool; }

private:
    VkDescriptorSetLayout   _layout;
    VkDescriptorPool        _pool;
};
}
