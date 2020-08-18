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
#include "wrapper/MoeVkUniformBuffer.hpp"
#include "wrapper/MoeVkImage.hpp"

namespace moe {

// forward declaration:
class VkWindow;

class MoeVkRenderer {

public:
    MoeVkRenderer(VkWindow* window, Drawable& drawable, RendererOptions options = RendererOptions::none);
    ~MoeVkRenderer();

    void draw();
    void recreateSwapChain();

private:

    static constexpr int maxFramesInFlight = 2;

    void createSurface(VkWindow* window);
    void cleanSwapchain();

    VkWindow*                   window;

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

    // TODO: This should be in an own class "scene" in the long termn
    Drawable&                       drawable;

    MoeVkArrayBuffer<Vertex>*       vertexBuffer;
    MoeVkArrayBuffer<uint32_t>*     indexBuffer;
    MoeVkUniformBuffer              uniformBuffer;

    // TODO: This should be part of a camera class
    glm::mat4                       VP;

    // TODO: This will be part of a "MoeTexture" class
    MoeVkImage                           image;
    void loadTexture(/*filename*/);
};
}