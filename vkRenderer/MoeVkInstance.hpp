#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace moe {

enum class RendererOptions {
    none        = 0x00,
    validation  = 0x01
};

class VkWindow;

class MoeVkInstance {
public:
    MoeVkInstance(VkWindow* window, RendererOptions options);

private:
    void createDebugMessenger();
    bool hasExtensions(const std::vector<const char*>& extensions) const;
    bool hasLayerSupport(const std::vector<const char*>& layers) const;

    VkInstance                  instance;
    VkDebugUtilsMessengerEXT    debugMessenger;
};
}