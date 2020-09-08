#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkBuffer.hpp"

namespace moe {

class MoeTexture;

class MoeVkDescriptorPool {

public:
    MoeVkDescriptorPool(MoeVkPhysicalDevice& phyDevice, const  MoeVkLogicalDevice& device, size_t numBuffers);
    ~MoeVkDescriptorPool();

    VkDescriptorSetLayout& layout() { return _layout; }
    VkDescriptorPool& pool() { return _pool; }

private:
    void createLayout(MoeVkPhysicalDevice& phyDevice, const MoeVkLogicalDevice& device);
    void createPool(MoeVkPhysicalDevice& phyDevice, const MoeVkLogicalDevice& device, size_t numBuffers);

    const MoeVkLogicalDevice&     _device;
    VkDescriptorSetLayout   _layout;
    VkDescriptorPool        _pool;
};
}
