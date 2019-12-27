#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkQueueFamily.hpp"

namespace moe {
class MoeVkPhysicalDevice {
public:
    MoeVkPhysicalDevice(const VkInstance& instance);
    ~MoeVkPhysicalDevice();

    VkPhysicalDevice device()       { return _device; }
    MoeVkQueueFamily queueFamily()  { return _queueFamily; }

private:

    void        fetchAll            ();
    void        fetchBest           (const std::vector<VkPhysicalDevice>& devices, std::vector<const char *> extensions);
    int         score               (const VkPhysicalDevice& device, std::vector<const char*> extensions);
    MoeVkQueueFamily fetchQueueFamilies  (VkPhysicalDevice device);

    void printPhysicalDeviceStats(VkPhysicalDevice device);
    void printQueueFamilies(const std::vector<VkQueueFamilyProperties>& props) const;

    const VkInstance&   _instance;
    MoeVkQueueFamily    _queueFamily;
    VkPhysicalDevice    _device;
};
}

