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

    // TODO: Do NOT use 'new' ONLY because you are to proud to make a 'create' function instead of a huge constructor
    MoeVkPhysicalDevice*        physicalDevice;
    MoeVkLogicalDevice          logicalDevice;
};
}