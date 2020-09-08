
#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkSwapChainProps.hpp"
#include "MoeExceptions.hpp"

#include <map>
#include <spdlog/spdlog.h>

namespace moe {

MoeVkPhysicalDevice::MoeVkPhysicalDevice(const VkInstance& instance,
                                         const VkSurfaceKHR& surface,
                                         const std::vector<const char*> desiredExtensions) {

    auto devices = fetchAll(instance);
    fetchBest(devices, desiredExtensions, surface);
}

MoeVkPhysicalDevice::~MoeVkPhysicalDevice() { }

std::vector<VkPhysicalDevice> MoeVkPhysicalDevice::fetchAll(const VkInstance& instance) {
    uint32_t numDevices = 0;
    // fetch number of devices
    if (vkEnumeratePhysicalDevices(instance, &numDevices, nullptr) != VK_SUCCESS) {
        throw MoeInitError("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }
    if (numDevices == 0) {
        throw MoeInitError("Failed to find any device with Vulkan Support.", __FILE__, __FUNCTION__, __LINE__);
    }
    std::vector<VkPhysicalDevice> physDevices(numDevices);
    // fetch devices:
    if (vkEnumeratePhysicalDevices(instance, &numDevices, physDevices.data()) != VK_SUCCESS) {
        throw MoeInitError("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }

    spdlog::info("Number of gpu's detected: {0}", numDevices);
    for (size_t i = 0; i < numDevices; i++) {
        printPhysicalDeviceStats(physDevices[i]);
    }
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
        throw MoeInitError("No suitable GPU could be found.", __FILE__, __FUNCTION__, __LINE__);
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

    printQueueFamilies(familyProps);

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

    spdlog::info("Name: " + std::string(props.deviceName));
    spdlog::info("\tAPI-Version: {0}.{1}.{2}", VK_VERSION_MAJOR(props.apiVersion),
            VK_VERSION_MINOR(props.apiVersion),
            VK_VERSION_PATCH(props.apiVersion));

    spdlog::info("\tType: {0}", types[props.deviceType]);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    spdlog::info("\tGeometry shader: {0}", (features.geometryShader ? "Yes" : "No"));
    spdlog::info("\tTesselation shader: {0}", (features.tessellationShader ? "Yes" : "No"));

    spdlog::info("\tMax image size (with or height): {0} px", props.limits.maxImageDimension2D);

    // VkPhysicalDeviceMemoryProperties memory;
    // vkGetPhysicalDeviceMemoryProperties(device, &memory);
}

void MoeVkPhysicalDevice::printQueueFamilies(const std::vector<VkQueueFamilyProperties>& props) const {

        spdlog::info("\tSupported queues:");
        for (size_t i = 0; i < props.size(); i++) {
            spdlog::info("\tQueue Family #{0}: {1}{2}{3}{4}", i,
                         (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? "Graphics, " : ""),
                         (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT ? "Compute, " : ""),
                         (props[i].queueFlags & VK_QUEUE_TRANSFER_BIT ? "Transfer, " : ""),
                         (props[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "Sparse Binding" : ""));

            spdlog::info("\tQueue count: {0}", props[i].queueCount);
        }
    }

bool MoeVkPhysicalDevice::isFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) {

    VkFormatProperties props { };
    vkGetPhysicalDeviceFormatProperties(_device, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & featureFlags) == featureFlags) {
        return true;
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & featureFlags) == featureFlags) {
        return true;
    }
    else {
        return false;
    }
}

VkFormat MoeVkPhysicalDevice::findFirstSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling,
                                                       VkFormatFeatureFlags featureFlags) {
    for (VkFormat format : formats) {
        if (isFormatSupported(format, tiling, featureFlags)) {
            return format;
        }
    }
    throw MoeInitError("Could not find any supported image format", __FILE__, __FUNCTION__, __LINE__);
}

VkFormat MoeVkPhysicalDevice::findDepthFormat() {
    std::vector<VkFormat> possibleFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, // all new grafic cards support this format
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT    // fallback option, has no stencil
    };
    return findFirstSupportedFormat(possibleFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkAttachmentDescription MoeVkPhysicalDevice::getDepthAttachment() {
    VkAttachmentDescription result { };
    result.flags            = 0;
    result.format           = findDepthFormat();
    result.samples          = VK_SAMPLE_COUNT_1_BIT;  // TODO: Add multisampling ??
    result.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear with each frame
    result.storeOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    result.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    result.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    result.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    result.finalLayout      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    return result;
}

}