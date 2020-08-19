//
// Created by titus on 14.08.20.
//

#include "MoeVkUniformBuffer.hpp"
#include "MoeVkImage.hpp"
#include "../../Exceptions/InitException.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "MoeVkUtils.hpp"
#include <assert.h>

namespace moe {
void MoeVkUniformBuffer::createLayout(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice &device) {

    /**
     * A descriptor layout describes the details about the binding. This information is needed during pipeline
     * creation.
     * Vulkan differentiates between descriptors and descriptor sets. Each descriptor is part of a set.
     */

    // TODO: Do not hardcode descritor count and stage flag
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
            {
                    VkDescriptorSetLayoutBinding{
                            0,    // WARNING: Must be identical with the shader binding number
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            1,  // TODO: Use Model matrix on a per drawable basis
                            VK_SHADER_STAGE_VERTEX_BIT,
                            nullptr
                    }
            },
            {
                    // TODO longterm: optional for colored or textured shapes
                    VkDescriptorSetLayoutBinding{
                            1,    // WARNING: Must be identical with the shader binding number
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            1,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            nullptr
                    }
            }
    };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = 0;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw InitException("Failed to create descriptor set layout", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkUniformBuffer::createPool(moe::MoeVkPhysicalDevice &phyDevice, moe::MoeVkLogicalDevice &device, size_t numBuffers) {

    // TODO: Do not hardcode size here
    _buffers.resize(numBuffers);
    _sets.resize(numBuffers);
    for (size_t i = 0; i < numBuffers; i++) {
        _buffers[i] = new MoeVkBuffer(phyDevice, device, VkDeviceSize(sizeof(UBO)),
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  "for uniform");
    }

    // TODO: Automate from layoutBindings
    std::vector<VkDescriptorPoolSize> poolSizes{
            VkDescriptorPoolSize{
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    static_cast<uint32_t>(_buffers.size())
            },
            VkDescriptorPoolSize{
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    1
            },
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;
    poolInfo.maxSets = _buffers.size();
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();

    if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw InitException("Failed to create descriptor pool.", __FILE__, __FUNCTION__, __LINE__);
    }

    std::vector<VkDescriptorSetLayout> layouts(_buffers.size(), _layout);
    VkDescriptorSetAllocateInfo descriptorAllocateInfo{};
    descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocateInfo.pNext = nullptr;
    descriptorAllocateInfo.descriptorPool = _pool;
    descriptorAllocateInfo.descriptorSetCount = static_cast<uint32_t>(_buffers.size());
    descriptorAllocateInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(device.device(), &descriptorAllocateInfo, _sets.data()) != VK_SUCCESS) {
        throw InitException("Failed to allocate descriptor set.", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkUniformBuffer::updateSets(moe::MoeVkPhysicalDevice &phyDevice, moe::MoeVkLogicalDevice &device,
                                    moe::MoeVkImage &image) {

    assert(_buffers.size() == _sets.size());

    for (size_t i = 0; i < _sets.size(); i++) {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = _buffers[i]->buffer();
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = sizeof(UBO);  // TODO: Do not hardcode size here

        // TODO: Optional ?
        // TODO important: What if we want to load several images ?
        // TODO: What if we load images or vertices while the renderer is already running? Re-create ?
        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.sampler = image.sampler();
        descriptorImageInfo.imageView = image.view();
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

        vkUpdateDescriptorSets(device.device(), writeDescriptorSets.size(),
                               writeDescriptorSets.data(), 0, nullptr);
    }
}

void MoeVkUniformBuffer::destroy(MoeVkLogicalDevice &device) {
    vkDestroyDescriptorSetLayout(device.device(), _layout, nullptr);
    vkDestroyDescriptorPool(device.device(), _pool, nullptr);
    for (size_t i = 0; i < _buffers.size(); i++) {
        if (_buffers[i] != nullptr) {
            delete _buffers[i];
        }
    }
    _buffers.clear();
}
}