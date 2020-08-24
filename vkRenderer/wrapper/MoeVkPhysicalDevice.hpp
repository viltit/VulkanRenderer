#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkQueueFamily.hpp"

namespace moe {
class MoeVkPhysicalDevice {
public:
    MoeVkPhysicalDevice();
    ~MoeVkPhysicalDevice();

    void create(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*> desiredExtensions);

    VkPhysicalDevice& device()       { return _device; }
    MoeVkQueueFamily& queueFamily()  { return _queueFamily; }

    bool isFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);
    VkFormat findFirstSupportedFormat(const std::vector<VkFormat>& formats,
            VkImageTiling tiling, VkFormatFeatureFlags  fetureFlags);
    VkFormat findDepthFormat();

    VkAttachmentDescription getDepthAttachment();

private:

    std::vector<VkPhysicalDevice>   fetchAll     (const VkInstance& instance);
    void                            fetchBest    (const std::vector<VkPhysicalDevice>& devices,
                                                        const std::vector<const char*>& extensions,
                                                        const VkSurfaceKHR& surface);
    int                             score        (const VkPhysicalDevice device,
                                                        const std::vector<const char*>& extensions,
                                                        const VkSurfaceKHR& surface);
    MoeVkQueueFamily                fetchQueueFamilies  (VkPhysicalDevice device, const VkSurfaceKHR& surface);
    bool                            hasExtensions(const VkPhysicalDevice device, const std::vector<const char*>& extensions) const;

    void printPhysicalDeviceStats(VkPhysicalDevice device);
    void printQueueFamilies(const std::vector<VkQueueFamilyProperties>& props) const;

    MoeVkQueueFamily    _queueFamily;
    VkPhysicalDevice    _device;
};
}

