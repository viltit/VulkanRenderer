//
// Created by titus on 14.08.20.
//

#include "MoeVkUniformBuffer.hpp"
#include "../Exceptions/InitException.hpp"
#include <glm/glm.hpp>

namespace moe {
void MoeVkUniformBuffer::create(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice &device) {

    /**
     * Vulkan differentiates between descriptors and descriptor sets. Each descriptor is part of a set.
     */

    // TODO: Do not hardcode descitor count and stage flag
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = 0;    // WARNING: Must be identical with the shader binding number
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;  // TODO: Use Model matrix on a per drawable basis
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = 0;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;

    if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw InitException("Failed to create descriptor set layout", __FILE__, __FUNCTION__, __LINE__);
    }

    // TODO: Do not hardcode size here
    _buffer = new MoeVkBuffer(phyDevice, device, VkDeviceSize(sizeof(glm::mat4)),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDescriptorPoolSize poolSize { };
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo { };
    poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext          = nullptr;
    poolInfo.flags          = 0;
    poolInfo.maxSets        = 1;
    poolInfo.poolSizeCount  = 1;
    poolInfo.pPoolSizes     = &poolSize;

    if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw InitException("Failed to create descriptor pool.", __FILE__, __FUNCTION__, __LINE__);
    }

    VkDescriptorSetAllocateInfo descriptorAllocateInfo { };
    descriptorAllocateInfo.sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocateInfo.pNext            = nullptr;
    descriptorAllocateInfo.descriptorPool   = _pool;
    descriptorAllocateInfo.descriptorSetCount = 1;
    descriptorAllocateInfo.pSetLayouts      = &_layout;

    if (vkAllocateDescriptorSets(device.device(), &descriptorAllocateInfo, &_set) != VK_SUCCESS) {
        throw InitException("Failed to allocate descriptor set.", __FILE__, __FUNCTION__, __LINE__);
    }

    VkDescriptorBufferInfo descriptorBufferInfo { };
    descriptorBufferInfo.buffer = _buffer->buffer();
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range  = sizeof(glm::mat4);  // TODO: Do not hardcode size here

    VkWriteDescriptorSet writeDescriptorSet { };
    writeDescriptorSet.sType        = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext        = nullptr;
    writeDescriptorSet.dstSet       = _set;
    writeDescriptorSet.dstBinding   = 0;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pImageInfo    = nullptr;
    writeDescriptorSet.pBufferInfo   = &descriptorBufferInfo;
    writeDescriptorSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device.device(), 1, &writeDescriptorSet, 0, nullptr);
}

void MoeVkUniformBuffer::destroy(MoeVkLogicalDevice &device) {
    vkDestroyDescriptorSetLayout(device.device(), _layout, nullptr);
    vkDestroyDescriptorPool(device.device(), _pool, nullptr);
    if (_buffer != nullptr) {
        delete _buffer;
    }
}
}