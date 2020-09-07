#pragma once

#include <vulkan/vulkan.h>

namespace moe {

class MoeVkLogicalDevice;
class MoeVkPhysicalDevice;
class MoeVkSwapChain;

class MoeVkRenderPass {
public:
    MoeVkRenderPass()
        : _device { nullptr },
          _renderPass { VK_NULL_HANDLE }
    { }

    MoeVkRenderPass(const MoeVkRenderPass&) = delete;
    MoeVkRenderPass& operator = (const MoeVkRenderPass&) = delete;

    void create(MoeVkLogicalDevice &device, MoeVkPhysicalDevice& physicalDevice, const MoeVkSwapChain& swapChain);
    void destroy();

    const VkRenderPass& renderPass() const { return _renderPass; }

private:
    MoeVkLogicalDevice*     _device;
    VkRenderPass            _renderPass;

};
}
