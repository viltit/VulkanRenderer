
#include "MoeVkDescriptorSet.hpp"

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkUtils.hpp"
#include "MoeTexture.hpp"
#include "MoeVkDescriptorPool.hpp"

namespace moe {

MoeVkDescriptorSet::MoeVkDescriptorSet(
        MoeVkPhysicalDevice &physicalDevice,
        MoeVkLogicalDevice &logicalDevice,
        MoeTexture& texture,
        MoeVkDescriptorPool& descriptorPool,
        size_t numSets)

        : _device { logicalDevice } {

    // create buffers:
    spdlog::trace("Creating {0} descriptor sets", numSets);
    _buffers.resize(numSets);
    _sets.resize(numSets);
    for (size_t i = 0; i < numSets; i++) {
        _buffers[i] = new MoeVkBuffer(physicalDevice, logicalDevice, VkDeviceSize(sizeof(UBO)),
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       "for uniform");
    }


    std::vector<VkDescriptorSetLayout> layouts(_buffers.size(), descriptorPool.layout());
    VkDescriptorSetAllocateInfo descriptorAllocateInfo{};
    descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocateInfo.pNext = nullptr;
    descriptorAllocateInfo.descriptorPool = descriptorPool.pool();
    descriptorAllocateInfo.descriptorSetCount = static_cast<uint32_t>(_buffers.size());
    descriptorAllocateInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(logicalDevice.device(), &descriptorAllocateInfo, _sets.data()) != VK_SUCCESS) {
        throw MoeInitError("Failed to allocate descriptor set.", __FILE__, __FUNCTION__, __LINE__);
    }

    update(texture);

}

void MoeVkDescriptorSet::update(MoeTexture& texture) {
    for (size_t i = 0; i < _sets.size(); i++) {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = _buffers[i]->buffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(UBO);  // TODO: Do not hardcode size here

        // TODO: Optional ?
        // TODO important: What if we want to load several images ?
        // TODO: What if we load images or vertices while the renderer is already running? Re-create ?
        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.sampler = texture.sampler();
        descriptorImageInfo.imageView = texture.view();
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        writeDescriptorSets.resize(2);
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].pNext = nullptr;
        writeDescriptorSets[0].dstSet = _sets[i];
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].dstArrayElement = 0;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].pImageInfo = nullptr;
        writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;
        writeDescriptorSets[0].pTexelBufferView = nullptr;
        writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[1].pNext = nullptr;
        writeDescriptorSets[1].dstSet = _sets[i];
        writeDescriptorSets[1].dstBinding = 1;
        writeDescriptorSets[1].dstArrayElement = 0;
        writeDescriptorSets[1].descriptorCount = 1;
        writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;
        writeDescriptorSets[1].pBufferInfo = nullptr;
        writeDescriptorSets[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_device.device(), writeDescriptorSets.size(),
                               writeDescriptorSets.data(), 0, nullptr);
    }

}

MoeVkDescriptorSet::~MoeVkDescriptorSet() {
    for (size_t i = 0; i < _buffers.size(); i++) {
        if (_buffers[i] != nullptr) {
            delete(_buffers[i]);
        }
    }
    _buffers.clear();
}
}