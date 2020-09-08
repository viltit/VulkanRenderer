#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <utility>

namespace moe {

enum class RendererOptions {
    none        = 0x00,
    validation  = 0x01
};

class VkWindow;

class MoeVkInstance {
public:
    MoeVkInstance(VkWindow* window, RendererOptions options);
    ~MoeVkInstance();

    MoeVkInstance(const MoeVkInstance&)                 = delete;
    MoeVkInstance(MoeVkInstance&& other)
        :   _instance { std::exchange(other._instance, nullptr )},
            debugMessenger { std::exchange(other.debugMessenger, nullptr) }
    { }
    MoeVkInstance& operator = (const MoeVkInstance&)    = delete;
    MoeVkInstance& operator = (MoeVkInstance&&)         = delete;

    VkInstance& instance()   { return _instance; }

private:
    void createDebugMessenger();
    bool hasExtensions(const std::vector<const char*>& extensions) const;
    bool hasLayerSupport(const std::vector<const char*>& layers) const;

    VkInstance                  _instance;
    VkDebugUtilsMessengerEXT    debugMessenger;
};
}