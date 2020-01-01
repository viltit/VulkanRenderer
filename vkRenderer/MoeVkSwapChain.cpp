
#include "MoeVkSwapChain.hpp"
#include "../Exceptions/InitException.hpp"

#include <SDL2/SDL_vulkan.h>
#include <iostream>

namespace moe {

MoeVkSwapChain::MoeVkSwapChain()
{ }

MoeVkSwapChain::~MoeVkSwapChain() { }

void MoeVkSwapChain::destroy(MoeVkLogicalDevice &device) {
    for (size_t i = 0; i < _imageViews.size(); i++) {
        vkDestroyImageView(device.device(), _imageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(device.device(), _swapChain, nullptr);
}

void MoeVkSwapChain::create(
        MoeVkPhysicalDevice& device,
        MoeVkLogicalDevice& logicalDevice,
        const VkSurfaceKHR &surface,
        const moe::VkWindow &window) {

    _properties = SwapChainProps( device.device(), surface );
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.device(), surface, &_capabilities);
    _format = fetchBestFormat();
    _presentMode = fetchBestPresentMode();
    _extent = fetchBestExtent(window);

    uint numImages = _capabilities.minImageCount + 1;
    if (_capabilities.maxImageCount > 0 && numImages > _capabilities.maxImageCount) {
        numImages = _capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo { };
    createInfo.sType                = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext                = nullptr;
    createInfo.flags                = 0;
    createInfo.surface              = surface;
    createInfo.minImageCount        = numImages;
    createInfo.imageFormat          = _format.format;
    createInfo.imageColorSpace      = _format.colorSpace;
    createInfo.imageExtent          = _extent;
    createInfo.imageArrayLayers     = 1;    // 1 for most apps, exept maybe a stereoscopic app
    createInfo.imageUsage           = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // draw direcly to the screen -> no postporocessing for now

    // If we have two separate queues for graphics and presentation, the swap chain needs to share images between them:
    uint queueIndices[] = { device.queueFamily().selectedGraphicsIndex, device.queueFamily().selectedPresentationIndex };
    if (queueIndices[0] != queueIndices[1]) {
        // ownership of swap chain images is shared
        /* TODO: For performance, an exlusive mode is better -> this would need to explicitly handle swap chain ownership
            in subsequent code */
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform     = _capabilities.currentTransform;       // we do NOT want any transformations of images in the swap chain
    createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // no transparency to other windows
    createInfo.presentMode      = _presentMode;
    createInfo.clipped          = VK_TRUE;  // we are not interested in clipped pixel, for example behind another window
    createInfo.oldSwapchain     = nullptr;

    if (vkCreateSwapchainKHR(logicalDevice.device(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw InitException("Failed to create Swapchain.", __FILE__, __FUNCTION__, __LINE__);
    }

    // get a handle to the images in the Swapchain:
    vkGetSwapchainImagesKHR(logicalDevice.device(), _swapChain, &numImages, nullptr);
    _images.resize(numImages);
    vkGetSwapchainImagesKHR(logicalDevice.device(), _swapChain, &numImages, _images.data());

    // create a view into these images:
    createImageViews(logicalDevice);
}

void MoeVkSwapChain::createImageViews(MoeVkLogicalDevice& device) {

    _imageViews.resize(_images.size());
    for (size_t i = 0; i < _images.size(); i++) {
        VkImageViewCreateInfo createInfo { };
        createInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext            = nullptr;
        createInfo.flags            = 0;
        createInfo.image            = _images[i];
        createInfo.viewType         = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format           = _format.format;
        // we *could* map all channels into the red channel or stuff like that here
        createInfo.components       = { VK_COMPONENT_SWIZZLE_IDENTITY,
                                        VK_COMPONENT_SWIZZLE_IDENTITY,
                                        VK_COMPONENT_SWIZZLE_IDENTITY,
                                        VK_COMPONENT_SWIZZLE_IDENTITY };
        // TODO later: We could add mipmapping here
        createInfo.subresourceRange.aspectMask = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.device(), &createInfo, nullptr, &_imageViews[i]) != VK_SUCCESS) {
            throw InitException("Failed to create an Image View.", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

// TODO: Let the calling code define what is the best format
VkSurfaceFormatKHR MoeVkSwapChain::fetchBestFormat() const {
    for (const auto& format : _properties.formats) {
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) {
            return format;
        }
    }
    /* TODO: We could include this in the physical device rating score. For now, we just pick the first format if
        we did not found our "ideal" format */
    return _properties.formats[0];
}

/**
 * The present mode is one of the most important aspects of the swap chain.
 * For example:
 * -    VK_PRESENT_MODE_IMMEDIATE_KHR means that the swap chain will submit an image IMMEDIATLY to the screen,
 *      which may cause visible artifacts
 * -    VK_PRESENT_MODE_FIFO_KHR means the swap chain has a queue of images, where the screen will take an image from
 *      front of the queue whenever it is refreshing, while the gpu draws its images at the end of the queue
 *  The best option is VK_PRESENT_MODE_MAILBOX_KHR, where the queue is not blocked when it is full, but existing images
 *  will be overwritten. This option also allows for tripple buffering.
 */
VkPresentModeKHR MoeVkSwapChain::fetchBestPresentMode() const {
    for (const auto& presentMode : _properties.presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }
    // TODO: Again, we could include this in the score of a device
    // VK_PRESENT_MODE_FIFO_KHR is guaranteed to be avalable:
    return VK_PRESENT_MODE_FIFO_KHR;
}

/**
 * In most cases, the SDL-Window has alreade set a value matching the extent of our window
 * However, we have no guarantee :(
 *
 * TODO: SwapChain should not know the window. just give it the extent
 */
VkExtent2D MoeVkSwapChain::fetchBestExtent(const VkWindow& window) {
    // the window system already set the value to its size:
    if (_capabilities.currentExtent.width != UINT32_MAX) {
        return _capabilities.currentExtent;
    }
    // we need to get the size ourselfes:
    int w;
    int h;
    SDL_Vulkan_GetDrawableSize(window.sdlWindow(), &w, &h);
    VkExtent2D result = { uint(w), uint(h) };

    result.width = std::max(_capabilities.minImageExtent.width, std::min(_capabilities.maxImageExtent.width, result.width));
    result.height = std::max(_capabilities.minImageExtent.height, std::min(_capabilities.maxImageExtent.height, result.height));

    return result;
}

}