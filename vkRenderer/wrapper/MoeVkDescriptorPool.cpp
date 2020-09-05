//
// Created by titus on 14.08.20.
//

#include "MoeVkDescriptorPool.hpp"

namespace moe {
void MoeVkDescriptorPool::createLayout(MoeVkPhysicalDevice& phyDevice, MoeVkLogicalDevice &device) {

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
        throw MoeInitError("Failed to create descriptor set layout", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkDescriptorPool::createPool(moe::MoeVkPhysicalDevice &phyDevice, moe::MoeVkLogicalDevice &device, size_t numBuffers) {

    // TODO: Automate from layoutBindings
    std::vector<VkDescriptorPoolSize> poolSizes{
            VkDescriptorPoolSize{
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    static_cast<uint32_t>(numBuffers)
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
    poolInfo.maxSets = numBuffers;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();

    if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &_pool) != VK_SUCCESS) {
        throw MoeInitError("Failed to create descriptor pool.", __FILE__, __FUNCTION__, __LINE__);
    }
}


void MoeVkDescriptorPool::destroy(MoeVkLogicalDevice &device) {
    vkDestroyDescriptorSetLayout(device.device(), _layout, nullptr);
    vkDestroyDescriptorPool(device.device(), _pool, nullptr);
}
}