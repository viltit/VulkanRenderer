#include "MoeVkRenderer.hpp"
#include "../Exceptions/InitException.hpp"
#include "VkWindow.hpp"

#include <iostream>
#include <SDL_vulkan.h>

#define __DEBUG__

namespace moe {

// TODO: Go on with selecting the QueueFamilies
MoeVkRenderer::MoeVkRenderer(VkWindow* window, RendererOptions options)
    :   instance        { window, options },
        surface         { VK_NULL_HANDLE }
{
    createSurface(window);
    physicalDevice.create(instance.instance(), surface);
    logicalDevice.create(instance.instance(), physicalDevice);
}

MoeVkRenderer::~MoeVkRenderer() {
    vkDestroySurfaceKHR(instance.instance(), surface, nullptr);
}

void MoeVkRenderer::createSurface(moe::VkWindow *window) {
    if (!window) {
        return;
    }

#ifndef NDEBUG
    std::cout << "<VulkanHandler> Creating surface ...";
#endif

    if (!SDL_Vulkan_CreateSurface(window->sdlWindow(), instance.instance(), &surface)) {
        // TODO: Error handling
        std::cout << "Could not create a vulkan surface for the window." << std::endl;
        std::cout << SDL_GetError() << std::endl;
    }

#ifndef NDEBUG
    std::cout << "\t\tfinsihed\n";
#endif
}


void MoeVkRenderer::findQueueFamilies(VkPhysicalDevice device) {

}
}