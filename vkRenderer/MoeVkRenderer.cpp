#include "MoeVkRenderer.hpp"
#include "../Exceptions/InitException.hpp"
#include "VkWindow.hpp"

#include <iostream>
#include <SDL_vulkan.h>

#define __DEBUG__

namespace moe {
/*  --------------------------------------------------------------------------------------------------------------------
    Helper function for Vulkan extension functions that need to be queried for their adress first
    TODO: Put these in own file
    --------------------------------------------------------------------------------------------------------------------*/

/* callback function actvated by Vulkan in case of an error - only used when the validation layer is enabled */
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pData,
        void* pUserData
)
{
    std::cerr << "<Vulkan> Validation layer message: " << pData->pMessage << std::endl;
    return VK_FALSE;
}

/*  fetch adress for extension function
*/
VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, messenger, pAllocator);
    } else {
        // TODO
    }
}

/*  --------------------------------------------------------------------------------------------------------------------
    Renderer
    --------------------------------------------------------------------------------------------------------------------*/
// TODO: Decouple Vulkan initializing from renderer
// TODO: Go on with selecting the QueueFamilies

MoeVkRenderer::MoeVkRenderer(VkWindow* window, RendererOptions options)
    :   instance        { VK_NULL_HANDLE },
        surface         { VK_NULL_HANDLE },
        debugMessenger  { VK_NULL_HANDLE },
        physicalDevice  { nullptr }
{
    createInstance(window, options);
    createSurface(window);
    physicalDevice = new MoePhysicalDevice(instance);
}

MoeVkRenderer::~MoeVkRenderer() {
    if (debugMessenger) {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
}

void MoeVkRenderer::createInstance(VkWindow* window, RendererOptions options) {

    // query extensions for SDL Vulkan Window:
    // TODO: Put this code into window ??
    uint numExtensions{ 0 };
    std::vector<const char*> extensions;
    if (!SDL_Vulkan_GetInstanceExtensions(window->sdlWindow(), &numExtensions, nullptr)) {
        // TODO: Proper error handling
        std::cout << "SDL failed to get Vulkan extensions" << std::endl;
    }
    extensions.resize(numExtensions);
    if (!SDL_Vulkan_GetInstanceExtensions(window->sdlWindow(), &numExtensions, extensions.data())) {
        // TODO: Proper error handling
        std::cout << "SDL failed to get Vulkan extensions" << std::endl;
    }

    // Check if validation should be active
    // TODO: Put validation layer creation in an own function
    uint layerCount = 0;
    std::vector<const char*> validationLayers;
    if (uint(options) & uint(RendererOptions::validation)) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
        if (!hasLayerSupport(validationLayers)) {
            // TODO: Error handling
            std::cout << "Failed to get support for Validation Layers!\n";
        }
    }

    if (!hasExtensions(extensions)) {
        // TODO: Error handling
        std::cout << "SDL needs extensions not supported by Vulkan." << std::endl;
    }

    VkApplicationInfo appInfo { };
    appInfo.sType                   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext                   = nullptr;
    appInfo.pApplicationName        = "vkApp";
    appInfo.applicationVersion      = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName             = "vkRenderer";
    appInfo.engineVersion           = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion              = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo { };
    createInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext                = nullptr;
    createInfo.flags                = 0;
    createInfo.pApplicationInfo     = &appInfo;
    createInfo.enabledLayerCount    = validationLayers.size();
    createInfo.ppEnabledLayerNames  = validationLayers.data();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        // TODO: Include Vulkan error code as String
        throw InitException("Failed to create Vulkan instance", __FILE__, __FUNCTION__, __LINE__);
    }

    // create debug messenger if validation layer is active:
    if (uint(options) & uint(RendererOptions::validation)) {
        createDebugMessenger();
    }
}

void MoeVkRenderer::createSurface(moe::VkWindow *window) {
    if (!window) {
        return;
    }

#ifndef NDEBUG
    std::cout << "<VulkanHandler> Creating surface ...";
#endif

    if (!SDL_Vulkan_CreateSurface(window->sdlWindow(), instance, &surface)) {
        // TODO: Error handling
        std::cout << "Could not create a vulkan surface for the window." << std::endl;
        std::cout << SDL_GetError() << std::endl;
    }

#ifndef NDEBUG
    std::cout << "\t\tfinsihed\n";
#endif
}

void MoeVkRenderer::createDebugMessenger() {
#ifndef NDEBUG
    std::cout << "<VulkanHandler> Creating debug messenger ...";
#endif

    VkDebugUtilsMessengerCreateInfoEXT createInfo = { };
    createInfo.sType            = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // createInfo.pNext;
    // createInfo.flags;
    createInfo.messageSeverity  =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback  = debugCallback;
    createInfo.pUserData = nullptr;

    if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        std::cout << "failed.\n";
        return;
    }

#ifndef NDEBUG
    std::cout << "\tfinsihed\n";
#endif
}

bool MoeVkRenderer::hasExtensions(const std::vector<const char*>& extensions) const {
    uint numExtensions { 0 };
    vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr);
    std::vector<VkExtensionProperties> available(numExtensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, available.data());

    // TODO: Ineffective brute-force, but since its done only once, there is no priority
    for (const auto& desExt : extensions) {
        bool result = false;
        for (const auto& ext : available) {
            if (std::string(ext.extensionName) == std::string(desExt)) {
                result = true;
                break;
            }
        }
        if (!result) return false;
    }
    return true;
}

bool MoeVkRenderer::hasLayerSupport(const std::vector<const char*>& layers) const {
    uint numLayers { 0 };
    vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
    std::vector<VkLayerProperties> available(numLayers);
    vkEnumerateInstanceLayerProperties(&numLayers, available.data());

    // TODO: Same as in extensions: Ineffective brute-force
    for (const auto& desLayer : layers) {
        bool result = false;
        for (const auto& layer : available) {
            if (std::string(layer.layerName) == std::string(desLayer)) {
                result = true;
                break;
            }
        }
        if (!result) return false;
    }
    return true;
}


void MoeVkRenderer::findQueueFamilies(VkPhysicalDevice device) {

}
}