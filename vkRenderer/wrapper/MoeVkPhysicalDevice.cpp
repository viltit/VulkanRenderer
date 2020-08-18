
#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkSwapChainProps.hpp"
#include "../../Exceptions/InitException.hpp"

#include <map>
#include <iostream>

#define __DEBUG__

namespace moe {

MoeVkPhysicalDevice::MoeVkPhysicalDevice() { }

MoeVkPhysicalDevice::~MoeVkPhysicalDevice() { }

void MoeVkPhysicalDevice::create(
        const VkInstance &instance,
        const VkSurfaceKHR &surface,
        const std::vector<const char*> desiredExtensions) {

    auto devices = fetchAll(instance);
    fetchBest(devices, desiredExtensions, surface);
}

std::vector<VkPhysicalDevice> MoeVkPhysicalDevice::fetchAll(const VkInstance& instance) {
    uint32_t numDevices = 0;
    // fetch number of devices
    if (vkEnumeratePhysicalDevices(instance, &numDevices, nullptr) != VK_SUCCESS) {
        throw InitException("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }
    if (numDevices == 0) {
        throw InitException("Failed to find any device with Vulkan Support.", __FILE__, __FUNCTION__, __LINE__);
    }
    std::vector<VkPhysicalDevice> physDevices(numDevices);
    // fetch devices:
    if (vkEnumeratePhysicalDevices(instance, &numDevices, physDevices.data()) != VK_SUCCESS) {
        throw InitException("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }

#ifdef __DEBUG__
    std::cout << "Number of gpu's detected: " << numDevices << std::endl;
    for (size_t i = 0; i < numDevices; i++) {
        printPhysicalDeviceStats(physDevices[i]);
    }
#endif
    return physDevices;
}

void MoeVkPhysicalDevice::fetchBest(
        const std::vector<VkPhysicalDevice> &devices,
        const std::vector<const char *>& extensions,
        const VkSurfaceKHR& surface) {

    std::multimap<int, VkPhysicalDevice> candidates;

    // rate each device
    for (const auto& device : devices) {
        int newScore = score(device, extensions, surface);
        candidates.insert(std::make_pair(newScore, device));
    }

    // check if the best candidate is suitable:
    if (candidates.rbegin()->first >= 0) {
        _device = candidates.rbegin()->second;
        // TODO: We already fetched all families
        _queueFamily = fetchQueueFamilies(_device, surface);
    }
    else {
        throw InitException("No suitable GPU could be found.", __FILE__, __FUNCTION__, __LINE__);
    }
}
/**
 * @param device The Vulkan Handler to the physical device, ie a VkPhysicalDevice
 * @param extensions The extensions the decive should support, ie support for drawing on a surface
 * @return a score of this device, where -1 means that the device is not suited for this app
 */
int MoeVkPhysicalDevice::score(const VkPhysicalDevice device,
        const std::vector<const char *>& extensions,
        const VkSurfaceKHR& surface) {

    int score = 0;
    // fetch device properties and features
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    // dedicated GPU will get high scores. Integrated GPU is just accetable
    // TODO: Let calling code set some rules for rating a device
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 10;
            break;
        default:
            return -1;
    }

    // geometry shader is a must
    if (!features.geometryShader) {
        return -1;
    }
    // tesselation shader is nice to have
    if (features.tessellationShader) {
        score += 5;
    }
    // maximum texture size increases graphic quality
    score += std::min((props.limits.maxImageDimension2D / 10000), uint(5));

    // check if the device queue families are suitable:
    MoeVkQueueFamily family = fetchQueueFamilies(device, surface);
    if (!family.hasGraphics()) {
        return -1;  // no graphics queue is a killer
    }
    if (!family.hasPresentation()) {
        return -1;
    }

    // check if the device has the desired extensions:
    if (!hasExtensions(device, extensions)) {
        return -1;
    }
    // check if the Swap Chain is sufficient
    SwapChainProps swapChainProps { device, surface };
    if (swapChainProps.presentModes.size() == 0 || swapChainProps.formats.size() == 0) {
        return -1;
    }

    return score;
}

MoeVkQueueFamily MoeVkPhysicalDevice::fetchQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR& surface) {

    MoeVkQueueFamily family;
    uint numFamilies { 0 };

    vkGetPhysicalDeviceQueueFamilyProperties(device, &numFamilies, nullptr);
    std::vector<VkQueueFamilyProperties> familyProps(numFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &numFamilies, familyProps.data());

#ifdef __DEBUG__
    printQueueFamilies(familyProps);
#endif

    for (size_t i = 0; i < numFamilies; i++) {
        if (familyProps[i].queueCount > 0 && familyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            family.graphics.push_back(i);
            family.numQueues.push_back(familyProps[i].queueCount);
        }

        VkBool32 hasPresentation;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasPresentation);
        if (familyProps[i].queueCount > 0 && hasPresentation) {
            family.presentation.push_back(i);
        }
    }
    return family;
}

bool MoeVkPhysicalDevice::hasExtensions(const VkPhysicalDevice device, const std::vector<const char*>& desiredExtensions) const {

    uint numExtensions;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, nullptr);
    std::vector<VkExtensionProperties> extensions(numExtensions);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &numExtensions, extensions.data());

    uint unconfirmedExtensions = desiredExtensions.size();
    for (const auto& desired : desiredExtensions) {
        for (const auto& available : extensions) {
            if (std::string(available.extensionName) == std::string(desired)) {
                unconfirmedExtensions--;
                break;
            }
        }
    }
    return unconfirmedExtensions == 0;
}


void MoeVkPhysicalDevice::printPhysicalDeviceStats(VkPhysicalDevice device) {

    std::map<VkPhysicalDeviceType, std::string> types = {
            { VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, "Dedicated GPU" },
            { VK_PHYSICAL_DEVICE_TYPE_CPU, "CPU" },
            { VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, "Integrated GPU" },
            { VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU, "Virtual GPU" },
            { VK_PHYSICAL_DEVICE_TYPE_OTHER, "Unknown" }
    };

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    std::cout << "\tName: "     << props.deviceName << std::endl;
    std::cout << "\tAPI-Version: " << VK_VERSION_MAJOR(props.apiVersion) << "."
              << VK_VERSION_MINOR(props.apiVersion) << "."
              << VK_VERSION_PATCH(props.apiVersion) << std::endl;

    std::cout << "\tType: " << types[props.deviceType] << std::endl;

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    std::cout << "\tGeometry shader: " << (features.geometryShader ? "Yes" : "No") << std::endl;
    std::cout << "\tTesselation shader: " << (features.tessellationShader ? "Yes" : "No") << std::endl;

    std::cout << "\tMax image size (with or height): " << props.limits.maxImageDimension2D << " px" << std::endl;

    // VkPhysicalDeviceMemoryProperties memory;
    // vkGetPhysicalDeviceMemoryProperties(device, &memory);
}

void MoeVkPhysicalDevice::printQueueFamilies(const std::vector<VkQueueFamilyProperties>& props) const {

        std::cout << "\t\tSupported queues:\n";
        for (size_t i = 0; i < props.size(); i++) {
            std::cout << "\t\tQueue Family #" << i << ": ";
            std::cout << (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? "Graphics | " : " 0 | ");
            std::cout << (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT ? "Compute | " : " 0 | ");
            std::cout << (props[i].queueFlags & VK_QUEUE_TRANSFER_BIT ? "Transfer | " : " 0 | ");
            std::cout << (props[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "Sparse Binding" : "");
            std::cout << "\n\t\t\t" << "Queue count: " << props[i].queueCount << "\n";
        }
    }

}