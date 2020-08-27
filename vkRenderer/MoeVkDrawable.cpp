
#include "MoeVkDrawable.hpp"

#include "wrapper/MoeVkDescriptorSet.hpp"
#include "wrapper/MoeVkLogicalDevice.hpp"
#include "wrapper/MoeTexture.hpp"
#include "wrapper/MoeVkDescriptorPool.hpp"

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

}