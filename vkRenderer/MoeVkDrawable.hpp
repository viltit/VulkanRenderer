#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace moe {

class MoeVkDescriptorSet;
class Drawable;
class MoeVkDescriptorPool;
class MoeVkPhysicalDevice;
class MoeVkLogicalDevice;
class MoeTexture;

class MoeVkDrawable {
public:
    MoeVkDrawable(MoeVkPhysicalDevice& physicalDevice,
            MoeVkLogicalDevice& logicalDevice,
            MoeVkDescriptorPool& descriptorPool,
            MoeTexture& texture,
            Drawable* drawable,
            uint32_t numImagesInSwapchain);

    void update(MoeVkLogicalDevice& device,
            const glm::mat4& view, const glm::mat4& perspective,
            uint32_t imageIndex);

    MoeVkDescriptorSet& descriptors() { return *_descriptors; }

    ~MoeVkDrawable();

private:
    MoeVkDescriptorSet*     _descriptors;
    Drawable*               _drawable;
};
}
