#pragma once

#include <vulkan/vulkan.h>
#include "MoeVkLogicalDevice.hpp"
#include <iostream>
namespace moe {

/*
 * While semaphores are good to signal the gpu to wait on a certain operations, fences can be waited for explicitly in
 * our code and are therefore more suited for cpu-gpu synchronization.
 */
class MoeVkFence {
public:
    MoeVkFence() { _fence = VK_NULL_HANDLE; }
    ~MoeVkFence() { }

    MoeVkFence& operator = ( const MoeVkFence& other) {
        if (&other == this) {
            return *this;
        }
        this->_fence = other._fence;
        return *this;
    }

    void create(MoeVkLogicalDevice& device);
    void destroy(MoeVkLogicalDevice& device);

    void wait(MoeVkLogicalDevice& device, uint64_t timeoutLimit) const {
        vkWaitForFences(device.device(), 1, &_fence, VK_TRUE, timeoutLimit);
    }
    void reset(MoeVkLogicalDevice& device) const {
        vkResetFences(device.device(), 1, &_fence);
    }

    VkFence& fence() { return _fence; }
    bool isNull() const { return _fence == VK_NULL_HANDLE; }

private:
    VkFence _fence;
};
}
