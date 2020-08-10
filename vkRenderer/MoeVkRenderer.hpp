#include <vulkan/vulkan.h>
#include <vector>

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkInstance.hpp"
#include "MoeVkSwapChain.hpp"
#include "MoeVkPipeline.hpp"
#include "MoeVkFramebuffer.hpp"
#include "MoeVkCommandPool.hpp"
#include "MoeVkSemaphore.hpp"
#include "MoeVkFence.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

    void draw();

private:

    static constexpr int maxFramesInFlight = 2;

    void createSurface(VkWindow* window);

    MoeVkInstance               instance;
    MoeVkSwapChain              swapChain;
    MoeVkPipeline               pipeline;
    MoeVkFramebuffer            framebuffer;
    MoeVkCommandPool            commandPool;

    VkSurfaceKHR                surface;

    MoeVkPhysicalDevice         physicalDevice;
    MoeVkLogicalDevice          logicalDevice;

    // we need several semaphores to lock swapchain images separatedly and therefore allow "frames in flight"
    std::vector<MoeVkSemaphore>   imageAvalaibleSemaphore;
    std::vector<MoeVkSemaphore>   renderFinishedSemaphore;

    // ensure that there are no more than two frames of work enqueued
    // and that these frames are not accidentally using the same image
    std::vector<MoeVkFence>       imagesInFlight;
    std::vector<MoeVkFence>       fences;

    // keep track of the frame
    size_t currentFrame = 0;

};
}