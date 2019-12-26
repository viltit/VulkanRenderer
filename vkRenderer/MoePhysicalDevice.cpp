
#include "MoePhysicalDevice.hpp"
#include "../Exceptions/InitException.hpp"

#include <map>
#include <iostream>

#define __DEBUG__

namespace moe {

MoePhysicalDevice::MoePhysicalDevice(const VkInstance& instance)
    : _instance { instance }
{
    fetchAll();
}

MoePhysicalDevice::~MoePhysicalDevice() {

}

void MoePhysicalDevice::fetchAll() {
    uint32_t numDevices = 0;
    // fetch number of devices
    if (vkEnumeratePhysicalDevices(_instance, &numDevices, nullptr) != VK_SUCCESS) {
        throw InitException("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }
    if (numDevices == 0) {
        throw InitException("Failed to find any device with Vulkan Support.", __FILE__, __FUNCTION__, __LINE__);
    }
    std::vector<VkPhysicalDevice> physDevices(numDevices);
    // fetch devices:
    if (vkEnumeratePhysicalDevices(_instance, &numDevices, physDevices.data()) != VK_SUCCESS) {
        throw InitException("Failed to fetch Physical Devices", __FILE__, __FUNCTION__, __LINE__);
    }

#ifdef __DEBUG__
    std::cout << "Number of gpu's detected: " << numDevices << std::endl;
    for (size_t i = 0; i < numDevices; i++) {
        printPhysicalDeviceStats(physDevices[i]);
    }
#endif
    std::vector<const char*> extension = { };
    fetchBest(physDevices, extension);
}

void MoePhysicalDevice::fetchBest(
        const std::vector<VkPhysicalDevice> &devices,
        std::vector<const char *> extensions) {


    std::multimap<int, VkPhysicalDevice> candidates;

    // rate each device
    for (const auto& device : devices) {
        int newScore = score(device, extensions);
        candidates.insert(std::make_pair(newScore, device));
    }

    // check if the best candidate is suitable:
    if (candidates.rbegin()->first >= 0) {
        _device = candidates.rbegin()->second;
    }
    else {
        throw InitException("No suitable GPU could be found.", __FILE__, __FUNCTION__, __LINE__);
    }
}

int MoePhysicalDevice::score(const VkPhysicalDevice &device, std::vector<const char *> extensions) {

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

    return score;
}

void MoePhysicalDevice::printPhysicalDeviceStats(VkPhysicalDevice device) {

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

}