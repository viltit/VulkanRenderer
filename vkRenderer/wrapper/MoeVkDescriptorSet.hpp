#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <assert.h>

#include "MoeVkBuffer.hpp"

namespace moe {

class MoeVkPhysicalDevice;
class MoeVkLogicalDevice;
class MoeTexture;
class MoeVkDescriptorPool;

class MoeVkDescriptorSet {
public:
    MoeVkDescriptorSet(MoeVkPhysicalDevice& physicalDevice,
            MoeVkLogicalDevice& logicalDevice,
            MoeTexture& texture,   // TODO: Give in a drawable
            MoeVkDescriptorPool& descriptorPool,
            size_t numSets);
    ~MoeVkDescriptorSet();

    void update(MoeTexture& texture);

    size_t numSets() const {
        return _sets.size();
    }

    VkDeviceMemory& memory(int i) {
        assert(i < _buffers.size());
        return _buffers[i]->memory();
    }

    VkDescriptorSet& set(int i) {
        assert(i < _sets.size());
        return _sets[i];
    }

private:
    std::vector<MoeVkBuffer*>        _buffers;    // TODO: No pointer, need copy assignement
    std::vector<VkDescriptorSet>    _sets;

    MoeVkLogicalDevice&             _device;
};
}
