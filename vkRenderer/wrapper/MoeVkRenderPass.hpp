#pragma once

#include <vulkan/vulkan.h>

namespace moe {

class MoeVkLogicalDevice;
class MoeVkPhysicalDevice;
class MoeVkSwapChain;

class MoeVkRenderPass {
public:
    MoeVkRenderPass(const MoeVkLogicalDevice &device,
            const MoeVkPhysicalDevice& physicalDevice,
            const MoeVkSwapChain& swapChain);

    ~MoeVkRenderPass();

    MoeVkRenderPass(const MoeVkRenderPass&) = delete;
    MoeVkRenderPass& operator = (const MoeVkRenderPass&) = delete;


    const VkRenderPass& renderPass() const { return _renderPass; }

private:
    const MoeVkLogicalDevice&     _device;
    VkRenderPass                  _renderPass;

};
}
