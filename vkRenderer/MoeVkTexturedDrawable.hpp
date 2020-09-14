#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "wrapper/MoeVkArrayBuffer.hpp"
#include "wrapper/MoeTexture.hpp"
#include "MoeDrawable.hpp"

namespace moe {

class MoeVkDescriptorSet;
class MoeVkDescriptorPool;
class MoeVkPhysicalDevice;
class MoeVkLogicalDevice;
class MoeVkCommandPool;

/**
 * Base class for all drawable objects
 * @tparam T: The vertex type of this drawable
 */
template <class T>
class MoeVkIDrawable {
public:
    MoeVkIDrawable(MoeVkPhysicalDevice& physicalDevice,
                   MoeVkLogicalDevice& logicalDevice,
                   MoeVkCommandPool& commandPool,
                   MoeVkDescriptorPool& descriptorPool,
                   Drawable& drawable,
                   uint32_t numImagesInSwapchain)
        : _device   { logicalDevice },
          _drawable { drawable }
    {
        // TODO: Use a cache for the buffers
        _vertexBuffer = new MoeVkArrayBuffer<Vertex>(physicalDevice, logicalDevice,
                                                     commandPool,
                                                     drawable.vertices,
                                                     MoeBufferUsage::vertexBuffer,
                                                     "vertex buffer");
        _indexBuffer = new MoeVkArrayBuffer<uint32_t>(physicalDevice, logicalDevice,
                                                      commandPool,
                                                      drawable.indices,
                                                      MoeBufferUsage::indexBuffer,
                                                      "index buffer");
    }

    virtual void update(MoeVkLogicalDevice& device,
                        const glm::mat4& view, const glm::mat4& perspective,
                        const glm::vec3& lightPos,
                        uint32_t imageIndex) = 0;

    MoeVkDescriptorSet& descriptors() { return *_descriptors; }
    MoeVkArrayBuffer<T>*   vertexBuffer() { return  _vertexBuffer; }
    MoeVkArrayBuffer<uint32_t>*  indexBuffer()  { return _indexBuffer; }

protected:
    // TODO: We need a texture and vertex buffer cache in the long run
    MoeVkArrayBuffer<T>*           _vertexBuffer;
    MoeVkArrayBuffer<uint32_t>*    _indexBuffer;

    MoeVkLogicalDevice&     _device;
    MoeVkDescriptorSet*     _descriptors;
    Drawable&               _drawable;
};

class MoeVkTexturedDrawable : public MoeVkIDrawable<Vertex> {
public:
    MoeVkTexturedDrawable(MoeVkPhysicalDevice& physicalDevice,
                          MoeVkLogicalDevice& logicalDevice,
                          MoeVkCommandPool& commandPool,
                          MoeVkDescriptorPool& descriptorPool,
                          Drawable* drawable,
                          uint32_t numImagesInSwapchain);

    virtual void update(MoeVkLogicalDevice& device,
            const glm::mat4& view, const glm::mat4& perspective,
            const glm::vec3& lightPos,
            uint32_t imageIndex) override;

    ~MoeVkTexturedDrawable();

private:
    MoeTexture                      _texture;
};
}
