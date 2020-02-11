#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkInstance.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkPipeline.hpp"
#include "MoeVkFramebuffer.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

private:
    void createSurface(VkWindow* window);

    MoeVkInstance               instance;
    MoeVkSwapChain              swapChain;
    MoeVkPipeline               pipeline;
    MoeVkFramebuffer            framebuffer;
    VkSurfaceKHR                surface;

    MoeVkPhysicalDevice         physicalDevice;
    MoeVkLogicalDevice          logicalDevice;
};
}