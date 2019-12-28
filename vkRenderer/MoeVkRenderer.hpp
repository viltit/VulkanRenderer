#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkInstance.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

private:
    void createSurface(VkWindow* window);
    void findQueueFamilies(VkPhysicalDevice device);

    MoeVkInstance               instance;
    VkSurfaceKHR                surface;

    MoeVkPhysicalDevice         physicalDevice;
    MoeVkLogicalDevice          logicalDevice;
};
}