#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace moe {
// helper struct to keep track of some important features of our Swap Chain
struct SwapChainProps {

    SwapChainProps(const VkPhysicalDevice &device, const VkSurfaceKHR &surface) {

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

        unsigned int numFormats{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, nullptr);
        // TODO: Error message if numFormats is 0 ??
        if (numFormats != 0) {
            formats.resize(numFormats);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, formats.data());
        }
        unsigned int numPresentationModes{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentationModes, nullptr);
        if (numPresentationModes != 0) {
            presentModes.resize(numPresentationModes);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentationModes, presentModes.data());
        }
    }

    VkSurfaceCapabilitiesKHR capabilities;
    std::vector <VkSurfaceFormatKHR> formats;
    std::vector <VkPresentModeKHR> presentModes;
};
}