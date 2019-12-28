#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkQueueFamily.hpp"

namespace moe {
class MoeVkPhysicalDevice {
public:
    MoeVkPhysicalDevice();
    ~MoeVkPhysicalDevice();

    void create(const VkInstance& instance, const VkSurfaceKHR& surface);

    VkPhysicalDevice device()       { return _device; }
    MoeVkQueueFamily queueFamily()  { return _queueFamily; }

private:

    std::vector<VkPhysicalDevice>   fetchAll     (const VkInstance& instance);
    void                            fetchBest    (const std::vector<VkPhysicalDevice>& devices,
                                                        std::vector<const char *> extensions,
                                                        const VkSurfaceKHR& surface);
    int                             score        (const VkPhysicalDevice device,
                                                        std::vector<const char*> extensions,
                                                        const VkSurfaceKHR& surface);
    MoeVkQueueFamily                fetchQueueFamilies  (VkPhysicalDevice device, const VkSurfaceKHR& surface);

    void printPhysicalDeviceStats(VkPhysicalDevice device);
    void printQueueFamilies(const std::vector<VkQueueFamilyProperties>& props) const;

    MoeVkQueueFamily    _queueFamily;
    VkPhysicalDevice    _device;
};
}

