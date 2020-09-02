#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "wrapper/MoeVkArrayBuffer.hpp"
#include "wrapper/MoeTexture.hpp"

namespace moe {

class MoeVkDescriptorSet;
class Drawable;
class MoeVkDescriptorPool;
class MoeVkPhysicalDevice;
class MoeVkLogicalDevice;
class MoeVkCommandPool;

class MoeVkDrawable {
public:
    MoeVkDrawable(MoeVkPhysicalDevice& physicalDevice,
            MoeVkLogicalDevice& logicalDevice,
            MoeVkCommandPool& commandPool,
            MoeVkDescriptorPool& descriptorPool,
            Drawable* drawable,
            uint32_t numImagesInSwapchain);

    void update(MoeVkLogicalDevice& device,
            const glm::mat4& view, const glm::mat4& perspective,
            const glm::vec3& lightPos,
            uint32_t imageIndex);

    MoeVkArrayBuffer<Vertex>*   vertexBuffer() { return  _vertexBuffer; }
    MoeVkArrayBuffer<uint32_t>*  indexBuffer()  { return _indexBuffer; }
    MoeVkDescriptorSet& descriptors() { return *_descriptors; }

    ~MoeVkDrawable();

private:
    // TODO: We need a texture and vertex buffer cache in the long run
    MoeVkArrayBuffer<Vertex>*       _vertexBuffer;
    MoeVkArrayBuffer<uint32_t>*     _indexBuffer;
    MoeTexture                      image;

    MoeVkDescriptorSet*     _descriptors;
    Drawable*               _drawable;
};
}
