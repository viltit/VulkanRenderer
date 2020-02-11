#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkSwapChainProps.hpp"
#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "VkWindow.hpp"

namespace moe {

class MoeVkSwapChain {
public:
    MoeVkSwapChain();
    ~MoeVkSwapChain();

    void create(MoeVkPhysicalDevice& device,
                MoeVkLogicalDevice& logicalDevice,
                const VkSurfaceKHR& surface,
                const VkWindow& window);

    void destroy(MoeVkLogicalDevice& device);

    VkExtent2D                  extent() const { return _extent; }
    VkSurfaceFormatKHR          format() const { return _format; }
    std::vector<VkImageView>&   images()       { return _imageViews; }


private:
    VkSurfaceFormatKHR  fetchBestFormat() const;
    VkPresentModeKHR    fetchBestPresentMode() const;
    VkExtent2D          fetchBestExtent(const VkWindow& window);
    void                createImageViews(MoeVkLogicalDevice& device);

    VkSwapchainKHR              _swapChain;
    SwapChainProps              _properties;
    VkSurfaceCapabilitiesKHR    _capabilities;
    VkSurfaceFormatKHR          _format;
    VkPresentModeKHR            _presentMode;
    VkExtent2D                  _extent;
    std::vector<VkImage>        _images;
    std::vector<VkImageView >   _imageViews;

};
}