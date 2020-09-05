#include "MoeVkRenderer.hpp"
#include "../Exceptions/MoeExceptions.hpp"
#include "VkWindow.hpp"
#include "wrapper/MoeVkUtils.hpp"
#include "wrapper/MoeVkDescriptorSet.hpp"

#include <iostream>
#include <SDL_vulkan.h>

namespace moe {

MoeVkRenderer::MoeVkRenderer(VkWindow* window, std::vector<Drawable>& drawables, RendererOptions options)
    :   instance        { window, options },
        surface         { VK_NULL_HANDLE },
        window          { window },
        drawables       { drawables }
{

    const std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    createSurface(window);
    physicalDevice.create(instance.instance(), surface, extensions);
    logicalDevice.create(instance.instance(), physicalDevice, extensions);
    swapChain.create(physicalDevice, logicalDevice, surface, *window);

    uniformBuffer.createLayout(physicalDevice, logicalDevice);
    // TODO: numBuffers may change while the app is rendering -> do we need to reset the buffer ?
    uniformBuffer.createPool(physicalDevice, logicalDevice, swapChain.images().size() * drawables.size());

    std::vector<MoeVkShader> shaders { 2 };
    shaders[0].create(logicalDevice, "Shaders/triangle.vert.spv", MoeVkShaderStage::vertex);
    shaders[1].create(logicalDevice, "Shaders/triangle.frag.spv", MoeVkShaderStage::fragment);
    pipeline.prepare(shaders, 0, 0);
    pipeline.create(logicalDevice, physicalDevice, swapChain, uniformBuffer);
    commandPool.create(logicalDevice, physicalDevice.queueFamily(), pipeline, swapChain);
    framebuffer.create(logicalDevice, physicalDevice, swapChain, pipeline, commandPool);

    for (auto& drawable : drawables) {
        // TODO: No new -> need copy constrcutor
        vkDrawables.push_back(new MoeVkDrawable(
                physicalDevice, logicalDevice, commandPool, uniformBuffer,
                &drawable, swapChain.images().size()
                ));
    }

    commandBuffer.record(logicalDevice, framebuffer, pipeline, swapChain, commandPool,
            vkDrawables, VK_FALSE);

    // create semaphores:
    imageAvalaibleSemaphore.resize(maxFramesInFlight);
    renderFinishedSemaphore.resize(maxFramesInFlight);
    fences.resize(maxFramesInFlight);
    imagesInFlight.resize(swapChain.images().size());
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        imageAvalaibleSemaphore[i].create(logicalDevice);
        renderFinishedSemaphore[i].create(logicalDevice);
        fences[i].create(logicalDevice);
    }
}

void MoeVkRenderer::switchDebugDrawing(bool debugMode) {
    commandBuffer.record(
            logicalDevice,
            framebuffer,
            pipeline,
            swapChain,
            commandPool,
            vkDrawables,
            debugMode
            );
}

MoeVkRenderer::~MoeVkRenderer() {
    // because drawing command are executed asynchrounously, we need to wait until we destroy the resources:
    vkDeviceWaitIdle(logicalDevice.device());

    uniformBuffer.destroy(logicalDevice);

    for (size_t i = 0; i < vkDrawables.size(); i++) {
        delete vkDrawables[i];
    }
    vkDrawables.clear();

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        imageAvalaibleSemaphore[i].destroy(logicalDevice);
        renderFinishedSemaphore[i].destroy(logicalDevice);
        fences[i].destroy(logicalDevice);
    }
    commandPool.destroy(logicalDevice);
    framebuffer.destroy(logicalDevice);
    swapChain.destroy(logicalDevice);
    pipeline.destroy(logicalDevice);
    vkDestroySurfaceKHR(instance.instance(), surface, nullptr);
}

void MoeVkRenderer::draw() {

    fences[currentFrame].wait(logicalDevice, UINT64_MAX);

    /*  Acquire an image from the swap chain
        Execute the command buffer with that image as attachment in the framebuffer
        Return the image to the swap chain for presentation     */
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice.device(),
                                            swapChain.swapChain(),
                                            UINT64_MAX,
                                            imageAvalaibleSemaphore[currentFrame].semaphore(),
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    // check if the swapchain is out of date
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        // TODO: Maybe warn in case of suboptimal swapchain
        throw std::runtime_error("Failed to aquire a swapchain image.");
    }

    // check if a previous frame is using this image (ie, there is a fence for it)
    if(!imagesInFlight[imageIndex].isNull()) {
        vkWaitForFences(logicalDevice.device(), 1, &imagesInFlight[imageIndex].fence(), VK_TRUE, UINT64_MAX);
    }
    // mark the image as being used
    imagesInFlight[imageIndex] = fences[currentFrame];

    // Update uniforms
    // TODO: This should be part of a camera class
    glm::mat4 V = glm::lookAt(glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 0.f, 1.f });
    glm::mat4 P = glm::perspective(glm::radians(60.f), (float)swapChain.extent().width / (float)swapChain.extent().height, 0.01f, 10.f);

    // glm is made for openGL, where the y-Axis goes from bottom to top
    P[1][1] *= -1.f;

    for (auto& drawable : vkDrawables) {
        drawable->update(logicalDevice,
                V,
                P,
                glm::vec3{ 0.f, 3.f, -3.f },   // TODO: Not hardcode
                imageIndex);
    }

    VkSemaphore waitSemaphores[] = { imageAvalaibleSemaphore[currentFrame].semaphore() };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[currentFrame].semaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo { };
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &commandBuffer.buffer(imageIndex);
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    fences[currentFrame].reset(logicalDevice);

    if (vkQueueSubmit(logicalDevice.graphicsQueue(), 1, &submitInfo, fences[currentFrame].fence()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    // submit the resulting image back to the swap chain
    VkSwapchainKHR  swapchains[] = { swapChain.swapChain() };
    VkPresentInfoKHR presentInfo { };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // with several swapchains, we could get a VkResult for each of them

    result = vkQueuePresentKHR(logicalDevice.presentationQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image");
    }

    currentFrame = (currentFrame + 1) % maxFramesInFlight;
}

void MoeVkRenderer::recreateSwapChain() {
    vkDeviceWaitIdle(logicalDevice.device());
    cleanSwapchain();
    swapChain.create(physicalDevice, logicalDevice, surface, *window);
    // pipeline.create(logicalDevice, swapChain);
    framebuffer.create(logicalDevice, physicalDevice, swapChain, pipeline, commandPool);
    commandBuffer.record(logicalDevice, framebuffer, pipeline, swapChain, commandPool, vkDrawables, VK_FALSE);
}

void MoeVkRenderer::cleanSwapchain() {
    // TODO: Also destroy and recreate uniform buffer
    framebuffer.destroyBuffers(logicalDevice);
    // pipeline.destroy(logicalDevice);
    // TODO: Command buffers ?

    swapChain.destroy(logicalDevice);
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


}