#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkQueueFamily.hpp"

namespace moe {
class MoeVkPhysicalDevice {
public:
    MoeVkPhysicalDevice(const VkInstance& instance,
            const VkSurfaceKHR& surface,
            const std::vector<const char*> desiredExtensions);
    ~MoeVkPhysicalDevice();

    const VkPhysicalDevice& device() const { return _device; }
    MoeVkQueueFamily& queueFamily()  { return _queueFamily; }

    bool isFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const;

    VkFormat findFirstSupportedFormat(const std::vector<VkFormat>& formats,
            VkImageTiling tiling, VkFormatFeatureFlags  fetureFlags) const;

    VkFormat findDepthFormat() const;
    VkAttachmentDescription getDepthAttachment() const;

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

