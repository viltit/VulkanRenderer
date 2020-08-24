
#include "MoeVkImage.hpp"
#include "../../Exceptions/InitException.hpp"

#include "MoeVkPhysicalDevice.hpp"
#include "MoeVkLogicalDevice.hpp"
#include "MoeVkUtils.hpp"

#include <spdlog/spdlog.h>

namespace moe {

    MoeVkImage::MoeVkImage()
        :   _image   { VK_NULL_HANDLE },
            _device  { nullptr }

    {}

    MoeVkImage::~MoeVkImage() {
        destroy();
    }

    void MoeVkImage::create(MoeVkLogicalDevice &device,
            MoeVkPhysicalDevice &physicalDevice,
            uint32_t w, uint32_t h,
            VkFormat format, VkImageTiling tiling,
            VkImageLayout layout,
            VkImageUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryFlags)
    {
        spdlog::trace("Creating MoeVkImage");
        _device = &device;
        _layout = layout;
        VkImageCreateInfo imageInfo { };
        imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext             = nullptr;
        imageInfo.flags             = 0;
        imageInfo.imageType         = VK_IMAGE_TYPE_2D;
        imageInfo.format            = format;
        imageInfo.extent.width      = w;
        imageInfo.extent.height     = h;
        imageInfo.extent.depth      = 1;
        imageInfo.mipLevels         = 1;  // TODO: Mipmapping
        imageInfo.arrayLayers       = 1;
        imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling            = tiling;
        imageInfo.usage             = usageFlags;
        imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.queueFamilyIndexCount = 0;
        imageInfo.pQueueFamilyIndices = nullptr;
        imageInfo.initialLayout     = layout;

        if (vkCreateImage(device.device(), &imageInfo, nullptr, &_image) != VK_SUCCESS) {
            throw InitException("Failed to create image", __FILE__, __FUNCTION__, __LINE__);
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device.device(), _image, &memoryRequirements);
        VkMemoryAllocateInfo memoryInfo { };
        memoryInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryInfo.pNext            = nullptr;
        memoryInfo.allocationSize   = memoryRequirements.size;
        memoryInfo.memoryTypeIndex  = MoeVkUtils::getMemoryType(physicalDevice, memoryRequirements.memoryTypeBits,
                                                                memoryFlags);
        if (vkAllocateMemory(device.device(), &memoryInfo, nullptr, &_memory) != VK_SUCCESS) {
            throw InitException("Failed to allocate memory for texture", __FILE__, __FUNCTION__, __LINE__);
        }

        // bind memory to image
        // offset: We could put several images into one block of image memory
        vkBindImageMemory(device.device(), _image, _memory, 0);
    }

void MoeVkImage::destroy() {
    spdlog::trace("Destroying MoeVkImage...");
    if (_device) {
        if (_view != VK_NULL_HANDLE) {
            vkDestroyImageView(_device->device(), _view, nullptr);
            _view = VK_NULL_HANDLE;
        }
        if (_image != VK_NULL_HANDLE) {
            vkDestroyImage(_device->device(), _image, nullptr);
            _image = VK_NULL_HANDLE;
        }
        if (_memory != VK_NULL_HANDLE) {
            vkFreeMemory(_device->device(), _memory, nullptr);
            _memory = VK_NULL_HANDLE;
        }
    }
    spdlog::trace("Finished.");
}

void MoeVkImage::createView(VkFormat format, VkImageAspectFlags aspectFlags) {

    // TODO: Make sure we have a device and an image

    VkImageViewCreateInfo viewCreateInfo { };
    viewCreateInfo.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.pNext        = nullptr;
    viewCreateInfo.flags        = 0;
    viewCreateInfo.image        = _image;
    viewCreateInfo.viewType     = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format       = format;
    // we could switch channels here:
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // TODO: Same code as in function "setLayout"
    viewCreateInfo.subresourceRange.aspectMask      = aspectFlags;
    viewCreateInfo.subresourceRange.baseMipLevel    = 0;
    viewCreateInfo.subresourceRange.levelCount      = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer  = 0;
    viewCreateInfo.subresourceRange.layerCount      = 1;

    if (vkCreateImageView(_device->device(), &viewCreateInfo, nullptr, &_view) != VK_SUCCESS) {
        throw InitException("Failed to create image view", __FILE__, __FUNCTION__, __LINE__);
    }
}

void MoeVkImage::changeLayout(MoeVkCommandPool &commandPool,
        VkQueue queue, VkFormat format, VkImageLayout newLayout)
{
    assert(_image != VK_NULL_HANDLE);

    MoeVkCommandBuffer buffer { };
    buffer.create(*_device, commandPool, 1);
    buffer.startRecording(commandPool, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 0);

    // start layouting the image. This happens with so called "image-memory-barrier".
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destStage;
    VkImageMemoryBarrier barrier;
    barrier.sType           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext           = nullptr;
    VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    // transition from preloaded image to image an image we can copy into
    if (_layout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
        // transition from image we copied into to shader optimized image
    else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    else if (_layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (MoeVkUtils::isStencilFormat(format)) {
            aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else {
        throw InitException("Invalid layout transition", __FILE__, __FUNCTION__, __LINE__);
    }
    barrier.oldLayout       = _layout;
    barrier.newLayout       = newLayout;
    // we could change the queue an image belongs to here
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = _image;
    // TODO: Use mipmapping
    barrier.subresourceRange.aspectMask     = aspectFlags;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(buffer.at(0),
                         sourceStage,
                         destStage,
                         0, 0, nullptr,
                         0, nullptr,
                         1,
                         &barrier);

    buffer.stopRecording(queue, commandPool, 0);

    // TODO: Wait for fence ??  --> Would make sence if we access this class in a multihtreaded environment
    vkQueueWaitIdle(queue);
    buffer.destroy();
    _layout = newLayout;
}
}