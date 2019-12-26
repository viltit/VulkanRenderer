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
    void createSurface(VkWindow* window);
    void findQueueFamilies(VkPhysicalDevice device);

    MoeVkInstance               instance;
    VkSurfaceKHR                surface;
    MoePhysicalDevice*          physicalDevice;
};
}