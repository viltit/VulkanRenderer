
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
        MoeVkDescriptorPool& descriptorPool,
        MoeTexture& texture,
        Drawable* drawable,
        uint32_t numImagesInSwapchain)

        :   _descriptors {  new MoeVkDescriptorSet(physicalDevice, logicalDevice,
                texture, descriptorPool, numImagesInSwapchain) },
            _drawable { drawable }

{
    assert(_drawable);
    assert(_descriptors->numSets() == 3);
}

MoeVkDrawable::~MoeVkDrawable() {
    if (_descriptors) {
        delete _descriptors;
        _descriptors = nullptr;
    }
}

void MoeVkDrawable::update(
        MoeVkLogicalDevice& device,
        const glm::mat4 &view, const glm::mat4 &perspective, uint32_t imageIndex) {

    // TODO: Set view and perspective for all models, and only model matrix for each one ?
    UBO ubo { };
    ubo.M = _drawable->M;
    ubo.V = view;
    ubo.P = perspective;

    void* data;
    vkMapMemory(device.device(), descriptors().memory(imageIndex), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device.device(), descriptors().memory(imageIndex));
}

}