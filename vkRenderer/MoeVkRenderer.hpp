#include <vulkan/vulkan.h>
#include <vector>

#include "wrapper/MoeVkPhysicalDevice.hpp"
#include "wrapper/MoeVkLogicalDevice.hpp"
#include "wrapper/MoeVkInstance.hpp"
#include "wrapper/MoeVkSwapChain.hpp"
#include "wrapper/MoeVkPipeline.hpp"
#include "wrapper/MoeVkFramebuffer.hpp"
#include "wrapper/MoeVkCommandPool.hpp"
#include "wrapper/MoeVkSemaphore.hpp"
#include "wrapper/MoeVkFence.hpp"
#include "wrapper/MoeVkArrayBuffer.hpp"
#include "MoeDrawable.hpp"
#include "wrapper/MoeVkDescriptorPool.hpp"
#include "wrapper/MoeTexture.hpp"
#include "wrapper/MoeDepthTexture.hpp"
#include "wrapper/MoeVkRenderPass.hpp"
#include "MoeVkDrawable.hpp"
#include "MoeVkRendererCommandBuffer.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, std::vector<Drawable>& drawables, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

    void draw();
    void recreateSwapChain();

    void switchDebugDrawing(bool debugMode);

private:

    static constexpr int maxFramesInFlight = 2;

    void createSurface(VkWindow* window);
    void cleanSwapchain();

    VkWindow*                   window;

    MoeVkInstance               instance;
    MoeVkSwapChain              swapChain;
    MoeVkRenderPass             renderPass;
    std::vector<MoeVkPipeline>  pipelines;
    MoeVkFramebuffer            framebuffer;
    MoeVkCommandPool            commandPool;
    MoeVkRendererCommandBuffer  commandBuffer;
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

    // TODO: This should be in an own class "scene" in the long termn
    std::vector<Drawable>&          drawables;
    std::vector<MoeVkDrawable*>      vkDrawables;

    MoeVkDescriptorPool              uniformBuffer;
};
}