#include <vulkan/vulkan.h>
#include <vector>

#include "MoePhysicalDevice.hpp"
#include "MoeVkInstance.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

private:
    void createInstance(VkWindow* window, RendererOptions options);
    void createSurface(VkWindow* window);
    void createDebugMessenger();
    void findQueueFamilies(VkPhysicalDevice device);

    bool hasExtensions(const std::vector<const char*>& extensions) const;
    bool hasLayerSupport(const std::vector<const char*>& layers) const;

    VkInstance                  instance;
    VkSurfaceKHR                surface;
    VkDebugUtilsMessengerEXT    debugMessenger;
    MoePhysicalDevice*          physicalDevice;
};
}