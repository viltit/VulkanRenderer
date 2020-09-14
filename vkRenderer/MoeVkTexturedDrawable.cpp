
#include "MoeVkTexturedDrawable.hpp"

#include "wrapper/MoeVkDescriptorSet.hpp"
#include "wrapper/MoeVkLogicalDevice.hpp"
#include "wrapper/MoeTexture.hpp"
#include "wrapper/MoeVkDescriptorPool.hpp"
#include "wrapper/MoeVkUtils.hpp"
#include "MoeDrawable.hpp"

namespace moe {

MoeVkTexturedDrawable::MoeVkTexturedDrawable(
        MoeVkPhysicalDevice& physicalDevice,
        MoeVkLogicalDevice& logicalDevice,
        MoeVkCommandPool& commandPool,
        MoeVkDescriptorPool& descriptorPool,
        Drawable* drawable,
        uint32_t numImagesInSwapchain)
        : MoeVkIDrawable {physicalDevice, logicalDevice, commandPool, descriptorPool, *drawable, numImagesInSwapchain }
{


    _texture.load(drawable->texturePath);
    _texture.upload(logicalDevice, physicalDevice, commandPool, logicalDevice.graphicsQueue());

    // TODO: No new
    _descriptors = new MoeVkDescriptorSet(physicalDevice, logicalDevice,
                                          _texture, descriptorPool, numImagesInSwapchain);
}

MoeVkTexturedDrawable::~MoeVkTexturedDrawable() {
    if (_descriptors) {
        delete _descriptors;
        _descriptors = nullptr;
    }

    if (_vertexBuffer != nullptr) {
        delete _vertexBuffer;
        _vertexBuffer = nullptr;
    }
    if (_indexBuffer != nullptr) {
        delete _indexBuffer;
        _indexBuffer = nullptr;
    }
}

void MoeVkTexturedDrawable::update(
        MoeVkLogicalDevice& device,
        const glm::mat4 &view, const glm::mat4 &perspective,
        const glm::vec3& lightPos,
        uint32_t imageIndex) {

    // TODO: Set view and perspective for all models, and only model matrix for each one ?
    UBO ubo { };
    ubo.M = _drawable.M;
    ubo.V = view;
    ubo.P = perspective;
    ubo.lightPos = lightPos;

    void* data;
    vkMapMemory(device.device(), _descriptors->memory(imageIndex), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device.device(), _descriptors->memory(imageIndex));
}

}