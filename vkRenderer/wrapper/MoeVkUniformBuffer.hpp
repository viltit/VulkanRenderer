#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkBuffer.hpp"

namespace moe {
class MoeVkUniformBuffer {
public:
    MoeVkUniformBuffer() { }
    void create(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device);
    void destroy(MoeVkLogicalDevice& device);

    VkDeviceMemory& memory() { return _buffer->memory(); }
    VkDescriptorSet& set() { return _set; }
    VkDescriptorSetLayout& layout() { return _layout; }
private:
    MoeVkBuffer*             _buffer;   // TODO: No pointer
    VkDescriptorSetLayout   _layout;
    VkDescriptorPool        _pool;
    VkDescriptorSet         _set;
};
}
