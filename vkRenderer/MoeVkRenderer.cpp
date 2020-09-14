#include "MoeVkRenderer.hpp"
#include "MoeExceptions.hpp"
#include "VkWindow.hpp"
#include "wrapper/MoeVkUtils.hpp"
#include "wrapper/MoeVkDescriptorSet.hpp"

#include <iostream>
#include <SDL_vulkan.h>

namespace moe {

// TODO Remove - maybe make part of RenderOptions
const std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

MoeVkRenderer::MoeVkRenderer(VkWindow* window, std::vector<Drawable>& drawables, RendererOptions options)
    :   window          { window },
        instance        { window, options },
        surface         { instance, *window },
        drawables       { drawables },
        physicalDevice  { instance.instance(), surface.surface(), extensions },
        logicalDevice   { instance.instance(), physicalDevice, extensions },
        swapChain       { physicalDevice, logicalDevice, surface.surface(), *window },
        descriptorPool  { physicalDevice, logicalDevice, swapChain.images().size() * drawables.size() },
        renderPass      { logicalDevice, physicalDevice, swapChain }
{

    std::vector<MoeVkShader> shaders { 2 };
    shaders[0].create(logicalDevice, "Shaders/triangle.vert.spv", MoeVkShaderStage::vertex);
    shaders[1].create(logicalDevice, "Shaders/triangle.frag.spv", MoeVkShaderStage::fragment);
    pipelines.reserve(2);
    pipelines.push_back(MoeVkPipeline());
    pipelines[0].prepare(shaders, swapChain.extent().width, swapChain.extent().height);
    pipelines[0].create(logicalDevice, physicalDevice, swapChain, renderPass, descriptorPool);
    std::vector<MoeVkShader> wireframeShaders { 2 };
    wireframeShaders[0].create(logicalDevice, "Shaders/simple.vert.spv", MoeVkShaderStage::vertex);
    wireframeShaders[1].create(logicalDevice, "Shaders/simple.frag.spv", MoeVkShaderStage::fragment);
    pipelines.push_back(MoeVkPipeline());
    pipelines[1].prepare(wireframeShaders,
            swapChain.extent().width, swapChain.extent().height);
    pipelines[1].rasterizationCreateInfo().polygonMode = VK_POLYGON_MODE_LINE;
    pipelines[1].create(logicalDevice, physicalDevice, swapChain, renderPass, descriptorPool);

    // set correct rendering viewports for the pipelines:
    pipelines[0].setRenderingViewport(swapChain.extent().width / 2, swapChain.extent().height, 0, 0);
    pipelines[1].setRenderingViewport(swapChain.extent().width / 2, swapChain.extent().height, swapChain.extent().width / 2, 0);


    commandPool.create(logicalDevice, physicalDevice.queueFamily(), swapChain);
    framebuffer.create(logicalDevice, physicalDevice, swapChain, renderPass, commandPool);

    for (auto& drawable : drawables) {
        // TODO: No new -> need copy constrcutor
        vkDrawables.push_back(new MoeVkTexturedDrawable(
                physicalDevice, logicalDevice, commandPool, descriptorPool,
                &drawable, swapChain.images().size()
                ));
    }

    commandBuffer.record(logicalDevice, framebuffer, renderPass, pipelines, swapChain, commandPool,
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
            renderPass,
            pipelines,
            swapChain,
            commandPool,
            vkDrawables,
            debugMode
            );
}

MoeVkRenderer::~MoeVkRenderer() {
    // because drawing command are executed asynchrounously, we need to wait until we destroy the resources:
    vkDeviceWaitIdle(logicalDevice.device());

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
    for (auto& pipeline : pipelines) {
        pipeline.destroy();
    }
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
    // TODO: Automaticly adapt perspective matrix to respective rendering viewport ?
    glm::mat4 V = glm::lookAt(glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 0.f, 1.f });
    glm::mat4 P = glm::perspective(glm::radians(60.f), (float)swapChain.extent().width / (2.f * (float)swapChain.extent().height), 0.01f, 10.f);

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
    // TODO: Make sure we do this right (ie, no memory leak and not unnecessary performance loss by re-building too much
    vkDeviceWaitIdle(logicalDevice.device());
    cleanSwapchain();
    swapChain.recreate(physicalDevice, *window, surface.surface());
    // pipeline.create(logicalDevice, swapChain);
    // set correct rendering viewports for the pipelines:
    pipelines[0].setRenderingViewport(swapChain.extent().width / 2, swapChain.extent().height, 0, 0);
    pipelines[1].setRenderingViewport(swapChain.extent().width / 2, swapChain.extent().height, swapChain.extent().width / 2, 0);

    framebuffer.create(logicalDevice, physicalDevice, swapChain, renderPass, commandPool);
    commandBuffer.record(logicalDevice, framebuffer, renderPass, pipelines, swapChain, commandPool, vkDrawables, VK_FALSE);
}

void MoeVkRenderer::cleanSwapchain() {
    // TODO: Also destroy and recreate uniform buffer ?
    framebuffer.destroyBuffers(logicalDevice);
    // pipeline.destroy(logicalDevice);
    // TODO: Command buffers ?
}
}