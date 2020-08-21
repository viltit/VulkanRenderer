#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkBuffer.hpp"

namespace moe {

class MoeTexture;

class MoeVkUniformBuffer {


public:
    MoeVkUniformBuffer() { }
    void createLayout(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device);
    void createPool(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device, size_t numBuffers);
    void updateSets(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice& device, MoeTexture& image);

    void destroy(MoeVkLogicalDevice& device);

    VkDeviceMemory& memory(int i) { return _buffers[i]->memory(); }
    VkDescriptorSet& set(int i) { return _sets[i]; }
    VkDescriptorSetLayout& layout() { return _layout; }
private:
    std::vector<MoeVkBuffer*>  _buffers;   // TODO: No pointer
    VkDescriptorSetLayout   _layout;
    VkDescriptorPool        _pool;
    std::vector<VkDescriptorSet>         _sets;
};
}
