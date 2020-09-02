
#include "MoeVkDrawable.hpp"

#include "wrapper/MoeVkDescriptorSet.hpp"
#include "wrapper/MoeVkLogicalDevice.hpp"
#include "wrapper/MoeTexture.hpp"
#include "wrapper/MoeVkDescriptorPool.hpp"
#include "wrapper/MoeVkUtils.hpp"
#include "MoeDrawable.hpp"

namespace moe {

MoeVkDrawable::MoeVkDrawable(
        MoeVkPhysicalDevice& physicalDevice,
        MoeVkLogicalDevice& logicalDevice,
        MoeVkCommandPool& commandPool,
        MoeVkDescriptorPool& descriptorPool,
        Drawable* drawable,
        uint32_t numImagesInSwapchain)
        : _drawable { drawable }
{
    assert(_drawable != nullptr);

    _vertexBuffer = new MoeVkArrayBuffer<Vertex>(physicalDevice, logicalDevice,
                                                 commandPool,
                                                 drawable->vertices,
                                                 MoeBufferUsage::vertexBuffer,
                                                 "vertex buffer");
    _indexBuffer = new MoeVkArrayBuffer<uint32_t>(physicalDevice, logicalDevice,
                                                  commandPool,
                                                  drawable->indices,
                                                  MoeBufferUsage::indexBuffer,
                                                  "index buffer");

    image.load(drawable->texturePath);
    image.upload(logicalDevice, physicalDevice, commandPool, logicalDevice.graphicsQueue());

    _descriptors = new MoeVkDescriptorSet(physicalDevice, logicalDevice,
                                           image, descriptorPool, numImagesInSwapchain);
}

MoeVkDrawable::~MoeVkDrawable() {
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

void MoeVkDrawable::update(
        MoeVkLogicalDevice& device,
        const glm::mat4 &view, const glm::mat4 &perspective,
        const glm::vec3& lightPos,
        uint32_t imageIndex) {

    // TODO: Set view and perspective for all models, and only model matrix for each one ?
    UBO ubo { };
    ubo.M = _drawable->M;
    ubo.V = view;
    ubo.P = perspective;
    ubo.lightPos = lightPos;

    void* data;
    vkMapMemory(device.device(), descriptors().memory(imageIndex), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device.device(), descriptors().memory(imageIndex));
}

}